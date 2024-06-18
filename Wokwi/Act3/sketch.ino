// Librerias
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <IRremote.h>  //Incluir libreias del mando.
#include <SPI.h>
#include <Fuzzy.h> // Incluye la librería de lógica difusa


//----------------DHT----------------//
#define DHTTYPE DHT22
#define DHTPIN 3 //Definición del pin como entrada del sensor DHT
DHT dht(DHTPIN, DHTTYPE); //inicializa el sensor DHT
float temperatura = 0.0; //Se definen temperatura y humedad para almacenar lo medido por el sensor
float humedad = 0.0;
float temperatura_anterior = 0.0; 
float humedad_anterior = 0.0;
//--------------------------------//

//-----------------LDR------------------//
#define LDRPIN A0 //Pin entrada 0 analógico
int ldrVal = 0;
float lux = 0.0;
float lux_anterior;
#define NUM_LEDS 8
#define PINSW A3
int leds_encendidos = 0;
bool primera_medida = true;
const int ledPins[NUM_LEDS] = {13, 12, 11, 9, 8, 5, 4, 2}; // Pines del conjunto de 8 leds

//----------LED RGB----------//
//Pines led RGB
#define PINRED 13
#define PINGREEN 12
#define PINBLUE 11
//-------------------------------------//

//-------------LCD---------------//
int lcdAddress = 0x27; // Puede variar según tu módulo I2C
LiquidCrystal_I2C lcd(lcdAddress, 16, 2);
//Simbolos LCD Humnedad
byte humidity[8] = {B00100, B00100, B01010, B01010, B10001, B10001, B10001, B01110};
//Simbolo LCD Temperatura
byte temp[8] = {B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110};
//Simbolo LCD iluminacion
byte ilum[8] = {B00000, B10101, B01110, B11011, B01110, B10101, B00000, B00000};
//Simbolo sensor HC-SR04
byte presence[8] = {B01110, B01110, B00100, B11111, B00100, B01110, B01010, B01010};

//----------Sensor de presencia----------//
//Pines del sensor SR04
#define PINTRIG 5
#define PINECHO 6
//Variables para almacenamiento y procesamiento de las mediciones
float duracion, distanciaanterior;
float distancia = 0;
//-----------------------------------//

//-------Sensor temperatura externa-------//
#define sensorPin 7 //Definición del pin
float temp_externa_anterior, temp_interna_anterior;
float temp_externa_actual = 0;
float temp_interna_actual = 0;

OneWire oneWire(sensorPin);
DallasTemperature sensors(&oneWire);
//Temperatura objetivo del control y limites fijados
float temp_objetivo = 25;
float LimInf = temp_objetivo - 3;
float LimSup = temp_objetivo + 3;
int ventilar = 0;
boolean vuelta = false;
//----------------------------------//

//----------Servomotor------------------//
Servo myservo;
#define SERVOPIN 10
int pos = 0;
//--------------------------------------//

//---------IP REMOTE---------------//
int modo = 0; //modo 0, todo apagado
#define PIN_RECEIVER 16   // Define el pin2 con el nombre Pin_receiver.
IRrecv receiver(PIN_RECEIVER); //Define el pin_reciver como receptor de los datos

//-------------Lógica difusa----------------//
Fuzzy *fuzzy = new Fuzzy();
Fuzzy *fuzzy_TyH = new Fuzzy();
//-------------------------------------------//


void setup() {
  // Inicializar serial
  Serial.begin(9600);
  Serial.println("Iniciando el sistema");

  // Inicializar el sensor DHT
  dht.begin();

  // Configurar pin LDR como entrada
  pinMode(LDRPIN, INPUT);

  //Configuración del pin del switch
  pinMode(PINSW, INPUT_PULLUP);

  // Definición de pines del LED RGB como salida
  pinMode(PINRED, OUTPUT);
  pinMode(PINGREEN, OUTPUT);
  pinMode(PINBLUE, OUTPUT);

  //Configura los pines de los 8 ldes como salida
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Configuración pines del sensor de presencia
  pinMode(PINTRIG, OUTPUT);
  pinMode(PINECHO, INPUT);
  digitalWrite(PINTRIG, LOW);

  // Inicialización del sensor de temperatura
  sensors.begin();
  delay(20);

  // Inicialización del servomotor
  myservo.attach(SERVOPIN);

  // Inicialización de la LCD
  lcd.init();
  lcd.createChar(3, humidity);
  lcd.createChar(5, temp);
  lcd.createChar(7, ilum);
  lcd.createChar(9, presence);
  

  // Configuración de la lógica difusa a para la iluminacion
  configuracionIluminacionFuzzy();
  // Configuración de la lógica difusa a para la temperatura e iluminacion
  configuracionTyHFuzzy();

  receiver.enableIRIn(); // Comando para iniciar el receptor de infrarrojos.

  
}

void loop() {
  // Tratamiento del pulsación del botón

  // Checks received an IR signal
  if (receiver.decode()) { //Si se recibe una señal por el pin_receiver (2) realiza:
    translateIR();        //La funcion translateIR(). (Mas abajo definiremos esta función)
    receiver.resume();   //La receiver.resume(). (Mas abajo definiremos esta función)
  }
 

  // Control de la LCD y modos
  if (modo == 1) { // Modo 1: Humedad y Temperatura
    temperatura_anterior = temperatura;
    humedad_anterior = humedad;
    temperatura = dht.readTemperature();
    humedad = dht.readHumidity();

    if(temperatura_anterior != temperatura || humedad_anterior != humedad){
      fuzzy_TyH->setInput(1, temperatura);
      fuzzy_TyH->setInput(2, humedad);
      fuzzy_TyH->fuzzify();

      float comfort_level = fuzzy_TyH->defuzzify(1);
      Serial.println(comfort_level);
      controlarConfort(comfort_level);

      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperatura, 1);
      lcd.print(" C");

      lcd.setCursor(0, 1);
      lcd.print("Hum: ");
      lcd.print(humedad, 1);
      lcd.print(" %");
      lcd.display();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.write(5);
      lcd.setCursor(2, 0);
      lcd.print(temperatura, 1);
      lcd.print(" C");

      lcd.setCursor(0, 1);
      lcd.write(3);
      lcd.setCursor(2, 1);
      lcd.print(humedad, 1);
      lcd.print(" %");
    }
  }

  if (modo == 2) { // Modo 2: Iluminación
    ldrVal = analogRead(LDRPIN);
    lux = map(ldrVal, 0, 1023, 0, 1000);

    //Logica difusa
    fuzzy->setInput(1, lux);
    fuzzy->fuzzify();
    float numLeds = fuzzy->defuzzify(1);

    //control de leds
    encenderLeds(numLeds);

    //muestra info y actualizacion de la luz
    mostrarIluminacion(lux, lux_anterior);
    lux_anterior = lux;

    delay(200);
  }

  if (modo == 3) { // Modo 3: Presencia
    distanciaanterior = distancia;
    distancia = medirDistancia();

    if (distancia > distanciaanterior + 3 || distancia < distanciaanterior - 3) {
      lcd.display();
      lcd.backlight();
      LimpiezaLCD();
      if (distancia < 200 && distancia > 2) {
        BrilloLed(255, 0, 0);
        lcd.setCursor(0, 0);
        lcd.write(9);
        lcd.setCursor(2, 0);
        lcd.print(distancia, 1);
        lcd.print(" cm");
        delay(10);
      } else {
        lcd.setCursor(0, 0);
        lcd.write(9);
        lcd.setCursor(2, 0);
        lcd.print(" No presencia");
        BrilloLed(0, 0, 0);
        delay(10);
      }
    }
  }

  if (modo == 4) { // Modo 4: Control de Temperatura
    temp_externa_anterior = temp_externa_actual;
    temp_interna_anterior = temp_interna_actual;
    temp_interna_actual = dht.readTemperature();
    sensors.requestTemperatures();
    temp_externa_actual = sensors.getTempCByIndex(0);

    distanciaanterior = distancia;
    distancia = medirDistancia();

    if (temp_externa_actual != temp_externa_anterior || temp_interna_actual != temp_interna_anterior || distancia > distanciaanterior + 3 || distancia < distanciaanterior - 3) {
      mostrar_temperaturas();
      float temp_media = (temp_externa_actual + temp_interna_actual) / 2;
      Serial.print("Temperatura media: ");
      Serial.println(temp_media);
      if (temp_interna_actual > LimSup) {
        BrilloLed(255, 0, 0);
        if (temp_media < temp_interna_actual) {
          myservo.write(180);
          ventilar = 1;
          Serial.println("Abrimos ventilación para refrescar la habitación");
        } else {
          myservo.write(0);
          ventilar = 0;
          Serial.println("Fuera hace demasiado calor, cerramos ventilación");
        }
      } else if (temp_interna_actual < LimInf) {
        BrilloLed(0, 0, 255);
        if (ventilar == 0 && temp_media >= temp_interna_actual) {
          myservo.write(90);
          ventilar = 1;
          Serial.println("Abrimos ventilación para calentar la habitación, fuera hace suficiente calor");
        }
        if (ventilar == 1 && temp_media < temp_interna_actual) {
          myservo.write(0);
          ventilar = 0;
          Serial.println("Ventalación abierta pero hace demasiado frio fuera: Cerramos");
        }
      }
      if (temp_interna_actual <= LimSup && temp_interna_actual >= LimInf) {
        BrilloLed(0, 255, 0);
        if (distancia >= 200 || distancia <= 2) {
          myservo.write(45);
          Serial.println("Temperatura ideal: Ventilamos el cuarto (45ª)");
        } else {
          Serial.println("Cerramos ventilación");
          myservo.write(0);
        }
      }
    }
  }

  if (modo == 0) { // Modo 0: Apagado
    BrilloLed(0, 0, 0);
    LimpiezaLCD();
    lcd.noBacklight();
  }
}

// Funciones auxiliares
void LimpiezaLCD() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 2);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("                ");
}

void BrilloLed(int rojo, int verde, int azul) {
  analogWrite(PINRED, rojo);
  analogWrite(PINGREEN, verde);
  analogWrite(PINBLUE, azul);
}

long medirDistancia() {
  digitalWrite(PINTRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINTRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINTRIG, LOW);
  long duration = pulseIn(PINECHO, HIGH);
  long distance = duration * 0.034 / 2;
  if (distance > 200) {
    distance = 200;
  } else if (distance < 2) {
    distance = 2;
  }
  return distance;
}

void mostrar_temperaturas() {
  lcd.display();
  lcd.backlight();
  LimpiezaLCD();
  lcd.setCursor(0, 0);
  lcd.write(5);
  lcd.setCursor(2, 0);
  lcd.print("Exter: ");
  lcd.print(temp_externa_actual, 1);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.setCursor(2, 1);
  lcd.print("Int: ");
  lcd.print(temp_interna_actual, 1);
  lcd.print(" C");
}

void encenderLeds(float numLeds) {
  Serial.println("Leds de la fuzzy");
  Serial.println(numLeds);
  int ledsToTurnOn = (int)round(numLeds);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < ledsToTurnOn) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
}

void mostrarIluminacion(float lux, float lux_anterior) {
  if (lux_anterior != lux) {
    lcd.display();
    lcd.backlight();
    LimpiezaLCD();
    lcd.setCursor(0, 0);
    lcd.write(7);
    lcd.setCursor(2, 0);
    lcd.print(lux, 1);
    lcd.print(" %");
  }
}




void translateIR()
{
  // Takes command based on IR code received
  //Gestión de modos mediante un el mando: Pulsa el 1 te lleva al modo 1, pulsar el 2 al modo 2, el tres al modo 3
  //pulsar el 4 al modo 4 y el boton de apagado lleva al modo 0
  switch (receiver.decodedIRData.command) {
    case 162:
      Serial.println("POWER");//Modo apagado
      if(modo !=0){
        modo = 0;
        BrilloLed(0, 0, 0);//Apagado led rgb
        leds_encendidos = 0;//Apagamos los ldes del modo iluminación
        myservo.detach(); 
        Serial.println("Entrada en Modo 0: Sistema apagado");
      }
      break;
    case 48:
    //If y else para controlar el puslado consecutivo del mismo boton
    //Si el usuario pulsa el mismo boton para entrar al modo en el que ya esta 
    //Se continua como si nada y se avisa de que ya esta en ese modo
      if(modo !=1){
        Serial.println("Entrada en Modo 1: Humedad y Temperatura");
        modo = 1;
        temperatura = 0;
        humedad = 0;
         BrilloLed(0, 0, 0);
        myservo.attach(SERVOPIN);//Conectamos el servo
      }
      else{
        Serial.println("Ya se encuentra en el Modo 1");
      }
      break;
    case 24:
    //Misma logica de if y else de pulsado consecutivo para el resto de modos
      if(modo != 2){
        modo = 2;
        ldrVal = 0;
        lux = 0.0;
        lux_anterior = 0.0;
        leds_encendidos = 0;
         BrilloLed(0, 0, 0);
        primera_medida = true;
        myservo.detach();//Desactivamos el servo
        LimpiezaLCD();
        Serial.println("Entrada en Modo 2: Iluminacion");
      }
      else{
        Serial.println("Ya se encuentra en el Modo 2");
      }
      break;
    case 122:
      
      if(modo !=3){
        modo = 3;
        distancia = 0;
        LimpiezaLCD();
        Serial.println("Entrada en Modo 3: Presencia");
        myservo.detach();//Desactivamos el servo
      }
      else{
         Serial.println("Ya se encuentra en el Modo 3");
      }
      
      break;
    case 16:
      if(modo !=4){
        modo = 4;
        temp_interna_actual = 0;
        temp_externa_actual = 0;
        ventilar = 0;
        distancia = 0;
        BrilloLed(0, 0, 0);
        myservo.attach(SERVOPIN);//Conectamos el servo
        LimpiezaLCD();
      Serial.println("Entrada en Modo 4: Control de Temperatura");
      }
      else{
        Serial.println("Ya se encuentra en el Modo 4");
      }
     
      break;
    default:
      //lcd.clear();
      Serial.println(receiver.decodedIRData.command);
      Serial.println(" other button");
  }
}

void controlarConfort(float comfort_level) {
  if (comfort_level < 50) {
    BrilloLed(255, 0, 0); // Rojo - Inconfortable
    myservo.write(180); // Abrir ventilación
  } else {
    BrilloLed(0, 255, 0); // Verde - Confortable
    myservo.write(0); // Cerrar ventilación
  }
}
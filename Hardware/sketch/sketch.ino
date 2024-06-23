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
#define DHTTYPE DHT11
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
int modoControl = 0; //0 es control al 80%, 1 control con la fuzzy
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
//Fuzzy *fuzzy_TyH = new Fuzzy();
//-------------------------------------------//


void setup() {
  // Inicializar serial
  Serial.begin(9600);
  Serial.println("Iniciando el sistema");


  // Inicializar el sensor DHT
  dht.begin();

  // Configurar pin LDR como entrada
  pinMode(LDRPIN, INPUT);

  // Definición de pines del LED RGB como salida
  pinMode(PINRED, OUTPUT);
  pinMode(PINGREEN, OUTPUT);
  pinMode(PINBLUE, OUTPUT);

  //Configura los pines de los 8 leds como salida
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
      fuzzy->setInput(1, temperatura);
      fuzzy->setInput(2, humedad);
      fuzzy->fuzzify();

      float servo_position = fuzzy->defuzzify(3);
      Serial.println("Servo Position: ");
      Serial.println(servo_position);
      myservo.write(servo_position);

      //Se muestra la temperatura y humedad medida del DHT en el monitor serie
      Serial.println("Temperatura y Humedad medidas:");
      Serial.print("Temperatura: ");
      Serial.print(temperatura, 1);
      Serial.println(" C");
      Serial.print("Humedad: ");
      Serial.print(humedad, 1);
      Serial.println(" %");
      delay(1000)
    }
  }

  if (modo == 2) { // Modo 2: Iluminación
    if (modoControl == 0){
      //Control de iluminación al 80%
      int ldrValue = analogRead(LDRPIN);
  
      // Convertir el valor del LDR a un porcentaje de iluminación (0-100%)
      //Para lograr visualizar el efecto del control al 80% se parte de una medida tomada
      //por el sensor, el resto de iteraciones usará como medida de lux un incremento
      //del valor medido para simular un incremento de la luex al haber encendido leds
      if(primera_medida==true){
        lux_anterior = lux;
        lux = map(ldrValue, 0, 1023, 100, 0);
        primera_medida=false;
      }
    
      mostrarIluminacion(lux, lux_anterior);
      lux = controlarIluminacion(lux) ;
      
      // Esperar un poco antes de la próxima lectura
      delay(200);
    } else {
      ldrVal = analogRead(LDRPIN);
      lux = map(ldrVal, 0, 1023, 0, 1000);//La luz se mide de 0 a 1000
      //Logica difusa para control de iluminacion
      fuzzy->setInput(1, lux);
      fuzzy->fuzzify();
      float numLeds = fuzzy->defuzzify(1);

      //control de leds
      encenderLeds(numLeds);
    }
    
    //muestra info y actualizacion de la luz
    mostrarIluminacion(lux, lux_anterior);
    lux_anterior = lux;

    delay(200);
  }

  if (modo == 3) { // Modo 3: Presencia
    distanciaanterior = distancia;
    distancia = medirDistancia();

    if (distancia > distanciaanterior + 3 || distancia < distanciaanterior - 3) {
      if (distancia < 200 && distancia > 2) {
        BrilloLed(255, 0, 0);
        Serial.print("Distancia: ");
        Serial.print(distancia, 1);
        Serial.println(" cm");
      } else {
        Serial.println("No presencia");
        BrilloLed(0, 0, 0);
      }
      delay(10);
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
    Serial.println("Sistema Apagado");
    delay(10000);
  }
}

// Funciones auxiliares
void LimpiezaLCD() {
  // Esta función no es necesaria para el monitor serie
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
  Serial.print("Temp Externa: ");
  Serial.print(temp_externa_actual, 1);
  Serial.println(" C");
  Serial.print("Temp Interna: ");
  Serial.print(temp_interna_actual, 1);
  Serial.println(" C");
}

void encenderLeds(float numLeds) {
  Serial.print("Leds de la fuzzy: ");
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
    Serial.print("Iluminacion: ");
    Serial.print(lux, 1);
    if(modoControl==0){
      Serial.println(" %");
    } else {
      Serial.println();
    }
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

float controlarIluminacion(float lux) {
  // Mostrar el porcentaje calculado en el monitor serie
  Serial.print(" - Iluminacion: ");
  Serial.print(lux);
  Serial.println("%");
  
  // Calcular la diferencia respecto al umbral deseado
  int difference = lux - 80;
    
  // Determinar la cantidad de LEDs a encender
  int ledsToTurnOn = NUM_LEDS * lux / 100;
  Serial.print("Leds a encender:");
  Serial.println(ledsToTurnOn);

  // Ajustar la cantidad de LEDs encendidos para mantener la iluminación al 80%
  if (difference < -2) {
    ledsToTurnOn++;
    //Esto de aquí es para simular el efecto de que aumenta la iluminación al encender un led más
    lux_anterior = lux;
    lux= lux+2;//Para simular efecto del control
  } else if (difference > 2) {
    ledsToTurnOn--;

    //Esto lo mismo pero para simular que reduce la luz al apagar uno más
    lux_anterior = lux;
    lux=lux-2;//Para simular el efecto del control
  }
  
  // Limitar el número de LEDs a encender entre 0 y NUM_LEDS
  ledsToTurnOn = constrain(ledsToTurnOn, 0, NUM_LEDS);
  
  // Encender/Apagar los LEDs según sea necesario
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < ledsToTurnOn) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
  return lux;
}

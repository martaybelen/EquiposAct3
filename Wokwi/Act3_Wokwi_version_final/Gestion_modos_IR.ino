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
        encenderLeds(0);
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
         encenderLeds(0);
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
        encenderLeds(0);
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
        encenderLeds(0);
        myservo.attach(SERVOPIN);//Conectamos el servo
        LimpiezaLCD();
      Serial.println("Entrada en Modo 4: Control de Temperatura");
      }
      else{
        Serial.println("Ya se encuentra en el Modo 4");
      }
     
      break;
    case 176:
    //Conmutacion entre modo control iluminacion 80% y control con fuzzy
    primera_medida = true;
    if (modoControl==0){
      
      modoControl=1;//Si estabamos en modo control a 80% y pulsamos C pasamos al modo Fuzzy
    }
    else{ //Si estabamso en Fuzzy y pulsamos C pasamos a modo control al 80%
      modoControl=0;
    }
    if (modoControl == 0){
      Serial.println("Control iluminacion al 80%");
      lux = 0.0;
      lux_anterior=0.0;

    }
    else{
      Serial.println("Control iluminacion con Fuzzy Logic");
      lux = 0.0;
      lux_anterior=0.0;
    }
    break;
    default:
      //lcd.clear();
      Serial.println(receiver.decodedIRData.command);
      Serial.println(" other button");
  }
}
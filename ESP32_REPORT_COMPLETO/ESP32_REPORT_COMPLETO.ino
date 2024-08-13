
#include <HTTPClient.h>
#include <EEPROM.h>
#include <WiFi.h>

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

int AUXCOIN = 0;
int AUX2COIN = 0;
int CREDITO = 0;
int TIEMPO = 0;
unsigned int CUENTA = 0;
int TIEMPO2 = 0;
#define ECOIN 2 // lee los pulsos del billetero
#define SALIDA 13 // se usa para dar coin a la maquina
#define TEST 34 // GPIO 34
#define TEST1 35 // GPIO 35
#define TEST2 36 // GPIO 36
#define ENTHOPER 4 // retorno del hopper 
#define SALHOPER 5 // activa el hopper si se usa 
#define BOTON 3 // boton de cambio de dinero ingresado por fichas
#define SAL1 11 // se usa para inhibir el billetero
#define BARRERA 7 // entrada de barrera o tickets de la maquina
int PRECIO = 0; // no se esta usando
int TIEMPO3 = 0;
int TIEMPO4 = 0;
int TIEMPO5 = 0;
int INICIO = 0;
unsigned int ENT = 0;
unsigned int SAL = 0;
unsigned int EFIJO = 0;
unsigned int SFIJO = 0;
int AUX = 0;
int AUX1 = 0;
int BORRARCONTADORES = 0;
int FICHAS = 0;
int VALOR1 = 0;
int VALOR2 = 0;
int VALOR3 = 0;
unsigned int FICHAS1 = 0;
unsigned int FICHAS2 = 0;
unsigned int FICHAS3 = 0;
int HOPPER = 0;
int AUX2 = 0;
int AUX3 = 0;
int TIEMPO1 = 0;
int TIEMPO6 = 0;
int AUX4 = 0;
unsigned int BILLETE100 = 0;
unsigned int BILLETE200 = 0;
unsigned int BILLETE500 = 0;
unsigned int BILLETE1000 = 0;
unsigned int BILLETE2000 = 0;
int BILLETE1 = 0;
int BILLETE2 = 0;
int BILLETE3 = 0;
int BILLETE4 = 0;
int BILLETE5 = 0;
int TIEMPO7 = 0;
char DAT;
int BARRERAAUX = 0; // variable para lectura de barrera
int CUENTAAUX = 0;
int PREMIO = 0;
int TIEMPO8 = 0;
int BARRERAX = 0;
char MENSAJE;



void connectToWiFi() {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
    Serial.print("Dirección IP del ESP32: ");
    Serial.println(WiFi.localIP());
}

void sendDataToPHP() {
    Serial.println("Enviando datos al servidor PHP:");
    Serial.print("Dato1: ");
    Serial.println(dato1);
    Serial.print("Dato2: ");
    Serial.println(dato2);
    Serial.print("Dato3: ");
    Serial.println(dato3);

    HTTPClient http;
    http.begin(serverAddress);

    // Crear cuerpo de la solicitud JSON
    String postData = "{\"dato1\":" + String(dato1) + ",\"dato2\":" + String(dato2) + ",\"dato3\":" + String(dato3) + "}";

    Serial.print("Post Data: ");
    Serial.println(postData);

    // Enviar la solicitud POST
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
        Serial.print("Datos enviados a PHP correctamente. Código de respuesta HTTP: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println("Respuesta del servidor:");
        Serial.println(payload);
    } else {
        Serial.print("Error al enviar datos a PHP. Código de error HTTP: ");
        Serial.println(httpResponseCode);
        Serial.print("Error message: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();

    // Incrementar los valores después de enviarlos
    dato1++;
    dato2++;
    dato3++;
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();

          //este bloque se CONFIGURACION datos fijos
  HOPPER = 0; //en 1 activa salida hopper en 0 activa la salida para maquina
  digitalWrite(SAL1,HIGH);//asignamos el estdo para la salida de inhibicion
  digitalWrite(SALIDA,LOW);// asignamos estado a la salida de coin

  BILLETE100 = 1;// 1 pulto del billetero (billete 1)  cantidad de pulsos del billetero o monedero
  BILLETE200 = 3;// 2 pulsos del billetero(billete 2)
  BILLETE500 = 7;// 3 pulsos del billetero(billete 3)
  BILLETE1000 = 16; //4 pulsos del billetero(billete4)
  BILLETE2000 = 20; //5 pulsos del billetero(billete5)

  BILLETE1 = 100;  //valor de cada billete o moneda
  BILLETE2 = 200;
  BILLETE3 = 500;
  BILLETE4 = 1000;
  BILLETE5 = 2000;

  TIEMPO7 = 0;
  

 lcd.clear();
 lcd.print("iniciando 3");
}

void leerbarrera (){
    if (digitalRead (BARRERA) == HIGH){BARRERAAUX = HIGH;}
     delay (30);
     if (BARRERAAUX == HIGH){ lcd.setCursor(0,1);lcd.print("###");delay(1000);
     if (digitalRead (BARRERA) == HIGH){ lcd.setCursor(0,1);lcd.print("Barrera error");
     delay (5000);       
     }
     }
    }

void programar (){

    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("VERSION sep.2023");
    delay (500);
    while (digitalRead(TEST) == HIGH) {
        delay (20);
    }


    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("P$ F");
    lcd.setCursor(7,0);
    lcd.print((EFIJO * 100));
    lcd.setCursor(0,1);
    lcd.print("Coin F");
    lcd.setCursor(10,1);
    lcd.print(SFIJO); 
    delay (500);
    while (digitalRead(TEST) == HIGH) { 
      delay (20);
    }  
    
    EEPROM.get(13,EFIJO);  
    delay (500);

    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("P$");
    lcd.setCursor(6,0);
    lcd.print((ENT * 100));
    lcd.setCursor(0,1);
    lcd.print("Coin");
    lcd.setCursor(9,1);
    lcd.print(SAL); 
    delay (500);

    while (digitalRead(TEST) == HIGH) { 
      delay (20);
    }  
    EEPROM.get(5,ENT);   
    delay (500);

    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("Promedio ficha");
    lcd.setCursor(0,1);
    lcd.print((ENT * 100)/SAL);
    delay (500);

    while (digitalRead(TEST) == HIGH) { 
      delay (20);
    } 
    EEPROM.get(5,ENT); 
    delay (500);  

    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("Premios");
    lcd.setCursor(0,1);
    lcd.print(PREMIO);
    delay (500);
    while (digitalRead(TEST) == HIGH) { 
      delay (20);
      }  
    delay (500); 


  lcd.clear ();
  lcd.setCursor(0,0);
  lcd.print("BORRA CONTADORES");  
  lcd.setCursor(0,1);lcd.print("NO");
  delay (500);

  while (digitalRead(TEST) == HIGH){ 
   if (digitalRead(TEST1) == LOW){
    BORRARCONTADORES = HIGH;
    lcd.setCursor(0,1);
    lcd.print("SI"); 
    delay (500);
    }
   if (digitalRead(TEST2) == LOW) {
    BORRARCONTADORES = LOW;
    lcd.setCursor(0,1);
    lcd.print("NO");  
    delay (500);
    } }
   if (BORRARCONTADORES == HIGH){  
    EEPROM.put(5,1);
    EEPROM.put(9,1); 
    BORRARCONTADORES = LOW ;
    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("BORRADOS");
    delay (1000);  
    EEPROM.get(5,ENT);  
    EEPROM.get(9,SAL); 
   } 
   delay (500);

  while (digitalRead(TEST) == HIGH){ 
   delay (100);
   lcd.clear ();
   lcd.setCursor(0,0);
   lcd.print("Promo1 P$");
   lcd.setCursor(0,1);
   lcd.print(VALOR1);
   if (digitalRead(TEST1) == LOW)
   {VALOR1 = (VALOR1 + BILLETE1);
   lcd.setCursor(0,1);
   lcd.print(VALOR1); 
   delay (200);
   }
   if (digitalRead(TEST2) == LOW){
    VALOR1 = (VALOR1 - BILLETE1);
    lcd.setCursor(0,1);
    lcd.print(VALOR1);  
    delay (200);
    }
   } 
  EEPROM.put (25,VALOR1); 
  EEPROM.get(25,VALOR1); 
  delay (500);


  while (digitalRead(TEST) == HIGH){ 
    delay (100);
    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("Promo1 ");
    lcd.setCursor(8,1);
    lcd.print(FICHAS1);
    lcd.setCursor(0,1);
    lcd.print("Fichas");
    if (digitalRead(TEST1) == LOW){
      FICHAS1 = (FICHAS1 + 1);
      lcd.setCursor(8,1);
      lcd.print(FICHAS1); 
      delay (200);
      }
    if (digitalRead(TEST2) == LOW){
      FICHAS1 = (FICHAS1 - 1);
      lcd.setCursor(8,1);
      lcd.print(FICHAS1);  
      delay (200);
      }
   } 
   EEPROM.put(37,FICHAS1);
   EEPROM.get (37,FICHAS1);
   delay (500);

  while (digitalRead(TEST) == HIGH){ 
    delay (100);
    lcd.clear ();lcd.setCursor(0,0);lcd.print("Promo2 P$");lcd.setCursor(0,1);lcd.print(VALOR2);
    if (digitalRead(TEST1) == LOW){
      VALOR2 = (VALOR2 + BILLETE3);
      lcd.setCursor(0,1);
      lcd.print(VALOR2); 
      delay (200);
      }
    if (digitalRead(TEST2) == LOW){
      VALOR2 = (VALOR2 - BILLETE3);
      lcd.setCursor(0,1);
      lcd.print(VALOR2);  
      delay (200);
      }
   }

   EEPROM.put (29,VALOR2);
   EEPROM.get (29,VALOR2); 
   delay (500);


  while (digitalRead(TEST) == HIGH){ 
    delay (100);
    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("Promo2 ");
    lcd.setCursor(8,1);
    lcd.print(FICHAS2);
    lcd.setCursor(0,1);
    lcd.print("Fichas");  
    if (digitalRead(TEST1) == LOW){
      FICHAS2 = (FICHAS2 + 1);
      lcd.setCursor(0,1);
      lcd.print(FICHAS2);
      delay (200);
      }
    if (digitalRead(TEST2) == LOW){
      FICHAS2 = (FICHAS2 - 1);
      lcd.setCursor(0,1);
      lcd.print(FICHAS2);  
      delay (200);
      }
   } 
   EEPROM.put (41,FICHAS2); 
   EEPROM.get(41,FICHAS2);    
   delay (500);


  while (digitalRead(TEST) == HIGH){ 
    delay (100);
    lcd.clear ();
    lcd.setCursor(0,0);
    lcd.print("Promo3 P$");
    lcd.setCursor(0,1);
    lcd.print(VALOR3);
    if (digitalRead(TEST1) == LOW){
      VALOR3 = (VALOR3 + BILLETE4);
      lcd.setCursor(0,1);
      lcd.print(VALOR3); 
      delay (200);
      }
    if (digitalRead(TEST2) == LOW){
      VALOR3 = (VALOR3 - BILLETE4);
      lcd.setCursor(0,1);
      lcd.print(VALOR3);  
      delay (200);
      }
   } 
   EEPROM.put (33,VALOR3);  
   EEPROM.get(33,VALOR3); 
   delay (500);
  

while (digitalRead(TEST) == HIGH){ 
  delay (100);
  lcd.clear ();
  lcd.setCursor(0,0);
  lcd.print("Promo3 ");
  lcd.setCursor(8,1);
  lcd.print(FICHAS3);
  lcd.setCursor(0,1);
  lcd.print("Fichas");
  if (digitalRead(TEST1) == LOW){
   FICHAS3 = (FICHAS3 + 1);
   lcd.setCursor(0,1);lcd.print(FICHAS3); delay (200);
   }
  if (digitalRead(TEST2) == LOW){
    FICHAS3 = (FICHAS3 - 1);
    lcd.setCursor(0,1);
    lcd.print(FICHAS3);  
    delay (200);
    }
   } 
  EEPROM.put (45,FICHAS3); EEPROM.get(45,FICHAS3); delay (500);
  
  grabareprom ();
  graficar1 ();
  delay (1000);
}


 void graficar1 () {

 lcd.clear ();lcd.setCursor(0,0);lcd.print ("P$");
 lcd.setCursor (4,0);lcd.print (CUENTAAUX);
 lcd.setCursor(0,1);lcd.print("Creditos");
 lcd.setCursor(10,1);lcd.print (FICHAS);
}

 void grabareprom () {

   EEPROM.put(1,PRECIO);
  EEPROM.put(5,ENT);
  EEPROM.put(9,SAL);
  EEPROM.put(13,EFIJO);
  EEPROM.put(17,SFIJO);
  EEPROM.put(21,PREMIO); 
 }

void loop() {
  // Bloque de lectura de la barrera de la máquina
  leerBarrera();

  // Bloque de lectura de los pulsos del billetero o monedero
  leerPulsosBilleteroMonedero();

  // Manejo de créditos y conversiones
  manejoCreditosConversiones();

  // Dispensador de fichas si HOPPER es 1
  dispensadorFichas();
}

    sendDataToPHP(); // Envía los datos al servidor PHP
    delay(60000);    // Espera 1 minuto (60 segundos)

}

// Función para leer los pulsos del billetero o monedero
void leerPulsosBilleteroMonedero() {
  if (digitalRead(TEST) == LOW) {
    programar();
  }
  if (digitalRead(ECOIN) == LOW && AUXCOIN < 5) {
    AUXCOIN++;
  }
  if (AUXCOIN == 5 && AUX2COIN == LOW) {
    CREDITO++;
    AUX2COIN = HIGH;
    TIEMPO = 0;
  }
  if (digitalRead(ECOIN) == HIGH && AUXCOIN > 0) {
    AUXCOIN--;
  }
  if (AUXCOIN == 0 && AUX2COIN == HIGH) {
    AUX2COIN = LOW;
  }
  if (TIEMPO <= 510) {
    TIEMPO++;
  }
}

// Función para manejar créditos y conversiones
void manejoCreditosConversiones() {
  // Código aquí...
 if (CREDITO == BILLETE100 && TIEMPO == 500 ){
  CUENTA = (CUENTA + BILLETE1);
  ENT = (ENT + 1);EFIJO = (EFIJO + 1);
  AUX1 = 1;CUENTAAUX = (CUENTA);graficar1 ();
  }

 if (CREDITO == BILLETE200 && TIEMPO == 500 )
 {CUENTA = (CUENTA + BILLETE2);ENT = (ENT + 2);
 EFIJO = (EFIJO + 2);AUX1 = 1;CUENTAAUX = (CUENTA);
 graficar1 ();
 }

 if (CREDITO == BILLETE500 && TIEMPO == 500 )
 {CUENTA = (CUENTA + BILLETE3);ENT = (ENT + 5);
 EFIJO = (EFIJO + 5);AUX1 = 1;CUENTAAUX = (CUENTA);
 graficar1 ();
 }

 if (CREDITO == BILLETE1000 && TIEMPO == 500 ){CUENTA = (CUENTA + BILLETE4);
 ENT = (ENT + 10);EFIJO = (EFIJO + 10);
 AUX1 = 1;
 CUENTAAUX = (CUENTA);graficar1 ();
 }

 if (CREDITO == BILLETE2000 && TIEMPO == 500 )
 {CUENTA = (CUENTA + BILLETE5);
 ENT = (ENT + 20);
 EFIJO = (EFIJO + 20);AUX1 = 1;
 CUENTAAUX = (CUENTA);
 graficar1 ();
 }

 if (TIEMPO == 510){
 TIEMPO2 = 0; 
 CREDITO = 0;TIEMPO5 = 1; 
 digitalWrite(SALHOPER,LOW);
     TIEMPO3 = 0;TIEMPO4 = 2;
     }

   if (AUX1 == 1){
    grabareprom () ;AUX1 = 0;
   }

  if (digitalRead (BOTON) == LOW){
    TIEMPO5 = 0;TIEMPO4 = 0;
    } //luego de ingresar dinero tocar boton para hacer el cambio

    //BLOQUE QUE DA TIEMPO PARA TOTALIZAR EL DINERO
   if (TIEMPO3 < 1000 && TIEMPO5 == 1) { 
    TIEMPO3 ++; 
    }

   if (TIEMPO3 == 1000 && TIEMPO4 > 0){ 
    TIEMPO3 = 0;TIEMPO4 --; graficar1();
     }  
   if (TIEMPO4 == 0){
    TIEMPO5 = 0;
    }

                                      //bloque hace la convercion de plata a fichas
  if (TIEMPO5 == 0) {                                   
    if (CUENTA >= VALOR1 && CUENTA < VALOR2){
      FICHAS = (FICHAS +FICHAS1);
      CUENTA  = (CUENTA - VALOR1); 
      CUENTAAUX = (CUENTA);
      SAL = (SAL + FICHAS1);
      SFIJO = (SFIJO + FICHAS1);
      grabareprom ();
      }

    if (CUENTA >= VALOR2 && CUENTA < VALOR3){
      FICHAS = (FICHAS +FICHAS2);
      CUENTA = (CUENTA - VALOR2); 
      CUENTAAUX = (CUENTA);
      SAL = (SAL + FICHAS2);
      SFIJO = (SFIJO + FICHAS2);
      grabareprom ();
      }
    if (CUENTA >= VALOR3){
      FICHAS = (FICHAS +FICHAS3);
      CUENTA = (CUENTA - VALOR3); 
      CUENTAAUX = (CUENTA);
      SAL = (SAL + FICHAS3);
      SFIJO = (SFIJO + FICHAS3);
      grabareprom ();
      }
  }
}

// Función para el dispensador de fichas
void dispensadorFichas() {
  if (TIEMPO5 == 0) {                                   
    if (CUENTA >= VALOR1 && CUENTA < VALOR2){
      FICHAS = (FICHAS +FICHAS1);
      CUENTA  = (CUENTA - VALOR1); 
      CUENTAAUX = (CUENTA);
      SAL = (SAL + FICHAS1);
      SFIJO = (SFIJO + FICHAS1);
      grabareprom ();
      }

    if (CUENTA >= VALOR2 && CUENTA < VALOR3){
      FICHAS = (FICHAS +FICHAS2);
      CUENTA = (CUENTA - VALOR2); 
      CUENTAAUX = (CUENTA);
      SAL = (SAL + FICHAS2);
      SFIJO = (SFIJO + FICHAS2);
      grabareprom ();
      }

    if (CUENTA >= VALOR3){
      FICHAS = (FICHAS +FICHAS3);
      CUENTA = (CUENTA - VALOR3); 
      CUENTAAUX = (CUENTA);
      SAL = (SAL + FICHAS3);
      SFIJO = (SFIJO + FICHAS3);
      grabareprom ();
      }
  }

if (FICHAS > 0 && TIEMPO4 == 0 && HOPPER == 0 ){  //este bloque se encarga de los pulsos de salida para una maquina 
 if ( TIEMPO2 == 2){
  digitalWrite(SALIDA,HIGH);
  }         // se activa con HOPPER = 0 en set up
 TIEMPO2 ++;
 if (TIEMPO2 == 200){
  digitalWrite(SALIDA,LOW);FICHAS --; 
  } 
 if (TIEMPO2 == 400){
  TIEMPO2 = 0; 
  }
 if (FICHAS == 0){
  TIEMPO4 = 10; graficar1 (); 
   } 
  }//tiempo4 no es nesesario usarlo cuando el conversor esta en una maquina, esta desactivado
    //de la experiencia mejor es usar el boton de convercion con el hopper

        // este bloque es el dispenser de fichas, se activa con HOPPER = 1
   if (FICHAS > 0 && TIEMPO5 == 0 && HOPPER == 1 && AUX4 == 0 ){
    digitalWrite (SALHOPER,HIGH);
    if (digitalRead(ENTHOPER) == LOW && AUX2 < 2){
      AUX2 ++;
      }
    if (AUX2 == 2 && AUX3 == LOW){
      FICHAS -- ;;
      AUX3 = HIGH;TIEMPO1 = 0;
      TIEMPO6 = 0;
      }
    if (digitalRead(ENTHOPER) == HIGH && AUX2 > 0){
      AUX2 --;
      }   
    if (AUX2 == 0 && AUX3 == HIGH){
      AUX3 = LOW;
      }
    if (FICHAS == 0){ 
      digitalWrite(SALHOPER,LOW); 
      graficar1 ();
      }
    if (TIEMPO1 <= 500 ){
      TIEMPO1 ++;
      } 
       //este bloque da un tiempo para que salgan las fichas
    if (TIEMPO6 < 5000 && AUX4 == 0) { 
      TIEMPO6 ++; 
      if (TIEMPO6 == 5000){
        lcd.clear ();
        lcd.setCursor(0,0);
        lcd.print("Sin servico");
        lcd.setCursor(0,1);lcd.print("Faltan fichas");digitalWrite (SAL1,LOW);
        digitalWrite (SALHOPER,LOW); 
        AUX4 = 1;
      } 
    }  
  }
}


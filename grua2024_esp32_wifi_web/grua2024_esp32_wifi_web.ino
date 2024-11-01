#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>  // Agregar la biblioteca Ticker

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

const char* device_id = "ESP32_001";
const char* ssid = "HUAWEI-2.4G-Dqc5";
const char* password = "46332714";
const char* serverAddress = "https://maquinasbonus.com/esp32_project/insert_data.php";
const char* serverAddress1 = "https://maquinasbonus.com/esp32_project/insert_heartbeat.php";

#define triger 13
#define echo 12
#define DATO11 19
#define DATO7 14
#define DATO3 4
#define DATO5 25
#define EPINZA 17
#define SPINZA 16
#define DATO6 34
#define DATO10 35
#define DATO12 27
#define ECOIN 26

int X = 0;
 int RDISTANCIA = 0;//referencia de distancia
 int TIEMPO = 0;
 int TIEMPOAUX = 0;//variable auxiliar sistema de tiempo de la pinza
 int B = 0;
 int A = 0;
 int PAGO = 20;
 int BARRERA = 0;
 int CLEAR = 0;
 unsigned int COIN ;
 int BANK = 0;
 unsigned int BANKTIEMPO = 0;
 unsigned int CONTSALIDA ;
  unsigned int Y ;//no se usa actualmente, era para dividir entrada /salida
  int Z = 0;
  int distancia = 0;
  int tiempo = 0;
unsigned int CTIEMPO = 0;
int AUX = 0;// variable usada para evitar ruido en cierre de pinza
int CREDITO = 0;//variable usada para los creditos
int AUXCOIN = 0;
int AUX2COIN = 0;
int FUERZA = 50;
int INICIO = 0;
int BORRARCONTADORES = LOW;
unsigned int PJFIJO = 0;
unsigned int PPFIJO = 0;
int BARRERAAUX = 0;
float FUERZAAUX = 0;
float FUERZAV = 0;
int TIEMPOAUX1 = 0;
int FUERZAAUX2 = 0;
int GRUADISPLAY = 0;
int BARRERAAUX2 = 0;
char DAT;
int TIEMPO7 = 0;
long TIEMPO8 = 0;
//char MENSAJE = "" ;
int TIEMPO9 = 0;
int AUXSIM = 0;
int TIEMPO5 = 0;
int prevPJFIJO = 0;
int prevPPFIJO = 0;
int prevBANK = 0;
int AUXDATO3 = 0;

unsigned long lastUpdate = 0; // Para controlar los incrementos de variables
unsigned long lastSendTime = 0; // Para controlar el envío de datos
const long updateInterval = 1000; // 1 segundo para actualizaciones
const long sendInterval = 30000; // 30 segundos para envío de datos

// Definir el ticker para ejecutar la función de envío del pulso cada 60 segundos
Ticker tickerPulso;


void enviarPulso() {
    // Comprobar si el WiFi está conectado
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverAddress1);  // Conectar al servidor

        // Crear el mensaje JSON con el device_id
        JsonDocument doc;
        doc["device_id"] = device_id;

        // Serializar el documento JSON
        String postData;
        serializeJson(doc, postData);

        // Añadir cabecera HTTP
        http.addHeader("Content-Type", "application/json");

        // Realizar la solicitud POST
        int httpResponseCode = http.POST(postData);

        // Comprobar la respuesta del servidor
        if (httpResponseCode > 0) {
            Serial.println("Heartbeat enviado correctamente.");
        } else {
            Serial.println("Error enviando el heartbeat: " + String(httpResponseCode));
        }

        // Finalizar la conexión HTTP
        http.end();
    } else {
        Serial.println("Error: WiFi desconectado.");
    }
}

// Conectar a WiFi con reinicio seguro
void connectToWiFi() {
    WiFi.disconnect(true); // Reinicia el WiFi completamente
    delay(1000);

    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 20) { // 20 intentos de conexión
        delay(1000);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi conectado");
        Serial.print("Dirección IP del ESP32: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Error: No se pudo conectar a WiFi.");
    }
}

// Enviar datos a PHP cuando haya cambios en las variables
void sendDataToPHP(const char* device_id, unsigned int dato1, unsigned int dato2, unsigned int dato3, int dato4) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverAddress);

        // Crear documento JSON
        JsonDocument doc;
        doc["device_id"] = device_id;
        doc["dato1"] = dato1;
        doc["dato2"] = dato2;
        doc["dato3"] = dato3;
        doc["dato4"] = dato4;

        // Serializar JSON y enviarlo
        String postData;
        serializeJson(doc, postData);

        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Datos enviados: " + response);
        } else {
            Serial.println("Error enviando datos: " + String(httpResponseCode));
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado. No se pueden enviar datos.");
    }
}

void setup() {

    Serial.begin(115200);
    EEPROM.begin(256);

    analogWriteFrequency(SPINZA, 100);

    // Inicializar LCD
    lcd.init();
    lcd.backlight();

    // Configuración de pines
    pinMode(triger, OUTPUT);
    pinMode(echo, INPUT);
    pinMode(DATO7, INPUT_PULLUP);
    pinMode(DATO5, OUTPUT);
    pinMode(DATO3, INPUT_PULLUP);
    pinMode(SPINZA, OUTPUT);
    pinMode(EPINZA, INPUT_PULLUP);
    pinMode(DATO11, OUTPUT);
    pinMode(DATO6, INPUT_PULLUP);
    pinMode(DATO10, INPUT_PULLUP);
    pinMode(DATO12, INPUT_PULLUP);
    pinMode(ECOIN, INPUT_PULLUP);
    
    // Leer variables de la EEPROM
    EEPROM.get(45, INICIO);
    if (INICIO != 35) {
        EEPROM.put(1, 0);
        EEPROM.put(5, 0);
        EEPROM.put(9, 0);
        EEPROM.put(13, 12);
        EEPROM.put(17, 2000);
        EEPROM.put(21, 50);
        EEPROM.put(25, 0);
        EEPROM.put(29, 0);
        EEPROM.put(33, 0);
        EEPROM.put(37, 0);
        EEPROM.put(41, 0);
        EEPROM.put(45, 35);
        EEPROM.commit();
    }

  EEPROM.get(1,COIN);
  EEPROM.get(5,CONTSALIDA);
  EEPROM.get(9,BANK);
  EEPROM.get(13,PAGO);
  EEPROM.get(17,TIEMPO);
  EEPROM.get(21,FUERZA);
  EEPROM.get(25,PJFIJO);
  EEPROM.get(29,PPFIJO);
  EEPROM.get(33,BARRERAAUX2);
  EEPROM.get(37,GRUADISPLAY);
  EEPROM.get(41,TIEMPO5);
  
   if (digitalRead(DATO3) == LOW){ lcd.clear ();lcd.setCursor(0,0);lcd.print("CONT.FIJOS"); lcd.setCursor(0,1);lcd.print("PJ:"); 
     lcd.setCursor(3,1);lcd.print(PJFIJO); lcd.setCursor(9,1);lcd.print("PP:");
     lcd.setCursor(12,1);lcd.print(PPFIJO);delay (1000);
      while (digitalRead(DATO3) == HIGH) { delay (20);} }delay (1000);
   
  Y = 2;
  
    while(B <=10){    //hace una prueba con la barrera y determina la distancia para operar
   digitalWrite(triger,LOW);
  delayMicroseconds(2);
  digitalWrite(triger,HIGH);
  delayMicroseconds(10);
  tiempo = pulseIn(echo,HIGH);
  distancia = tiempo/10;
  RDISTANCIA = distancia ;
  B ++;
  delay(50);}
  
  digitalWrite (DATO11,1);// inicializa el dato 11 mantiene a 0 el reset del sim 800
  connectToWiFi(); // Conectar a WiFi al inicio
  tickerPulso.attach(60, enviarPulso);  // Ejecuta cada 60 segundos
}

void graficar() {
    if (GRUADISPLAY == 0) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("PJ:"); lcd.setCursor(3, 0); lcd.print(COIN);
        lcd.setCursor(9, 0); lcd.print("PP:"); lcd.setCursor(12, 0); lcd.print(CONTSALIDA);
        lcd.setCursor(0, 1); lcd.print("PA:"); lcd.setCursor(3, 1); lcd.print(PAGO);
        lcd.setCursor(7, 1); lcd.print(BANKTIEMPO); lcd.setCursor(9, 1); lcd.print("BK:");
        lcd.setCursor(12, 1); lcd.print(BANK);
    }
    if (GRUADISPLAY == 1) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Credito");
        lcd.setCursor(8, 0); lcd.print(CREDITO);
    }
}

void leecoin() {
    while (digitalRead(ECOIN) == LOW && AUXCOIN < 5) {
        AUXCOIN++;
        delay(1);
        if (digitalRead(ECOIN) == HIGH) AUXCOIN = 0;
    }
    if (AUXCOIN == 5 && AUX2COIN == LOW) {
        CREDITO++;
        AUX2COIN = HIGH;
        graficar();
    }
    while (digitalRead(ECOIN) == HIGH && AUXCOIN > 0) {
        AUXCOIN--;
        delay(2);
        if (digitalRead(ECOIN) == LOW) AUXCOIN = 5;
    }
    if (AUXCOIN == 0 && AUX2COIN == HIGH) AUX2COIN = LOW;
}
void leerbarrera() {
    if (BARRERAAUX2 == 1) {
        // Leer distancia con sensor ultrasónico
        digitalWrite(triger, LOW);
        delayMicroseconds(2);
        digitalWrite(triger, HIGH);
        delayMicroseconds(10);
        tiempo = pulseIn(echo, HIGH);
        distancia = tiempo / 10;

        // Comparar distancia medida con la referencia
        if ((distancia + 130) < RDISTANCIA || (distancia - 130) > RDISTANCIA) {
            BARRERA = HIGH;
        }

        // Si la barrera se activa
        if (BARRERA == HIGH) {
            lcd.setCursor(0, 1); lcd.print("###");
            delay(2500);
            graficar();

            if (digitalRead(DATO12) == HIGH) {
                CONTSALIDA++;
                PPFIJO++;
                BANK -= PAGO;
                BARRERA = LOW;
                BARRERAAUX = HIGH;
                graficar();
                EEPROM.put(5, CONTSALIDA);
                EEPROM.put(29, PPFIJO);
                EEPROM.put(9, BANK);
                EEPROM.commit();
            }
        }
    }

    if (BARRERAAUX2 == 0) {
        // Barrera infrarroja
        if (digitalRead(DATO7) == HIGH) BARRERA = HIGH;

        if (BARRERA == HIGH) {
            AUXSIM = HIGH;
            lcd.setCursor(0, 1); lcd.print("###");
            delay(1000);
            CONTSALIDA++;
            PPFIJO++;
            BANK -= PAGO;
            BARRERA = LOW;
            BARRERAAUX = HIGH;
            graficar();
            EEPROM.put(5, CONTSALIDA);
            EEPROM.put(29, PPFIJO);
            EEPROM.put(9, BANK);
            EEPROM.commit();
        }
    }
}

void ajustebarrera() {
    // Similar a leerbarrera pero usado para ajuste
    if (BARRERAAUX2 == 1) {
        digitalWrite(triger, LOW);
        delayMicroseconds(2);
        digitalWrite(triger, HIGH);
        delayMicroseconds(10);
        tiempo = pulseIn(echo, HIGH);
        distancia = tiempo / 10;

        if ((distancia + 130) < RDISTANCIA || (distancia - 130) > RDISTANCIA) {
            BARRERA = HIGH;
        }

        if (BARRERA == HIGH) {
            lcd.setCursor(0, 1); lcd.print("###");
            delay(1000);
        }
    }

    if (BARRERAAUX2 == 0) {
        if (digitalRead(DATO7) == HIGH) BARRERA = HIGH;

        if (BARRERA == HIGH) {
            AUXSIM = HIGH;
            lcd.setCursor(0, 1); lcd.print("###");
            delay(1000);
        }
    }
}

   void programar (){

     lcd.clear ();lcd.setCursor(0,0);lcd.print("VERSION 1.7");  lcd.setCursor(0,1);lcd.print("24/5/24");delay (500);
      while (digitalRead(DATO3) == HIGH) { delay (20);} 

    lcd.clear ();lcd.setCursor(0,0);lcd.print("PJ:");lcd.setCursor(4,0);lcd.print(PJFIJO);
    lcd.setCursor(0,1);lcd.print("PP:");lcd.setCursor(4,1);lcd.print(PPFIJO); delay (500);
    while (digitalRead(DATO3) == HIGH) { delay (20);}GRUADISPLAY = 0;

    graficar (); delay (500);
     while (digitalRead(DATO3) == HIGH) { delay (20);}EEPROM.get(37,GRUADISPLAY);
 
    lcd.clear ();lcd.setCursor(0,0);lcd.print("BORRA CONTADORES");  lcd.setCursor(0,1);lcd.print("NO");delay (500);
     while (digitalRead(DATO3) == HIGH){ 
   if (digitalRead(DATO6) == LOW){BORRARCONTADORES = HIGH;lcd.setCursor(0,1);lcd.print("SI"); delay (500);}
   if (digitalRead(DATO10) == LOW) {BORRARCONTADORES = LOW;lcd.setCursor(0,1);lcd.print("NO");  delay (500);} }
   if (BORRARCONTADORES == HIGH){ EEPROM.put(1,0); EEPROM.put(5,0);EEPROM.put(9,0);EEPROM.commit();
    EEPROM.get(1,COIN); EEPROM.get(5,CONTSALIDA);EEPROM.get(9,BANK);
   BORRARCONTADORES = LOW ;lcd.clear ();lcd.setCursor(0,0);lcd.print("BORRADOS");delay (1000);  } 
    

     lcd.clear ();lcd.setCursor(0,0);lcd.print("Display Modo");
     if(GRUADISPLAY == 0){lcd.setCursor(0,1);lcd.print("Modo Contadores");}
     if(GRUADISPLAY == 1){lcd.setCursor(0,1);lcd.print("Modo Coin");}delay (500);
    while (digitalRead(DATO3) == HIGH) {
   if (digitalRead(DATO6) == LOW){GRUADISPLAY = 0;lcd.setCursor(0,1);lcd.print("Modo Contadores"); }
   if (digitalRead(DATO10) == LOW ) {GRUADISPLAY = 1 ;lcd.setCursor(0,1);lcd.print("Modo Coin"); }
   }EEPROM.put(37,GRUADISPLAY);EEPROM.commit(); 
     
    lcd.clear ();lcd.setCursor(0,0);lcd.print("AJUSTAR PAGO"); delay (500);
   while (digitalRead(DATO3) == HIGH){  lcd.setCursor(0,1);lcd.print(PAGO);delay (100);
   if (digitalRead(DATO6) == LOW){PAGO ++;lcd.setCursor(0,1);lcd.print(PAGO); delay (400);}
   if (digitalRead(DATO10) == LOW) {PAGO --;lcd.setCursor(0,1);lcd.print(PAGO);  delay (400);}
   } EEPROM.put (13,PAGO);EEPROM.commit();
   
     lcd.clear ();lcd.setCursor(0,0);lcd.print("AJUSTAR TIEMPO"); delay (500);  
    while (digitalRead(DATO3) == HIGH) {  lcd.setCursor(0,1);lcd.print(TIEMPO);delay(100);
   if (digitalRead(DATO6) == LOW && TIEMPO < 5000){TIEMPO = (TIEMPO +10);lcd.setCursor(0,1);lcd.print(TIEMPO); }
   if (digitalRead(DATO10) == LOW && TIEMPO > 500) {TIEMPO = (TIEMPO -10);lcd.setCursor(0,1);lcd.print(TIEMPO); }
   } EEPROM.put (17,TIEMPO);EEPROM.commit(); 
    
     lcd.clear ();lcd.setCursor(0,0);lcd.print("TIEMPO F. FUERTE"); delay (500);  
    while (digitalRead(DATO3) == HIGH) {  lcd.setCursor(0,1);lcd.print(TIEMPO5);delay(100);
   if (digitalRead(DATO6) == LOW && TIEMPO5 < 5000){TIEMPO5 = (TIEMPO5 +10);lcd.setCursor(0,1);lcd.print(TIEMPO5); }
   if (digitalRead(DATO10) == LOW && TIEMPO5 > 0) {TIEMPO5 = (TIEMPO5 -10);lcd.setCursor(0,1);lcd.print(TIEMPO5); }
   } EEPROM.put (41,TIEMPO5); EEPROM.commit();

   
    lcd.clear ();lcd.setCursor(0,0);lcd.print("AJUSTAR FUERZA");delay (500);
    while (digitalRead(DATO3) == HIGH) { lcd.setCursor(0,1);lcd.print(FUERZA);delay(100);
   if (digitalRead(DATO6) == LOW && FUERZA < 101){FUERZA ++;lcd.setCursor(0,1);lcd.print(FUERZA); }
   if (digitalRead(DATO10) == LOW && FUERZA > 5) {FUERZA --;lcd.setCursor(0,1);lcd.print(FUERZA); }
   } EEPROM.put (21,FUERZA);EEPROM.commit();
   
     lcd.clear ();lcd.setCursor(0,0);lcd.print("TIPO BARRERA");
     if(BARRERAAUX2 == 0){lcd.setCursor(0,1);lcd.print("INFRARROJO");}
     if(BARRERAAUX2 == 1){lcd.setCursor(0,1);lcd.print("ULTRASONIDO");}delay (500);
    while (digitalRead(DATO3) == HIGH) {
   if (digitalRead(DATO6) == LOW ){BARRERAAUX2 = 0;lcd.setCursor(0,1);lcd.print("INFRARROJO"); }
   if (digitalRead(DATO10) == LOW ) {BARRERAAUX2 = 1 ;lcd.setCursor(0,1);lcd.print("ULTRASONIDO"); }
   }EEPROM.put(33,BARRERAAUX2); EEPROM.commit();
   
     lcd.clear ();lcd.setCursor(0,0);lcd.print("PRUEBA BARRERA"); delay (500);
     while (digitalRead(DATO3) == HIGH) { lcd.setCursor(0,1);lcd.print(distancia);
    lcd.setCursor (8,1);lcd.print (RDISTANCIA); ajustebarrera ();
    if (BARRERA == HIGH){lcd.setCursor (0,1);lcd.print("###");delay(1000);BARRERA = LOW; }}
     
     graficar ();delay (500);
     
   }

void loop() {

  unsigned int dato1, dato2, dato3;
  int dato4;

    graficar(); // Actualiza el LCD

    // Espera a que la señal de la pinza se ponga en HIGH
    while (digitalRead(EPINZA) == LOW && AUX < 5) {
        CTIEMPO++;
        if (digitalRead(EPINZA) == HIGH) {
            AUX++;
        }
        if (digitalRead(EPINZA) == LOW) {
            AUX = 0;
        }

        if (TIEMPO7 < 20000) {
            TIEMPO7++;
            delay(1);
        }

        // Leer monedas
        leecoin();

        // Verificar si las variables han cambiado para guardar los valores
        if (PJFIJO != prevPJFIJO || PPFIJO != prevPPFIJO || BANK != prevBANK) {
            dato1 = PJFIJO; dato2 = PJFIJO; dato3 = PPFIJO; dato4 = BANK;
            sendDataToPHP(device_id, dato1, dato2, dato3, dato4);
            prevPJFIJO = PJFIJO;
            prevPPFIJO = PPFIJO;
            prevBANK = BANK;
        }

        // Control de la rutina de programación
        if (digitalRead(DATO3) == LOW) {
            AUXDATO3++;
        } else {
            AUXDATO3 = 0;
        }

        if (digitalRead(DATO3) == LOW && AUXDATO3 == 30) {
            programar();
            AUXDATO3 = 0;
        }

        // Control del tiempo de juego
        if (CTIEMPO >= 18000 && BANKTIEMPO > 0) {
            CTIEMPO = 0;
            BANKTIEMPO--;
        }

        // Leer la barrera periódicamente
        if (TIEMPO8 < 200) {
            TIEMPO8++;
            if (TIEMPO8 == 199) {
                TIEMPO8 = 0;
                if (BARRERAAUX == LOW) {
                    leerbarrera();
                }
            }
        }
    }

    // Restablecer variables
    BARRERAAUX = LOW;
    AUX = 0;
    BARRERA = LOW;

    // Descontar créditos si existen
    if (CREDITO >= 1) {
        CREDITO--;
    }

    graficar(); // Actualizar LCD con nuevos valores

    // Incremento periódico de COIN, BANK, PJFIJO
    COIN++;
    BANK++;
    PJFIJO++;

    if (BANKTIEMPO < 10) {
        BANKTIEMPO++;
    }

    // Guardar los valores actualizados en la EEPROM
    EEPROM.put(9, BANK);
    EEPROM.put(25, PJFIJO);
    EEPROM.put(1, COIN);
    EEPROM.commit();

Z = random(5); // Valor aleatorio para el control de la pinza

// Si el crédito es suficiente y el valor aleatorio cumple la condición
if (PAGO <= BANK && Z <= 3) {
    // Cerrar la pinza usando PWM
    analogWrite(SPINZA, 250);  // Establecer el duty cycle (250 de 255 para cerrar la pinza)
    delay(2000);  // Esperar 2 segundos

    // Control de las aperturas temporales en la señal de la pinza
    while (digitalRead(EPINZA) == HIGH || X < 100) {
        if (digitalRead(EPINZA) == HIGH) {
            X = 0;
        }
        if (X == 15) {
            analogWrite(SPINZA, 0);  // Apagar la pinza (duty cycle = 0)
        }
        if (digitalRead(EPINZA) == LOW) {
            X++;  // Incrementa X para cerrar progresivamente
        }

        // Leer la barrera periódicamente (si existe)
        if (TIEMPO8 < 550) {
            TIEMPO8++;
            if (TIEMPO8 == 549) {
                TIEMPO8 = 0;
                if (BARRERAAUX == LOW) {
                    leerbarrera();  // Lógica para la barrera
                }
            }
        }

        // Si la barrera está activa, salir del loop
        if (BARRERA == HIGH) {
            break;
        }
    }
} else {
    // Bloque de cierre de la pinza cuando no hay pago
    analogWrite(SPINZA, 255);  // Cerrar la pinza completamente (duty cycle máximo)
    delay(TIEMPO5);  // Esperar según TIEMPO5

    // Ajustar la fuerza de la pinza dependiendo del banco
    FUERZAAUX2 = FUERZA;
    if (BANK <= -10) {
        FUERZAAUX2 = FUERZA * 0.8;  // Reducir la fuerza un 20% si el banco es negativo
    }

    // Ajuste del valor aleatorio de fuerza
    FUERZAV = random(FUERZAAUX2 * 1.8, FUERZAAUX2 * 2.5);
    FUERZAAUX = (FUERZAV - FUERZAAUX2) / 10;

    TIEMPOAUX = 0;
    TIEMPOAUX1 = (TIEMPO / 10);

    // Aflojar progresivamente la fuerza de la pinza
    while (TIEMPOAUX <= 10) {
        FUERZAV -= FUERZAAUX;
        analogWrite(SPINZA, FUERZAV);  // Ajustar el duty cycle de la pinza
        TIEMPOAUX++;
        delay(TIEMPOAUX1);  // Ajustar el delay progresivo
    }

    // Ajuste final de la pinza
    analogWrite(SPINZA, FUERZA * 1.3);  // Aumentar la fuerza temporalmente
    delay(300);
    analogWrite(SPINZA, FUERZA);  // Volver a la fuerza normal
    delay(100);
    analogWrite(SPINZA, FUERZA * 1.3);  // Aumentar la fuerza nuevamente

    // Control de las aperturas temporales en la señal de la pinza
    while (digitalRead(EPINZA) == HIGH || X < 100) {
        if (digitalRead(EPINZA) == HIGH) {
            X = 0;
        }
        if (X == 15) {
            analogWrite(SPINZA, 0);  // Apagar la pinza
        }
        if (digitalRead(EPINZA) == LOW) {
            X++;
        }

        // Leer la barrera periódicamente
        if (TIEMPO8 < 550) {
            TIEMPO8++;
            if (TIEMPO8 == 549) {
                TIEMPO8 = 0;
                if (BARRERAAUX == LOW) {
                    leerbarrera();  // Lógica para la barrera
                }
            }
        }

        if (BARRERA == HIGH) {
            break;
        }
    }
}

// Apagar la pinza después de terminar
analogWrite(SPINZA, 0);  // Apagar la pinza (duty cycle 0)
TIEMPO8 = 0;
X = 0;
A = 0;

}

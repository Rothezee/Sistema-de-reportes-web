/*
2 Enero de 2025
Esta vercion reporta dinero ingresado/fichasexpendidas en la web, 
tiene una opcion para leer la barrera y reportar.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>  

typedef struct{
  unsigned int dato1, dato2, dato3, dato5;
  int dato4;
  const char* device_id;
}info;

Preferences preferences;
Ticker tickerPulso;
info i;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

int AUXCOIN = 0;
int AUX2COIN = 0;
int CREDITO = 0;
int TIEMPO = 0;
unsigned int CUENTA = 0;
int TIEMPO2 = 0;
#define ECOIN 35   // lee los pulsos del billetero
#define SALIDA 17  // se usa para dar coin a la maquina
#define AC 16 //apertura y cierre 
#define BOTON_ENTREGA 26 // Pin del botón de entrega de promoción
#define TEST 4
#define TEST1 34
#define TEST2 35
#define ENTHOPER 32  // retorno del hopper
#define SALHOPER 12  // activa el hopper si se usa
#define BOTON 36     // boton de cambio de dinero ingresado por fichas
#define SAL1 00      // se usa para inhibir el billetero
#define BARRERA 34   //entrada de barrera o tickets de la maquina
int PRECIO = 0;      //no se esta usando
int TIEMPO1 = 0, TIEMPO3 = 0, TIEMPO4 = 0, TIEMPO5 = 0, TIEMPO6 = 0, TIEMPO7 = 0;
int INICIO = 0;
unsigned int ENT = 0, SAL = 0, EFIJO = 0, SFIJO = 0, prevEFIJO = 0, prevSFIJO = 0;
int AUX = 0;
int AUX1 = 0;
int BORRARCONTADORES = 0;
int FICHAS = 0;
unsigned int VALOR1 = 0, VALOR2 = 0, VALOR3 = 0;
unsigned int FICHAS1 = 0, FICHAS2 = 0, FICHAS3 = 0;
int HOPPER = 0;
int AUX2 = 0;
int AUX3 = 0;
int AUX4 = 0;
char DAT;
int BARRERAAUX = 0;  //variable para lectura de barrera
int CUENTAAUX = 0;
int PREMIO = 0;
int TIEMPO8 = 0;
int BARRERAX = 0;
unsigned int Rcuenta = 0, Rsal = 0;

const char* ssid = "HUAWEI-2.4G-Dqc5";
const char* password = "46332714";
const char* serverAddress1 = "https://www.maquinasbonus.com/esp32_project/insert_heartbeat.php";
const char* serverAddress2 = "https://www.maquinasbonus.com/esp32_project/insert_close.php";

void enviarPulso() {
  // Comprobar si el WiFi está conectado
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverAddress1);  // Conectar al servidor

    // Crear el mensaje JSON con el device_id
    JsonDocument doc;
    doc["device_id"] = i.device_id;

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
  WiFi.disconnect(true);  // Reinicia el WiFi completamente
  delay(1000);

  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {  // 20 intentos de conexión
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
void sendDataToPHP(info i, const char* serverAddress) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverAddress);

    // Crear documento JSON
    JsonDocument doc;
    doc["device_id"] = i.device_id;
    doc["dato1"] = i.dato1;
    doc["dato2"] = i.dato2;
    doc["dato3"] = i.dato3;
    doc["dato4"] = i.dato4;

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
  i.device_id = "Ticket_1";
  Serial.begin(115200);
  preferences.begin("storage", false);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.print("iniciando");
  delay(1500);

  pinMode(AC, INPUT_PULLUP);  //INTERRUPTOR PARA LA APERTURA Y CIERRE DEL DIA
  pinMode(ECOIN, INPUT);    //entrada pulsos del billetero
  pinMode(SALIDA, INPUT);  //salida pulsos para maquina
  pinMode(TEST1, INPUT);    //botones de programacion
  pinMode(TEST2, INPUT);
  pinMode(TEST, INPUT);
  pinMode(ENTHOPER, INPUT);   //retorno sensor hopper
  pinMode(SALHOPER, OUTPUT);  //habilitacion hopper
  pinMode(BOTON, INPUT);      //boton cambio manual
  pinMode(SAL1, OUTPUT);      //bloqueo del billetero
  pinMode(BARRERA, INPUT);    // lee la barrera o el pago o los tickets
  pinMode(BOTON_ENTREGA, INPUT);
  lcd.clear();
  delay(1500);

  int INICIO = preferences.getInt("INICIO", 0); // Valor por defecto = 0

  // Inicializar por primera vez si INICIO != 40
  if (INICIO != 40) {
    preferences.putInt("INICIO", 40);  // Guarda el valor 40 como bandera

    // Guardar los valores iniciales
    preferences.putInt("PRECIO", 200);
    preferences.putInt("ENT", 1);
    preferences.putInt("SAL", 1);
    preferences.putInt("EFIJO", 0);
    preferences.putInt("SFIJO", 0);
    preferences.putInt("PREMIO", 0);
    preferences.putInt("VALOR1", 1000);
    preferences.putInt("VALOR2", 5000);
    preferences.putInt("VALOR3", 10000);
    preferences.putInt("FICHAS1", 1);
    preferences.putInt("FICHAS2", 2);
    preferences.putInt("FICHAS3", 5);

    Serial.println("Valores inicializados.");
  }

  // Cargar variables desde la memoria
  PRECIO = preferences.getInt("PRECIO");
  ENT = preferences.getInt("ENT");
  SAL = preferences.getInt("SAL");
  EFIJO = preferences.getInt("EFIJO");
  SFIJO = preferences.getInt("SFIJO");
  PREMIO = preferences.getInt("PREMIO");
  VALOR1 = preferences.getInt("VALOR1");
  VALOR2 = preferences.getInt("VALOR2");
  VALOR3 = preferences.getInt("VALOR3");
  FICHAS1 = preferences.getInt("FICHAS1");
  FICHAS2 = preferences.getInt("FICHAS2");
  FICHAS3 = preferences.getInt("FICHAS3");

  HOPPER = 0;                 //en 1 activa salida hopper en 0 activa la salida para maquina
  digitalWrite(SAL1, HIGH);   //asignamos el estdo para la salida de inhibicion
  digitalWrite(SALIDA, LOW);  // asignamos estado a la salida de coin

  TIEMPO7 = 0;

  lcd.clear();
  lcd.print("iniciando 3");
  connectToWiFi();
  tickerPulso.attach(60,  enviarPulso);  // Ejecuta cada 60 segundos
}

void leerbarrera() {

  if (digitalRead(BARRERA) == HIGH) { BARRERAAUX = HIGH; }
  delay(30);
  if (BARRERAAUX == HIGH) {
    lcd.setCursor(0, 1);
    lcd.print("###");
    delay(1000);
    if (digitalRead(BARRERA) == HIGH) {
      lcd.setCursor(0, 1);
      lcd.print("Barrera error");
      delay(5000);
    }
  }
}


void configurarPromo(const char* nombrePromo, unsigned int &valorPromo, int paso, int min, int max, const char* key, unsigned int &fichasPromo) {
    while (digitalRead(TEST) == HIGH) { 
        delay(100); // Mantener tiempo de respuesta
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(nombrePromo);
        lcd.setCursor(0, 1);
        lcd.print(valorPromo);

        if (digitalRead(TEST1) == LOW) {
            valorPromo += paso;
            if (valorPromo > max) valorPromo = max;
            lcd.setCursor(0, 1);
            lcd.print(valorPromo);
            delay(200);
        }
        if (digitalRead(TEST2) == LOW) {
            valorPromo -= paso;
            if (valorPromo < min) valorPromo = min;
            lcd.setCursor(0, 1);
            lcd.print(valorPromo);
            delay(200);
        }
        // Si se presiona el botón de entrega, actualizar valores
        if (digitalRead(BOTON_ENTREGA) == LOW) {
            CUENTA -= valorPromo;  // Restar el precio de la promoción
            Rcuenta += valorPromo; // Registrar en el cierre diario
            FICHAS += fichasPromo; // Agregar fichas entregadas
            Rsal += fichasPromo;   // Registrar fichas en el cierre diario
            SFIJO += fichasPromo;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Entrega Exitosa!");
            delay(1000);
        }
    }
    while (digitalRead(TEST) == LOW) { delay(50); }  
    preferences.putInt(key, valorPromo);
}

void programar() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VERSION ene,2025");
    delay(500);

    while (digitalRead(TEST) == HIGH) { delay(20); }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("P$ F");
    lcd.setCursor(7, 0);
    lcd.print((EFIJO * 100));
    lcd.setCursor(0, 1);
    lcd.print("Coin F");
    lcd.setCursor(10, 1);
    lcd.print(SFIJO);
    delay(500);

    while (digitalRead(TEST) == HIGH) { delay(20); }

    EFIJO = preferences.getInt("EFIJO", 13);
    ENT = preferences.getInt("ENT", 5);
    SAL = preferences.getInt("SAL", 9);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Premios");
    lcd.setCursor(0, 1);
    lcd.print(PREMIO);
    delay(500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BORRA CONTADORES");
    lcd.setCursor(0, 1);
    lcd.print("NO");
    delay(500);

    while (digitalRead(TEST) == HIGH) {
        if (digitalRead(TEST1) == LOW) {
            BORRARCONTADORES = HIGH;
            lcd.setCursor(0, 1);
            lcd.print("SI");
            delay(500);
        }
        if (digitalRead(TEST2) == LOW) {
            BORRARCONTADORES = LOW;
            lcd.setCursor(0, 1);
            lcd.print("NO");
            delay(500);
        }
    }

    if (BORRARCONTADORES == HIGH) {
        preferences.putInt("ENT", 1);
        preferences.putInt("SAL", 1);
        BORRARCONTADORES = LOW;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BORRADOS");
        delay(1000);
        ENT = preferences.getInt("ENT", 5);
        SAL = preferences.getInt("SAL", 9);
    }

    delay(500);

    // Configuración de promociones con el botón de entrega
    configurarPromo("Promo1 P$", VALOR1, 100, 0, 10000, "VALOR1", FICHAS1);
    delay(300);

    configurarPromo("Promo1 Fichas", FICHAS1, 1, 1, 100, "FICHAS1", FICHAS1);
    delay(300);

    configurarPromo("Promo2 P$", VALOR2, 500, 0, 10000, "VALOR2", FICHAS2);
    delay(300);

    configurarPromo("Promo2 Fichas", FICHAS2, 1, 1, 100, "FICHAS2", FICHAS2);
    delay(300);
    
    configurarPromo("Promo3 P$", VALOR3, 1000, 0, 10000, "VALOR3", FICHAS3);
    delay(300);

    configurarPromo("Promo3 Fichas", FICHAS3, 1, 1, 100, "FICHAS3", FICHAS3);
    delay(300);

    grabareprom();
    graficar1();
    delay(1000);
}



void graficar1() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P$");
  lcd.setCursor(4, 0);
  lcd.print(CUENTAAUX);
  lcd.setCursor(0, 1);
  lcd.print("Creditos");
  lcd.setCursor(10, 1);
  lcd.print(FICHAS);
}

void grabareprom() {
  preferences.putInt("PRECIO", 1);
  preferences.putInt("ENT", 5);
  preferences.putInt("SAL", 9);
  preferences.putInt("EFIJO", 13);
  preferences.putInt("SFIJO", 17);
  preferences.putInt("PREMIO", 21);
}

void procesarBillete(int valorBillete, int entrada, int fijo) {
    CUENTA += valorBillete;
    Rcuenta += valorBillete;
    Rsal += fijo;
    ENT += entrada;
    EFIJO += fijo;
    AUX1 = 1;
    CUENTAAUX = CUENTA;
    graficar1();
}

void evaluarCredito(int credito) {
  switch (credito) {
    case 1:
      procesarBillete(100, 1, 1);
      break;
    case 2:
        procesarBillete(200, 2, 2);
        break;
    case 5:
        procesarBillete(500, 5, 5);
        break;
    case 10:
        procesarBillete(1000, 10, 10);
        break;
    case 20:
        procesarBillete(2000, 20, 20);
        break;
  }
}

void convertirFichas(int valorMin, int valorMax, int fichas) {
  if (CUENTA >= valorMin && CUENTA < valorMax) {
    FICHAS += fichas;
    CUENTA -= valorMin;
    CUENTAAUX = CUENTA;
    SAL += fichas;
    Rsal += fichas;
    SFIJO += fichas;
    grabareprom();
  }
}

void enviarCierreDiario() {
  i.dato2 = Rcuenta;
  i.dato5 = Rsal;
  sendDataToPHP(i, "https://www.maquinasbonus.com/esp32_project/insert_data.php");//FALTA CAMBIAR LA DIRECCION A LA QUE SON ENVIADOS ACA

  Rcuenta = 0;
  Rsal = 0;
}

void loop() {

int estadoAC = digitalRead(AC);  // Leer estado actual del interruptor
static int estadoAC_anterior = HIGH;  // Variable para guardar el estado anterior

// Si el estado cambia de HIGH a LOW, se ejecuta el cierre
if (estadoAC == LOW && estadoAC_anterior == HIGH) {
    enviarCierreDiario();  // Llamar a la función solo una vez
}

// Actualizar el estado anterior para la próxima iteración
estadoAC_anterior = estadoAC;

  if(EFIJO != prevEFIJO || SFIJO != prevSFIJO){
    i.dato2 = EFIJO;
    i.dato5 = SFIJO;
    sendDataToPHP(i, "https://www.maquinasbonus.com/esp32_project/insert_data.php");
    prevEFIJO = EFIJO;
    prevSFIJO = SFIJO;
  }

  //este bloque lee la barrera de la maquina
  BARRERAAUX = digitalRead(BARRERA);
  if (BARRERAAUX == 1 && BARRERAX == 0) {
    BARRERAX = 1;
    PREMIO++;
    TIEMPO8 = 0;
    preferences.putInt("PREMIO", 21);
    lcd.clear();
    delay(1000);
    graficar1();
  }
  if (TIEMPO8 <= 5002) { TIEMPO8++; }
  if (BARRERAAUX == 1 && TIEMPO8 > 5000) { BARRERAX = 0; }

  //este bloque lee los pulsos del billetero o monedero
  if ((digitalRead(TEST)) == LOW) { programar(); }
  if (digitalRead(ECOIN) == LOW && AUXCOIN < 5) { AUXCOIN++; }
  //if (digitalRead(ECOIN) == HIGH){AUXCOIN = 0;}
  if (AUXCOIN == 5 && AUX2COIN == LOW) {
    CREDITO++;
    AUX2COIN = HIGH;
    TIEMPO = 0;
  }
  if (digitalRead(ECOIN) == HIGH && AUXCOIN > 0) { AUXCOIN--; }
  //if (digitalRead(ECOIN) == LOW ){AUXCOIN = 5;}
  if (AUXCOIN == 0 && AUX2COIN == HIGH) { AUX2COIN = LOW; }
  delay(1);
  if (TIEMPO <= 510) { TIEMPO++; }

  if(TIEMPO == 500)
    evaluarCredito(CREDITO);

  if (TIEMPO == 510) {
    TIEMPO2 = 0;
    CREDITO = 0;
    TIEMPO5 = 1;
    digitalWrite(SALHOPER, LOW);
    TIEMPO3 = 0;
    TIEMPO4 = 2;
    Serial.print(CUENTAAUX);
  }
  if (AUX1 == 1) {
    grabareprom();
    AUX1 = 0;
  }

  if (digitalRead(BOTON) == LOW) {
    TIEMPO5 = 0;
    TIEMPO4 = 0;
  }  //luego de ingresar dinero tocar boton para hacer el cambio

  //bloque da tiempo para totalizar dinero
  if (TIEMPO3 < 1000 && TIEMPO5 == 1) { TIEMPO3++; }
  if (TIEMPO3 == 1000 && TIEMPO4 > 0) {
    TIEMPO3 = 0;
    TIEMPO4--;
    graficar1();
  }
  if (TIEMPO4 == 0) { TIEMPO5 = 0; }

  //bloque hace la convercion de plata a fichas
  if (TIEMPO5 == 0) {
    convertirFichas(VALOR1, VALOR2, FICHAS1);
    convertirFichas(VALOR2, VALOR3, FICHAS2);
    if (CUENTA >= VALOR3) {
        convertirFichas(VALOR3, INT_MAX, FICHAS3); // Se procesa lo restante
    }
  }

  if (FICHAS > 0 && TIEMPO4 == 0 && HOPPER == 0) {     //este bloque se encarga de los pulsos de salida para una maquina
    if (TIEMPO2 == 0) { digitalWrite(SALIDA, HIGH); }  // se activa con HOPPER = 0 en set up
    TIEMPO2++;
    if (TIEMPO2 == 200) {
      digitalWrite(SALIDA, LOW);
      FICHAS--;
    }
    if (TIEMPO2 == 400) { TIEMPO2 = 0; }
    if (FICHAS == 0) {
      TIEMPO4 = 10;
      graficar1();
    }
  }  //tiempo4 no es nesesario usarlo cuando el conversor esta en una maquina, esta desactivado
     //de la experiencia mejor es usar el boton de convercion con el hopper

  // este bloque es el dispenser de fichas, se activa con HOPPER = 1
  if (FICHAS > 0 && TIEMPO5 == 0 && HOPPER == 1 && AUX4 == 0) {
    digitalWrite(SALHOPER, HIGH);
    if (digitalRead(ENTHOPER) == LOW && AUX2 < 2) { AUX2++; }
    if (AUX2 == 2 && AUX3 == LOW) {
      FICHAS--;
      ;
      AUX3 = HIGH;
      TIEMPO1 = 0;
      TIEMPO6 = 0;
    }
    if (digitalRead(ENTHOPER) == HIGH && AUX2 > 0) { AUX2--; }
    if (AUX2 == 0 && AUX3 == HIGH) { AUX3 = LOW; }
    if (FICHAS == 0) {
      digitalWrite(SALHOPER, LOW);
      graficar1();
    }
    if (TIEMPO1 <= 500) { TIEMPO1++; }
    //este bloque da un tiempo para que salgan las fichas
    if (TIEMPO6 < 5000 && AUX4 == 0) {
      TIEMPO6++;
      if (TIEMPO6 == 5000) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sin servico");
        lcd.setCursor(0, 1);
        lcd.print("Faltan fichas");
        digitalWrite(SAL1, LOW);
        digitalWrite(SALHOPER, LOW);
        AUX4 = 1;
      }
    }
  }
}

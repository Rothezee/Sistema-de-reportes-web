#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(12, 10);

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

int AUXCOIN = 0;
int AUX2COIN = 0;
int CREDITO = 0;
int TIEMPO = 0;
unsigned int COIN = 0; // Variable to store coin count

#define ECOIN 2    // Lee los pulsos del billetero
#define SALIDA 13  // Se usa para dar coin a la maquina
#define TEST A0    // Boton de programacion
#define BOTON 3    // Boton de cambio de dinero ingresado por fichas

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  EEPROM.begin();

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.print("Iniciando");
  delay(1500);

  pinMode(ECOIN, INPUT_PULLUP);  // Entrada pulsos del billetero
  pinMode(SALIDA, OUTPUT);       // Salida pulsos para maquina
  pinMode(TEST, INPUT_PULLUP);   // Boton de programacion
  pinMode(BOTON, INPUT_PULLUP);  // Boton cambio manual

  lcd.clear();
  lcd.print("EEPROM Init");
  delay(1500);

  EEPROM.get(0, COIN); // Obtener el valor almacenado de COIN

  lcd.clear();
  lcd.print("Iniciado");
}

void mensaje() {
  String jsonMessage = "{\"device_id\":\"Maquina_4\",\"coin\":" + String(COIN) + "}\n";
  mySerial.print(jsonMessage);
}

void grabarEEPROM() {
  EEPROM.put(0, COIN);
}

void graficar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Coins:");
  lcd.setCursor(6, 0);
  lcd.print(COIN);
}

void loop() {
  delay(1);  // Espera mínima para evitar sobrecargar el procesador

  // Si hay datos disponibles en el puerto serial
  if (mySerial.available()) {
    String data = mySerial.readStringUntil('\n');
    Serial.println("Received: " + data);
  }

  // Bloque de lectura de los pulsos del billetero
  if (digitalRead(ECOIN) == LOW && AUXCOIN < 5) {
    AUXCOIN++;
  }
  if (AUXCOIN == 5 && AUX2COIN == LOW) {
    COIN++;
    AUX2COIN = HIGH;
    TIEMPO = 0;
    mensaje(); // Enviar datos al ESP32 cada vez que se recibe un nuevo coin
    grabarEEPROM(); // Guardar el valor de COIN en EEPROM
    graficar(); // Actualizar el display
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

  // Modo de programación
  if (digitalRead(TEST) == LOW) {
    lcd.clear();
    lcd.print("Programa Coins");
    delay(500);
    while (digitalRead(TEST) == LOW) {
      delay(20);
    }
    COIN = 0; // Reiniciar el contador de coins
    grabarEEPROM();
    graficar();
  }

  // Conversión manual
  if (digitalRead(BOTON) == LOW) {
    // Aquí puedes añadir cualquier lógica adicional si necesitas realizar una conversión manual
  }
}

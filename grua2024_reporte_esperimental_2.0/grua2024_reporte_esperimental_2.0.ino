#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Prototipos de funciones
void setupPins();
void setupLCD(LiquidCrystal_I2C &lcd);
void setupEEPROM(int &COIN, int &CONTSALIDA, int &BANK, int &PAGO, int &TIEMPO, int &FUERZA, int &PJFIJO, int &PPFIJO, int &BARRERAAUX2, int &GRUADISPLAY, int &TIEMPO5);
void loop(LiquidCrystal_I2C &lcd, SoftwareSerial &Myserial, int &COIN, int &BANK, int &PJFIJO, int &PPFIJO, int &pesos, int &BARRERA, int &CREDITO);
void graficar(LiquidCrystal_I2C &lcd, int COIN, int CONTSALIDA, int PAGO, int BANK, int BANKTIEMPO, int GRUADISPLAY, int CREDITO);
void leerbarrera(int &BARRERA, int RDISTANCIA, int triger, int echo, int DATO12, int &CONTSALIDA, int PAGO, int &BANK, int &PPFIJO);
void programar(LiquidCrystal_I2C &lcd, int &PJFIJO, int &PPFIJO, int &GRUADISPLAY, int &PAGO, int &TIEMPO, int &TIEMPO5, int &FUERZA, int &BARRERAAUX2, int &COIN, int &CONTSALIDA, int &BANK);
void leecoin(int ECOIN, int &CREDITO);
void mensaje(SoftwareSerial &Myserial, int PJFIJO, int PPFIJO, int BANK);
void serialCheck(SoftwareSerial &Myserial);
void controlPinza(int SPINZA, int EPINZA, int FUERZA, int TIEMPO5, int &BANK, int &BARRERA, int &X, int &TIEMPO8, int &BARRERAAUX);
void mostrarVersion(LiquidCrystal_I2C &lcd);
void mostrarFijos(LiquidCrystal_I2C &lcd, int PJFIJO, int PPFIJO);
bool borrarContadores(LiquidCrystal_I2C &lcd, int DATO3, int DATO6, int DATO10, int &COIN, int &CONTSALIDA, int &BANK);
void ajustarValor(LiquidCrystal_I2C &lcd, const char* mensaje, int &valor, int min, int max, int step, int DATO3, int DATO6, int DATO10);
void seleccionarModo(LiquidCrystal_I2C &lcd, int &GRUADISPLAY, int DATO3, int DATO6, int DATO10);
void tipoBarrera(LiquidCrystal_I2C &lcd, int &BARRERAAUX2, int DATO3, int DATO6, int DATO10);
void pruebaBarrera(LiquidCrystal_I2C &lcd, int RDISTANCIA, int &BARRERA, int DATO3, int triger, int echo);

void setup() {
    LiquidCrystal_I2C lcd(0x27, 16, 2);
    SoftwareSerial Myserial(10, 11); // RX = 10, TX = 11

    int COIN = 0;
    int CONTSALIDA = 0;
    int BANK = 0;
    int PAGO = 20;
    int TIEMPO = 0;
    int FUERZA = 50;
    int PJFIJO = 0;
    int PPFIJO = 0;
    int BARRERAAUX2 = 0;
    int GRUADISPLAY = 0;
    int TIEMPO5 = 0;
    int pesos = 0;
    int BARRERA = 0;
    int CREDITO = 0;

    setupPins();
    setupLCD(lcd);
    setupEEPROM(COIN, CONTSALIDA, BANK, PAGO, TIEMPO, FUERZA, PJFIJO, PPFIJO, BARRERAAUX2, GRUADISPLAY, TIEMPO5);

    while (true) {
        loop(lcd, Myserial, COIN, BANK, PJFIJO, PPFIJO, pesos, BARRERA, CREDITO);
    }
}

void setupPins() {
    pinMode(9, OUTPUT);       // triger
    pinMode(8, INPUT);        // echo
    pinMode(7, INPUT_PULLUP); // DATO7 (botón)
    pinMode(3, OUTPUT);       // DATO5
    pinMode(A0, INPUT_PULLUP);// DATO3
    pinMode(5, OUTPUT);       // SPINZA
    pinMode(4, INPUT_PULLUP); // EPINZA (botón)
    pinMode(12, OUTPUT);      // DATO11
    pinMode(A2, INPUT_PULLUP);// DATO6
    pinMode(A1, INPUT_PULLUP);// DATO10
    pinMode(6, INPUT_PULLUP); // DATO12
    pinMode(2, INPUT_PULLUP); // ECOIN
}

void setupLCD(LiquidCrystal_I2C &lcd) {
    lcd.init();
    lcd.backlight();
}

void setupEEPROM(int &COIN, int &CONTSALIDA, int &BANK, int &PAGO, int &TIEMPO, int &FUERZA, int &PJFIJO, int &PPFIJO, int &BARRERAAUX2, int &GRUADISPLAY, int &TIEMPO5) {
    int INICIO;
    EEPROM.get(17, INICIO);
    if (INICIO != 35) {
        EEPROM.put(1, 0); 
        EEPROM.put(3, 0);
        EEPROM.put(5, 0);
        EEPROM.put(7, 12);
        EEPROM.put(9, 2000);
        EEPROM.put(11, 50);
        EEPROM.put(13, 0);
        EEPROM.put(17, 35);
        EEPROM.put(19, 0);
        EEPROM.put(21, 0);
        EEPROM.put(15, 0);
        EEPROM.put(23, 1000);
    }

    EEPROM.get(1, COIN);
    EEPROM.get(3, CONTSALIDA);
    EEPROM.get(5, BANK);
    EEPROM.get(7, PAGO);
    EEPROM.get(9, TIEMPO);
    EEPROM.get(11, FUERZA);
    EEPROM.get(13, PJFIJO);
    EEPROM.get(15, PPFIJO);
    EEPROM.get(19, BARRERAAUX2);
    EEPROM.get(21, GRUADISPLAY);
    EEPROM.get(23, TIEMPO5);
}

void loop(LiquidCrystal_I2C &lcd, SoftwareSerial &Myserial, int &COIN, int &BANK, int &PJFIJO, int &PPFIJO, int &pesos, int &BARRERA, int &CREDITO) {
    PJFIJO++;
    PPFIJO++;
    BANK++;


    static int RDISTANCIA = 0;
    static int CONTSALIDA = 0;
    static int BANKTIEMPO = 0;
    static int TIEMPO8 = 0;
    static int AUXSIM = 0;
    static int X = 0;
    int GRUADISPLAY = 0;
    int PAGO = 20;
    int TIEMPO = 0;
    int TIEMPO5 = 0;
    int FUERZA = 50;
    int BARRERAAUX2 = 0;

    Serial.print("dato1: " + String(PJFIJO * 100));
    graficar(lcd, COIN, CONTSALIDA, 20, BANK, BANKTIEMPO, 0, CREDITO);

    controlPinza(5, 4, FUERZA, TIEMPO5, BANK, BARRERA, X, TIEMPO8, AUXSIM);

    if (digitalRead(7) == LOW) { // DATO7 (botón)
        leerbarrera(BARRERA, RDISTANCIA, 9, 8, 12, CONTSALIDA, PAGO, BANK, PPFIJO);
    }

    leecoin(2, CREDITO);

    if (Myserial.available()) {
        mensaje(Myserial, PJFIJO, PPFIJO, BANK);
    }

    if (digitalRead(A0) == LOW) { // DATO3 (botón)
        programar(lcd, PJFIJO, PPFIJO, GRUADISPLAY, PAGO, TIEMPO, TIEMPO5, FUERZA, BARRERAAUX2, COIN, CONTSALIDA, BANK);
    }

    if (BANKTIEMPO > 0) {
        BANKTIEMPO--;
    }

    // Guardar valores importantes en EEPROM
    EEPROM.put(5, BANK);
    EEPROM.put(13, PJFIJO);
    EEPROM.put(1, COIN);

    delay(1000); // Evita saturar el loop
}

void graficar(LiquidCrystal_I2C &lcd, int COIN, int CONTSALIDA, int PAGO, int BANK, int BANKTIEMPO, int GRUADISPLAY, int CREDITO) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PJ: ");
    lcd.print(COIN);
    lcd.setCursor(9, 0);
    lcd.print("PP:");
    lcd.setCursor(12, 0);
    lcd.print(CONTSALIDA);
    lcd.setCursor(0, 1);
    lcd.print("PA:");
    lcd.setCursor(3, 1);
    lcd.print(PAGO);
    lcd.setCursor(7, 1);
    lcd.print(BANKTIEMPO);
    lcd.setCursor(9, 1);
    lcd.print("BK:");
    lcd.setCursor(12, 1);
    lcd.print(BANK);
}

void leerbarrera(int &BARRERA, int RDISTANCIA, int triger, int echo, int DATO12, int &CONTSALIDA, int PAGO, int &BANK, int &PPFIJO) {
    digitalWrite(triger, LOW);
    delayMicroseconds(2);
    digitalWrite(triger, HIGH);
    delayMicroseconds(10);
    int tiempo = pulseIn(echo, HIGH);
    int distancia = tiempo / 10;
    if ((distancia + 30) < RDISTANCIA) {
        BARRERA = HIGH;
    }
    if ((distancia - 30) > RDISTANCIA) {
        BARRERA = HIGH;
    }
    if (BARRERA == HIGH) {
        delay(2500);
        if (digitalRead(DATO12) == HIGH) {
            CONTSALIDA++;
            PPFIJO++;
            BANK -= PAGO;
            BARRERA = LOW;
            EEPROM.put(3, CONTSALIDA);
            EEPROM.put(15, PPFIJO);
            EEPROM.put(5, BANK);
        }
    }
}

void programar(LiquidCrystal_I2C &lcd, int &PJFIJO, int &PPFIJO, int &GRUADISPLAY, int &PAGO, int &TIEMPO, int &TIEMPO5, int &FUERZA, int &BARRERAAUX2, int &COIN, int &CONTSALIDA, int &BANK) {
    mostrarVersion(lcd);
    mostrarFijos(lcd, PJFIJO, PPFIJO);

    if (borrarContadores(lcd, A0, A2, A1, COIN, CONTSALIDA, BANK)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BORRADOS");
        delay(1000);
    }

    seleccionarModo(lcd, GRUADISPLAY, A0, A2, A1);
    ajustarValor(lcd, "AJUSTAR PAGO", PAGO, 1, 100, 1, A0, A2, A1);
    ajustarValor(lcd, "AJUSTAR TIEMPO", TIEMPO, 500, 5000, 10, A0, A2, A1);
    ajustarValor(lcd, "TIEMPO F. FUERTE", TIEMPO5, 0, 5000, 10, A0, A2, A1);
    ajustarValor(lcd, "AJUSTAR FUERZA", FUERZA, 5, 101, 1, A0, A2, A1);
    tipoBarrera(lcd, BARRERAAUX2, A0, A2, A1);
    
    // Variables locales para pasar por referencia
    int distancia = 0;
    int barrera = 0;
    
    pruebaBarrera(lcd, distancia, barrera, A0, 9, 8);
    
    graficar(lcd, COIN, CONTSALIDA, PAGO, BANK, 0, GRUADISPLAY, 0);
}

void leecoin(int ECOIN, int &CREDITO) {
    int AUXCOIN = 0;
    int AUX2COIN = 0;

    while (digitalRead(ECOIN) == LOW && AUXCOIN < 5) {
        AUXCOIN++;
        delay(1);
        if (digitalRead(ECOIN) == HIGH) {
            AUXCOIN = 0;
        }
    }
    if (AUXCOIN == 5 && AUX2COIN == LOW) {
        CREDITO++;
        AUX2COIN = HIGH;
    }
    while (digitalRead(ECOIN) == HIGH && AUXCOIN > 0) {
        AUXCOIN--;
        delay(2);
        if (digitalRead(ECOIN) == LOW) {
            AUXCOIN = 5;
        }
    }
    if (AUXCOIN == 0 && AUX2COIN == HIGH) {
        AUX2COIN = LOW;
    }
}

void mensaje(SoftwareSerial &Myserial, int PJFIJO, int PPFIJO, int BANK) {
    String jsonMessage = "{\"device_id\":\"Maquina_1\",\"dato1\":" + String(PJFIJO * 100) +
                         ",\"dato2\":" + String(PJFIJO) +
                         ",\"dato3\":" + String(PPFIJO) +
                         ",\"dato4\":" + String(BANK) + "}\n";
    Myserial.print(jsonMessage);
    Serial.print(jsonMessage);
}

void serialCheck(SoftwareSerial &Myserial) {
    while (Myserial.available()) {
        Serial.write(Myserial.read());
    }
    delay(200);
}

void controlPinza(int SPINZA, int EPINZA, int FUERZA, int TIEMPO5, int &BANK, int &BARRERA, int &X, int &TIEMPO8, int &BARRERAAUX) {
    int Z = random(5);
    int PAGO = 20; // Declarar PAGO aquí

    if (PAGO <= BANK && Z <= 3) {
        analogWrite(SPINZA, 250);
        delay(2000);
        while ((digitalRead(EPINZA) == HIGH) || (X < 100)) {
            if (digitalRead(EPINZA) == HIGH) {
                X = 0;
            }
            if (X == 15) {
                analogWrite(SPINZA, 0);
            }
            if (digitalRead(EPINZA) == LOW) {
                X++;
            }

            if (TIEMPO8 < 550) {
                TIEMPO8++;
                if (TIEMPO8 == 549) {
                    TIEMPO8 = 0;
                    if (BARRERAAUX == LOW) {
                        int RDISTANCIA = 0;  // Declarar RDISTANCIA aquí
                        int CONTSALIDA = 0;  // Declarar CONTSALIDA aquí
                        int PPFIJO = 0;      // Declarar PPFIJO aquí
                        leerbarrera(BARRERA, RDISTANCIA, 9, 8, 12, CONTSALIDA, PAGO, BANK, PPFIJO);
                    }
                }
            }

            if (BARRERA == HIGH) {
                break;
            }
        }
    } else {
        analogWrite(SPINZA, 255);
        delay(TIEMPO5);
        float FUERZAAUX2 = FUERZA;
        if (BANK <= -10) {
            FUERZAAUX2 = (FUERZA * 0.8);
        }
        float FUERZAV = random(FUERZAAUX2 * 1.8, FUERZAAUX2 * 2.5);
        float FUERZAAUX = (FUERZAV - FUERZAAUX2) / 10;
        int TIEMPOAUX = 0;
        int TIEMPOAUX1 = TIEMPO5 / 10;
        while (TIEMPOAUX <= 10) {
            FUERZAV = (FUERZAV - FUERZAAUX);
            analogWrite(SPINZA, FUERZAV);
            TIEMPOAUX++;
            delay(TIEMPOAUX1);
        }

        analogWrite(SPINZA, FUERZA * 1.3);
        delay(300);
        analogWrite(SPINZA, FUERZA);
        delay(100);
        analogWrite(SPINZA, FUERZA * 1.3);

        while ((digitalRead(EPINZA) == HIGH) || (X < 100)) {
            if (digitalRead(EPINZA) == HIGH) {
                X = 0;
            }
            if (X == 15) {
                analogWrite(SPINZA, 0);
            }
            if (digitalRead(EPINZA) == LOW) {
                X++;
            }

            if (TIEMPO8 < 550) {
                TIEMPO8++;
                if (TIEMPO8 == 549) {
                    TIEMPO8 = 0;
                    if (BARRERAAUX == LOW) {
                        int RDISTANCIA = 0;  // Declarar RDISTANCIA aquí
                        int CONTSALIDA = 0;  // Declarar CONTSALIDA aquí
                        int PPFIJO = 0;      // Declarar PPFIJO aquí
                        leerbarrera(BARRERA, RDISTANCIA, 9, 8, 12, CONTSALIDA, PAGO, BANK, PPFIJO);
                    }
                }
            }

            if (BARRERA == HIGH) {
                break;
            }
        }
    }

    analogWrite(SPINZA, 0);
}

void mostrarVersion(LiquidCrystal_I2C &lcd) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VERSION 1.7");
    lcd.setCursor(0, 1);
    lcd.print("24/5/24");
    delay(500);
    while (digitalRead(A0) == HIGH) {
        delay(20);
    }
}

void mostrarFijos(LiquidCrystal_I2C &lcd, int PJFIJO, int PPFIJO) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PJ:");
    lcd.setCursor(4, 0);
    lcd.print(PJFIJO);
    lcd.setCursor(0, 1);
    lcd.print("PP:");
    lcd.setCursor(4, 1);
    lcd.print(PPFIJO);
    delay(500);
    while (digitalRead(A0) == HIGH) {
        delay(20);
    }
}

bool borrarContadores(LiquidCrystal_I2C &lcd, int DATO3, int DATO6, int DATO10, int &COIN, int &CONTSALIDA, int &BANK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BORRA CONTADORES");
    lcd.setCursor(0, 1);
    lcd.print("NO");
    delay(500);
    bool borrar = false;
    while (digitalRead(DATO3) == HIGH) {
        if (digitalRead(DATO6) == LOW) {
            borrar = true;
            lcd.setCursor(0, 1);
            lcd.print("SI");
            delay(500);
        }
        if (digitalRead(DATO10) == LOW) {
            borrar = false;
            lcd.setCursor(0, 1);
            lcd.print("NO");
            delay(500);
        }
    }
    if (borrar) {
        EEPROM.put(1, 0);
        EEPROM.put(3, 0);
        EEPROM.put(5, 0);
        EEPROM.get(1, COIN);
        EEPROM.get(3, CONTSALIDA);
        EEPROM.get(5, BANK);
    }
    return borrar;
}

void ajustarValor(LiquidCrystal_I2C &lcd, const char* mensaje, int &valor, int min, int max, int step, int DATO3, int DATO6, int DATO10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(mensaje);
    delay(500);
    while (digitalRead(DATO3) == HIGH) {
        lcd.setCursor(0, 1);
        lcd.print(valor);
        delay(100);
        if (digitalRead(DATO6) == LOW && valor < max) {
            valor += step;
            lcd.setCursor(0, 1);
            lcd.print(valor);
            delay(400);
        }
        if (digitalRead(DATO10) == LOW && valor > min) {
            valor -= step;
            lcd.setCursor(0, 1);
            lcd.print(valor);
            delay(400);
        }
    }
    EEPROM.put(7, valor); // Guardar el valor ajustado en EEPROM
}

void seleccionarModo(LiquidCrystal_I2C &lcd, int &GRUADISPLAY, int DATO3, int DATO6, int DATO10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Display Modo");
    lcd.setCursor(0, 1);
    if (GRUADISPLAY == 0) {
        lcd.print("Modo Contadores");
    } else {
        lcd.print("Modo Coin");
    }
    delay(500);
    while (digitalRead(DATO3) == HIGH) {
        if (digitalRead(DATO6) == LOW) {
            GRUADISPLAY = 0;
            lcd.setCursor(0, 1);
            lcd.print("Modo Contadores");
        }
        if (digitalRead(DATO10) == LOW) {
            GRUADISPLAY = 1;
            lcd.setCursor(0, 1);
            lcd.print("Modo Coin");
        }
    }
    EEPROM.put(21, GRUADISPLAY);
}

void tipoBarrera(LiquidCrystal_I2C &lcd, int &BARRERAAUX2, int DATO3, int DATO6, int DATO10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TIPO BARRERA");
    if (BARRERAAUX2 == 0) {
        lcd.setCursor(0, 1);
        lcd.print("INFRARROJO");
    } else {
        lcd.setCursor(0, 1);
        lcd.print("ULTRASONIDO");
    }
    delay(500);
    while (digitalRead(DATO3) == HIGH) {
        if (digitalRead(DATO6) == LOW) {
            BARRERAAUX2 = 0;
            lcd.setCursor(0, 1);
            lcd.print("INFRARROJO");
        }
        if (digitalRead(DATO10) == LOW) {
            BARRERAAUX2 = 1;
            lcd.setCursor(0, 1);
            lcd.print("ULTRASONIDO");
        }
    }
    EEPROM.put(19, BARRERAAUX2);
}

void pruebaBarrera(LiquidCrystal_I2C &lcd, int RDISTANCIA, int &BARRERA, int DATO3, int triger, int echo) {
    int CONTSALIDA = 0;  // Declarar CONTSALIDA aquí
    int PAGO = 20;       // Declarar PAGO aquí
    int BANK = 0;        // Declarar BANK aquí
    int PPFIJO = 0;      // Declarar PPFIJO aquí

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PRUEBA BARRERA");
    delay(500);
    while (digitalRead(DATO3) == HIGH) {
        lcd.setCursor(0, 1);
        lcd.print("Distancia:");
        lcd.setCursor(10, 1);
        lcd.print(RDISTANCIA);
        leerbarrera(BARRERA, RDISTANCIA, triger, echo, 0, CONTSALIDA, PAGO, BANK, PPFIJO);
        if (BARRERA == HIGH) {
            lcd.setCursor(0, 1);
            lcd.print("###");
            delay(1000);
            BARRERA = LOW;
        }
    }
}

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuración de la red WiFi y del servidor
const char* device_id = "ESP32_001"; // Identificador único del ESP32
const char* ssid = "HUAWEI-2.4G-Dqc5"; // Nombre de la red WiFi
const char* password = "46332714"; // Contraseña de la red WiFi
const char* serverAddress = "http://192.168.100.5/esp32_project/insert_data.php"; // Dirección del servidor PHP

// Crear una instancia de HardwareSerial
HardwareSerial mySerial(1); // UART1

void connectToWiFi() {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password); // Iniciar la conexión WiFi
    while (WiFi.status() != WL_CONNECTED) { // Esperar hasta que se conecte
        delay(1000); // Esperar 1 segundo
        Serial.print("."); // Imprimir un punto para indicar que sigue intentando conectar
    }
    Serial.println("\nWiFi conectado");
    Serial.print("Dirección IP del ESP32: ");
    Serial.println(WiFi.localIP()); // Imprimir la dirección IP del ESP32
}

void sendDataToPHP(const char* device_id, unsigned int dato1, unsigned int dato2, unsigned int dato3, int dato4) {
    HTTPClient http; // Crear una instancia de HTTPClient
    http.begin(serverAddress); // Iniciar la conexión HTTP
    
    dato1 = dato1 * 100; // Multiplicar dato1 para la conversión de juegos a pesos
    
    // Crear el documento JSON
    StaticJsonDocument<200> doc;
    doc["device_id"] = device_id;
    doc["dato1"] = dato1;
    doc["dato2"] = dato2;
    doc["dato3"] = dato3;
    doc["dato4"] = dato4;

    // Convertir el documento JSON a una cadena
    String postData;
    serializeJson(doc, postData);

    Serial.print("Post Data: ");
    Serial.println(postData); // Imprimir los datos que se van a enviar

    http.addHeader("Content-Type", "application/json"); // Añadir la cabecera HTTP
    int httpResponseCode = http.POST(postData); // Enviar la solicitud POST

    if (httpResponseCode > 0) { // Verificar si la solicitud fue exitosa
        String response = http.getString(); // Obtener la respuesta del servidor
        Serial.println("Datos enviados: " + response); // Imprimir la respuesta
    } else { // Si hubo un error en la solicitud
        Serial.println("Error enviando datos: " + String(httpResponseCode)); // Imprimir el código de error
    }

    http.end(); // Finalizar la conexión HTTP
}

void setup() {
    Serial.begin(9600); // Iniciar la comunicación serial por hardware para el monitor serial
    mySerial.begin(9600, SERIAL_8N1, 12, 14); // Iniciar la comunicación serial por hardware en UART1 con pines 12 (RX) y 14 (TX)
    connectToWiFi(); // Conectar a la red WiFi
}

void loop() {
    if (mySerial.available() > 0) { // Verificar si hay datos disponibles en el puerto serial por hardware
        String data = mySerial.readStringUntil('\n'); // Leer los datos hasta el carácter de nueva línea
        
        // Crear un objeto JSON y parsear la cadena recibida
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, data);
        
        // Verificar si hubo un error al parsear el JSON
        if (error) {
            Serial.print("Error al parsear JSON: ");
            Serial.println(error.c_str());
            return;
        }
        
        // Extraer los datos del JSON
        unsigned int dato1 = doc["dato1"];
        unsigned int dato2 = doc["dato2"];
        unsigned int dato3 = doc["dato3"];
        int dato4 = doc["dato4"];
        
        // Enviar los datos al servidor PHP
        sendDataToPHP(device_id, dato1, dato2, dato3, dato4);
    }
    delay(1000); // Esperar un segundo antes de verificar nuevamente la entrada serial
}

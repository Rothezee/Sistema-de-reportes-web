#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

const char* ssid = "HUAWEI-2.4G-Dqc5";
const char* password = "46332714";
const char* serverAddress = "http://192.168.100.5/esp32_project/insert_data_videojuego.php"; // Cambia la dirección IP por la de tu servidor
const char* device_id = "Videojuego_1";
HardwareSerial mySerial(2); // Usar el puerto serial 2 del ESP32

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX en pines 16 y 17 (ajusta según sea necesario)
    connectToWiFi();
}

void loop() {
    if (mySerial.available()) {
        String receivedData = mySerial.readStringUntil('\n');
        Serial.println("Received: " + receivedData);
        
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, receivedData);

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.f_str());
            return;
        }

        const char* device_id = doc["device_id"];
        int dato2 = doc["coin"];
        
        sendDataToPHP(device_id, dato2);
    }
    delay(1000);
}

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

void sendDataToPHP(const char* device_id, int dato2) {
    Serial.println("Enviando datos al servidor PHP:");
    Serial.print("device_id: ");
    Serial.println(device_id);
    Serial.print("dato2: ");
    Serial.println(dato2);

    HTTPClient http;
    http.begin(serverAddress);

    // Crear cuerpo de la solicitud JSON
    String postData = "{\"device_id\":\"" + String(device_id) + "\",\"dato2\":" + String(dato2) + "}";

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
}

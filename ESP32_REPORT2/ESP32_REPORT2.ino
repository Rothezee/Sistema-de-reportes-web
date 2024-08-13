#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "HUAWEI-2.4G-Dqc5";
const char* password = "46332714";
const char* serverAddress = "http://192.168.100.5/esp32_project/insert_data.php"; // Cambia la dirección IP por la de tu servidor

int dato1 = 0;
int dato2 = 0;
int dato3 = 0;
int dato4 = 0;

const char* device_id = "ESP32_002"; // Identificador único del ESP32

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
    Serial.print("Dato4: ");
    Serial.println(dato4);

    HTTPClient http;
    http.begin(serverAddress);

    // Crear cuerpo de la solicitud JSON
    String postData = "{\"device_id\":\"" + String(device_id) + "\",\"dato1\":" + String(dato1) + ",\"dato2\":" + String(dato2) + ",\"dato3\":" + String(dato3) + "\",\"dato4\":" + String(dato4) + "}";

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
    dato4++;
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();
}

void loop() {
    sendDataToPHP(); // Envía los datos al servidor PHP
    delay(60000);    // Espera 1 minuto (60 segundos)
}

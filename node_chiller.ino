// node chiller.ino

#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// Data pin yang terhubung ke sensor DS18B20
#define ONE_WIRE_BUS_1 5  // Pin untuk sensor pertama
#define ONE_WIRE_BUS_2 16  // Pin untuk sensor kedua

#define BUZZER_PIN 2 // Pin untuk buzzer

OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);

DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);

// LCD settings
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "TPF-Office";
const char* password = "ToshinPrim@2023";
const char* host = "10.100.4.180";
const char* esp32ReceiverIP = "10.100.4.60";  // IP address of the receiving ESP32

// Static IP configuration
IPAddress local_IP(10, 100, 4, 50);  // Set your desired IP address
IPAddress gateway(10, 100, 4, 1);    // Set your network gateway
IPAddress subnet(255, 255, 255, 0);  // Set your subnet mask
IPAddress primaryDNS(8, 8, 8, 8);    // Optional: set a primary DNS server
IPAddress secondaryDNS(8, 8, 4, 4);  // Optional: set a secondary DNS server

unsigned long previousMillisDatabase = 0; // Stores last time data was sent to database
unsigned long previousMillisESP32 = 0;    // Stores last time data was sent to ESP32
const long intervalDatabase = 30000;      // Interval to send data to database (30 seconds)
const long intervalESP32 = 3000;          // Interval to send data to ESP32 (3 seconds)

void setup() {
    Serial.begin(115200);
    Serial.println("DS18B20 Output!");

    sensors1.begin(); // Start the first DS18B20 sensor
    sensors2.begin(); // Start the second DS18B20 sensor

    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");

    pinMode(BUZZER_PIN, OUTPUT); // Mengatur pin buzzer sebagai output
    digitalWrite(BUZZER_PIN, LOW); // Matikan buzzer pada awalnya

    // Configure the ESP32 to use a static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
    }

    Serial.println("\nConnecting to " + String(ssid));
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.print("WiFi Connected");
}

void loop() {
    // Get current time
    unsigned long currentMillis = millis();

    // Request temperature readings from both sensors
    sensors1.requestTemperatures(); 
    sensors2.requestTemperatures(); 
    
    float suhuChiller1 = sensors1.getTempCByIndex(0); // Reading temperature from sensor 1 in Celsius
    float suhuChiller2 = sensors2.getTempCByIndex(0); // Reading temperature from sensor 2 in Celsius
    
    lcd.clear();
    lcd.setCursor(0, 0);

    if (suhuChiller1 == DEVICE_DISCONNECTED_C) {
        lcd.print("Chiller 1 Error");
    } else {
        lcd.print("Chiller1: ");
        lcd.print(suhuChiller1);
        lcd.print(" C");
    }

    lcd.setCursor(0, 1);

    if (suhuChiller2 != DEVICE_DISCONNECTED_C) {
        lcd.print("Chiller2: ");
        lcd.print(suhuChiller2);
        lcd.print(" C");
    } else {
        lcd.print("Chiller 2 Error");
    }

    // Send data to database every 30 seconds
    if (currentMillis - previousMillisDatabase >= intervalDatabase) {
        previousMillisDatabase = currentMillis;
        sendToDatabase(suhuChiller1, suhuChiller2);
    }

    // Send temperature values to the other ESP32 every 3 seconds
    if (currentMillis - previousMillisESP32 >= intervalESP32) {
        previousMillisESP32 = currentMillis;
        sendTemperatureToESP32(suhuChiller1, suhuChiller2);
    }

    delay(500); // Short delay to prevent overwhelming the CPU
}

// Fungsi untuk menangani peringatan dari server
void handleWarning() {
    // Menyalakan buzzer ketika peringatan diterima
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000); // Buzzer menyala selama 1 detik
    digitalWrite(BUZZER_PIN, LOW);
}

// Function to send temperature values to the database
void sendToDatabase(float suhuChiller1, float suhuChiller2) {
    Serial.print("Connecting to ");
    Serial.println(host);

    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("Connection to database server failed");
    } else {
        // Construct the URL for the GET request to the database
        String url = "/monitoring_toshin/connect.php?suhuchiller1=" + String(suhuChiller1) + "&suhuchiller2=" + String(suhuChiller2);
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) { // Client Timeout after 5 seconds
                Serial.println(">>> Client Timeout !");
                client.stop();
                break;
            }
        }

        // Read and print the response from the server
        while(client.available()) {
            String line = client.readStringUntil('\r');
            Serial.print(line);

            // Handle warning if "warning" received from the server
            if (line.indexOf("warning") != -1) {
                handleWarning();
            }
        }

        Serial.println("\nClosing connection with database server");
        client.stop();
    }
}

// Function to send temperature values to another ESP32
void sendTemperatureToESP32(float suhu1, float suhu2) {
    WiFiClient client;
    if (!client.connect(esp32ReceiverIP, 80)) {
        Serial.println("Connection to receiver ESP32 failed");
        return;
    }

    String url = "/receive_temp?suhu1=" + String(suhu1) + "&suhu2=" + String(suhu2);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + esp32ReceiverIP + "\r\n" + 
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) { // Client Timeout after 5 seconds
            Serial.println(">>> Receiver ESP32 Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read and print the response from the receiver ESP32
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println("\nClosing connection with receiver ESP32");
}

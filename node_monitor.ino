// node_monitor.ino

#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "TPF-Office";
const char* password = "ToshinPrim@2023";

// Static IP configuration
IPAddress local_IP(10, 100, 4, 60);  // Set your desired IP address
IPAddress gateway(10, 100, 4, 1);    // Set your network gateway
IPAddress subnet(255, 255, 255, 0);  // Set your subnet mask
IPAddress primaryDNS(8, 8, 8, 8);    // Optional: set a primary DNS server
IPAddress secondaryDNS(8, 8, 4, 4);  // Optional: set a secondary DNS server

// LCD settings
LiquidCrystal_I2C lcd(0x27, 20, 4);

WiFiServer server(80);

const int buzzerPin = 5; // Buzzer pin
bool buzzerState = false;
unsigned long previousMillisBuzzer = 0;
const long intervalBuzzer = 500;  // Interval for blinking buzzer (milliseconds)
bool isDanger = false;

void setup() {
    Serial.begin(115200);

    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");

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
    lcd.setCursor(0, 0);
    lcd.print("CHILLER MONITORING");

    server.begin();
    pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED pin as an output
    pinMode(buzzerPin, OUTPUT);   // Set the buzzer pin as an output
    digitalWrite(buzzerPin, HIGH); // Ensure buzzer is off initially (active low)
}

void loop() {
    unsigned long currentMillis = millis(); // Store the current time

    // Handle blinking buzzer independently
    handleBuzzer(currentMillis);

    // Handle WiFi client
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
        String currentLine = "";                // make a String to hold incoming data from the client
        float suhuChiller1 = 0.0;
        float suhuChiller2 = 0.0;
        bool tempReceived = false;

        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character
                    // Check if the HTTP request contains temperature data
                    if (currentLine.startsWith("GET /receive_temp?")) {
                        // Extract temperature values from the HTTP request
                        int suhu1Index = currentLine.indexOf("suhu1=");
                        int suhu2Index = currentLine.indexOf("&suhu2=");
                        if (suhu1Index != -1 && suhu2Index != -1) {
                            suhuChiller1 = currentLine.substring(suhu1Index + 6, suhu2Index).toFloat();
                            suhuChiller2 = currentLine.substring(suhu2Index + 7).toFloat();
                            tempReceived = true;
                        }
                    }

                    // Clear the currentLine
                    currentLine = "";
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                // Check if temperature data has been received and exit the loop if yes
                if (tempReceived) {
                    break;
                }
            }
        }

        // Send HTTP response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();

        // Update LCD display
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CHILLER MONITORING");
        lcd.setCursor(0, 1);
        lcd.print("Suhu 1: ");
        lcd.print(suhuChiller1);
        lcd.print(" C");
        lcd.setCursor(0, 2);
        lcd.print("Suhu 2: ");
        lcd.print(suhuChiller2);
        lcd.print(" C");

        isDanger = (suhuChiller1 > 30 || suhuChiller2 > 30);

        Serial.print("Suhu Chiller 1: ");
        Serial.println(suhuChiller1);
        Serial.print("Suhu Chiller 2: ");
        Serial.println(suhuChiller2);
        Serial.print("isDanger: ");
        Serial.println(isDanger);

        if (isDanger) {
            // Display "BAHAYA" on LCD
            lcd.setCursor(0, 3);
            lcd.print("BAHAYA");
            
            // Print "BAHAYA" to serial monitor
            Serial.println("BAHAYA");

            // Turn on LED
            digitalWrite(LED_BUILTIN, HIGH);
        } else {
            // Display "AMAN" on LCD
            lcd.setCursor(0, 3);
            lcd.print("AMAN");

            // Print "AMAN" to serial monitor
            Serial.println("AMAN");

            // Turn off LED
            digitalWrite(LED_BUILTIN, LOW);

            // Turn off buzzer
            digitalWrite(buzzerPin, HIGH); // Low level trigger
        }

        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
    }
}

void handleBuzzer(unsigned long currentMillis) {
    if (currentMillis - previousMillisBuzzer >= intervalBuzzer) {
        previousMillisBuzzer = currentMillis;

        if (isDanger) {  // Only blink buzzer if in danger state
            buzzerState = !buzzerState;
            digitalWrite(buzzerPin, buzzerState ? LOW : HIGH); // Low level trigger
        } else {
            digitalWrite(buzzerPin, HIGH); // Ensure buzzer is off if not in danger state
        }
    }
}

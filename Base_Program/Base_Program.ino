#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Your Domain name with URL path or IP address with path
String serverName = "URL/iot/store";

// Define the DHT22 sensor pin
#define DHTPIN 2  // Change this to the appropriate pin

// Create a DHT object
DHT dht(DHTPIN, DHT22);

// The following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before sending the first POST request.");
}

void loop() {
  // Send an HTTP POST request every 5 seconds
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(serverName.c_str());

      // Read temperature and humidity from the DHT22 sensor
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      // Set the code value as a variable
      String code = "iot-a";

      // Add the POST parameters
      http.addHeader("Content-Type", "multipart/form-data");
      String postData = "code=" + code + "&humidity=" + String(humidity, 2) + "&temperature=" + String(temperature, 2);
      int httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString(); // Store the response in the payload variable
        Serial.println("Response:");
        Serial.println(payload); // Print the response to the serial monitor
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

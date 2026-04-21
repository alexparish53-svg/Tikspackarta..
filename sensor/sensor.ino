#include <WiFi.h>


// --- DIN MOBILS HOTSPOT-INFO ---
const char* ssid = "Alex";
const char* password = "test1234";


// --- PINS (Kolla att dessa matchar dina kablar!) ---
const int trigPin = 5;
const int echoPin = 18;


WiFiServer server(80);


void setup() {
  Serial.begin(115200);
 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  // Starta WiFi-anslutningen
  Serial.print("Ansluter till ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.print(".");
  }


  // Skriv ut IP-adressen (Denna ska du skriva in i din HTML-fil!)
  Serial.println("");
  Serial.println("WiFi anslutet!");
  Serial.print("IP-adress: ");
  Serial.println(WiFi.localIP());


  server.begin();
}


void loop() {
  // 1. MÄT AVSTÅNDET MED SENSORN
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  long duration = pulseIn(echoPin, HIGH);
  // Vi räknar ut avståndet i cm
  int distance = duration * 0.034 / 2;


  // Skriv ut i Serial Monitor så du ser att sensorn lever
  if (distance > 400 || distance < 2) {
    Serial.println("Utanför räckvidd");
  } else {
    Serial.print("Avstånd: ");
    Serial.print(distance);
    Serial.println(" cm");
  }


  // 2. LYSSNA EFTER DIN WEBBSIDA
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Webbsidan hämtar data...");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // SKICKA SVAR TILL WEBBSIDAN
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            // Viktigt: Tillåter din lokala HTML-fil att läsa datan
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: close");
            client.println();
           
            // Här skickas bara siffran, t.ex. "15"
            client.print(distance);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
 
  delay(1000); // Liten paus för stabilitet
}

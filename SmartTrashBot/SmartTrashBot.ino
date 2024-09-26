#include <ESP8266WiFi.h> // ESP8266 kullanıyorsanız
#include <WiFiClient.h>

const char* ssid = "YOUR_SSID"; // WiFi SSID
const char* password = "YOUR_PASSWORD"; // WiFi şifresi
const char* host = "your-api-host.com"; // API sunucu adresi

const int sol_enable = 11;
const int sag_ileri = 10;
const int sag_geri = 9;
const int sol_ileri = 8;
const int sol_geri = 7;
const int sag_enable = 6;

void setup() {
  Serial.begin(9600);
  pinMode(sag_ileri, OUTPUT);
  pinMode(sag_geri, OUTPUT);
  pinMode(sol_ileri, OUTPUT);
  pinMode(sol_geri, OUTPUT);
  pinMode(sag_enable, OUTPUT);
  pinMode(sol_enable, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  WiFiClient client;
  if (client.connect(host, 80)) { // API'ye bağlan
    client.print(String("GET /api/commands/ HTTP/1.1\r\n") +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(500);
    
    while (client.available()) {
      String line = client.readStringUntil('\r');
      if (line.startsWith("Command: ")) {
        char gelen_veri = line.charAt(9); // Komut karakterini al
        Serial.println(gelen_veri); // Gelen veriyi seri monitöre yazdır

        // Motorları kontrol et
        switch (gelen_veri) {
          case 'F': // İleri hareket
            digitalWrite(sag_ileri, HIGH);
            digitalWrite(sag_geri, LOW);
            digitalWrite(sol_ileri, HIGH);
            digitalWrite(sol_geri, LOW);
            analogWrite(sag_enable, 255); // Motor hızlarını ayarla
            analogWrite(sol_enable, 255);
            break;

          case 'R': // Sağ ileri
            digitalWrite(sag_ileri, HIGH);
            digitalWrite(sag_geri, LOW);
            digitalWrite(sol_ileri, LOW);
            digitalWrite(sol_geri, LOW);
            analogWrite(sag_enable, 255);
            analogWrite(sol_enable, 0);
            break;

          case 'L': // Sol ileri
            digitalWrite(sag_ileri, LOW);
            digitalWrite(sag_geri, LOW);
            digitalWrite(sol_ileri, HIGH);
            digitalWrite(sol_geri, LOW);
            analogWrite(sag_enable, 0);
            analogWrite(sol_enable, 255);
            break;

          case 'B': // Geri hareket
            digitalWrite(sag_ileri, LOW);
            digitalWrite(sag_geri, HIGH);
            digitalWrite(sol_ileri, LOW);
            digitalWrite(sol_geri, HIGH);
            analogWrite(sag_enable, 255);
            analogWrite(sol_enable, 255);
            break;

          case 'S': // Dur
            digitalWrite(sag_ileri, LOW);
            digitalWrite(sag_geri, LOW);
            digitalWrite(sol_ileri, LOW);
            digitalWrite(sol_geri, LOW);
            analogWrite(sag_enable, 0);
            analogWrite(sol_enable, 0);
            break;

          default:
            // Geçersiz komutlar için bir şey yapma
            break;
        }
      }
    }
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
  delay(10000); // Her 10 saniyede bir isteği tekrar et
}

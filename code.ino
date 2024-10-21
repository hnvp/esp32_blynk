#define BLYNK_TEMPLATE_ID "TMPL6SXFVIBQt"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#define BLYNK_AUTH_TOKEN "CzG3J0CcROu4Elj1ILU08iaVoWKPe9uY"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHTPIN 21         // Chân kết nối của DHT22 với ESP32
#define DHTTYPE DHT22     // Loại cảm biến DHT
DHT dht(DHTPIN, DHTTYPE);

#define LED_PIN 27        // Chân kết nối của LED (bơm nước) với ESP32
WidgetLED LED_ON_APP(V2);
int button;

#define POT_PIN 34        // Chân kết nối của Slide Potentiometer với ESP32 (sử dụng chân analog input 34)
#define MAX_MOISTURE 100  // Độ ẩm tối đa

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

bool pumpStatus = false; // Biến lưu trạng thái của bơm nước

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(auth, ssid, pass);
}

BLYNK_WRITE(V3) {
  pumpStatus = param.asInt(); // Lấy giá trị của nút nhấn WATER PUMP từ Blynk App
  digitalWrite(LED_PIN, pumpStatus); // Bật hoặc tắt LED tương ứng với trạng thái của nút nhấn
}

void loop() {
  Blynk.run();
  unsigned long currentMillis = millis();

  float temp = dht.readTemperature();     // Đọc nhiệt độ từ DHT22
  float humi = dht.readHumidity();        // Đọc độ ẩm từ DHT22
  int moisRaw = analogRead(POT_PIN);      // Đọc giá trị từ Slide Potentiometer
  int mois = map(moisRaw, 0, 4095, 0, 100); // Chuyển đổi giá trị đọc từ cảm biến sang phạm vi từ 0 đến 100

  // Gửi dữ liệu nhiệt độ và độ ẩm lên Blynk
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, humi);
  Blynk.virtualWrite(V2, mois);

  if (isnan(humi) || isnan(temp)) {
    delay(500);
    Serial.println("Failed to read from DHT sensor!\n");
    return;
  }

  // Kiểm tra trạng thái của bơm nước và độ ẩm
  if (pumpStatus && mois < MAX_MOISTURE && Blynk.connected()) {
    digitalWrite(LED_PIN, HIGH); // Bật đèn LED nếu cả hai điều kiện đều đúng
  } else {
    digitalWrite(LED_PIN, LOW); // Tắt đèn LED nếu bất kỳ điều kiện nào không đúng
  }

  // In ra giá trị nhiệt độ và độ ẩm ra Serial Monitor
  Serial.print("\n");
  Serial.print("Temperature: " + String(temp) + "°C");
  Serial.print("\t");
  Serial.print("Humidity: " + String(humi) + "%");
  Serial.print("\t");
  Serial.print("Moisture: " + String(mois) + "%");

  delay(2000); // Delay 2 giây trước khi đọc lại dữ liệu
}

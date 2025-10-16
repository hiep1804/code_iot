#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// Thông tin Wi-Fi
const char *ssid = "nghiep";
const char *password = "12345678";

String serverUrl = "http://192.168.137.1:8080/data"; // địa chỉ máy tính + port server

int redPin = 14;
int greenPin = 12;
int bluePin = 13;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Đã kết nối Wi-Fi!");
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

// Hàm trộn màu RGB (0–255)
void setColor(int r, int g, int b)
{
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverUrl);        // kết nối tới server trên máy tính
    int httpCode = http.POST(""); // gửi POST request

    if (httpCode > 0)
    {                                    // kiểm tra HTTP response
      String payload = http.getString(); // <-- nhận chuỗi JSON từ server
      Serial.println(payload);
      // parse dữ liệu
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      String type = doc["type"].as<String>();
      if (type == "tắt đèn")
      {
        setColor(0, 0, 0);
      }
      if (type == "bật đèn")
      {
        // lấy các thuộc tính của đèn
        JsonObject obj = doc["obj"];
        int level = obj["level"];
        int r = obj["r"];
        int g = obj["g"];
        int b = obj["b"];
        Serial.printf("r=%d, g=%d, b=%d, level=%d\n", r, g, b, level);
        // float sr = 0.0015;
        // float sg = 0.0008;
        // float sb = 0.00005;
        // r = (sr / 0.299 * 220 * r / 255 + 2) / 3.3 * 255 * (10 + level - 4) / 10;
        // g = (sg / 0.587 * 220 * g / 255 + 3) / 3.3 * 255 * (10 + level - 4) / 10;
        // r = (sb / 0.114 * 220 * b / 255 + 3.2) / 3.3 * 255 * (10 + level - 4) / 10;
        r=r*(float)0.38*(10+level-4)/10;
        g=g*(float)0.19*(10+level-4)/10;
        b=b*(float)1*(10+level-4)/10;
        Serial.printf("r=%d, g=%d, b=%d, level=%d\n", r, g, b, level);
        setColor(r, g, b);
      }
    }
    else
    {
      Serial.println("Lỗi kết nối: " + String(httpCode));
    }

    http.end(); // đóng kết nối
  }
  delay(2000); // gửi request mỗi 5 giây
}
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// Thông tin Wi-Fi
const char *ssid = "nghiep";
const char *password = "12345678";

String serverUrl = "http://192.168.137.1:8080"; // địa chỉ máy tính + port server

int redPin = 14;
int greenPin = 12;
int bluePin = 13;

int light_sensor=33;

int r = 0;
int g = 0;
int b = 0;
int level = 4;

long long begin_time = 0;
void setup()
{
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(light_sensor,INPUT);
}

// Hàm trộn màu RGB (0–255)
void setColor(int r, int g, int b)
{
  r = r * (float)0.38 * (10 + level - 4) / 10;
  g = g * (float)0.19 * (10 + level - 4) / 10;
  b = b * (float)1 * (10 + level - 4) / 10;
  Serial.printf("r=%d, g=%d, b=%d, level=%d\n", r, g, b, level);
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}
void statusFromSensor(){
  int val=analogRead(light_sensor);
  setColor(val/16,val/16,val/16);
  delay(1000);
}
void loop()
{
  long long milis = millis();
  //getColorFromServer();
  statusFromSensor();
  long long period = millis() - milis;
  long long t = period / 1000;
  delay((t + 1) * 1000 - period); // gửi request mỗi 1 giây
}
void getColorFromServer(){
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverUrl + "/data"); // kết nối tới server trên máy tính
    int httpCode = http.POST("");    // gửi POST request

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
        level = obj["level"];
        r = obj["r"];
        g = obj["g"];
        b = obj["b"];
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
}
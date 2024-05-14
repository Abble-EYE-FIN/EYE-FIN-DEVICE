#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "MPU9250.h"
#include <ArduinoJson.h>
MPU9250 mpu;

struct SensorData{
  float acc_X;
  float acc_Y;
  float acc_Z;
  float deg_X;
  float deg_Y;
  float deg_Z;
};

//와이파이 연결을 위한 와이파이 ssid와 pasword입
const char* ssid = "Galaxy S24U";
const char* password = "01035307712";

const char* mqtt_server = "192.168.206.32"; // MQTT 통신을 위한 브로커 IP 
const int mqtt_port = 1883; // MQTT 브로커의 포트 번호를 설정, 일반적으로 1883
const char* mqtt_topic = "righthand/input"; //토픽 정의 
char buffer[512];

WiFiClient espClient; 
PubSubClient client(espClient); //esp모듈을 클라이언트로 지정. 
//client : mqtt에 연결된 모든 것 
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//callback 함수 :  MQTT 클라이언트가 메시지를 수신했을 때 실행. 
//callback 함수는 MQTT 클라이언트가 메시지를 수신했을 때, 그 메시지를 처리하는 사용자 정의 함수
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) { //연결끊어졌을 때 
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //mpu setup
  Wire.begin();
  delay(2000);
  if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Send JSON 
  //StaticJsonDocument<200> doc;
  // ArduinoJson 라이브러리에서 제공하는 클래스로, JSON 데이터를 메모리에 저장하고 다루는 데 사용
  //doc["sensor"] = "temperature"; //센서 연결...
  //doc["value"] = 25.5;

   
//  serializeJson(doc, buffer);
//serializeJson() 함수는 ArduinoJson 라이브러리에서 제공되며, StaticJsonDocument나 DynamicJsonDocument와 같은 JSON 객체를 문자열로 직렬화(serialize)하는 역할
  SensorData Sensor;
    if (mpu.update()) {
        static uint32_t prev_ms = millis(); 
        if (millis() > prev_ms + 25) {
            
            Sensor.acc_X= mpu.getYaw();
            Serial.println(Sensor.acc_X); 
            Sensor.acc_Y=mpu.getPitch();
            Serial.println(Sensor.acc_Y); 
            Sensor.acc_Z=mpu.getRoll();
            Serial.println(Sensor.acc_Z); 
            prev_ms = millis();
            convertToJson(Sensor);
        }
    }
  
  if (client.publish(mqtt_topic, buffer)) { //토픽 발행 
    Serial.println("JSON message sent to the server");
  } else {
    Serial.println("Failed to send JSON message");
  }

  delay(5000);  // Send every 5 seconds



}


void convertToJson(const SensorData& Sensor) {
    // JSON 문서 생성, 크기는 필요에 따라 조정
    StaticJsonDocument<200> doc;
   
    // 구조체 데이터를 JSON 객체에 복사
    doc["X"] = Sensor.acc_X;
    doc["Y"] = Sensor.acc_Y;
    doc["Z"] = Sensor.acc_Z;

    // JSON 객체를 시리얼 포트로 출력
    serializeJson(doc, buffer);
    Serial.println(); // 줄바꿈 추가
}


void print_roll_pitch_yaw() {
    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(mpu.getYaw(), 2);
    Serial.print(", ");
    Serial.print(mpu.getPitch(), 2);
    Serial.print(", ");
    Serial.println(mpu.getRoll(), 2);
}

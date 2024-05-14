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

void setup() {
    Serial.begin(115200);
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
}

void convertToJson(const SensorData& Sensor) {
    // JSON 문서 생성, 크기는 필요에 따라 조정
    StaticJsonDocument<200> doc;

    // 구조체 데이터를 JSON 객체에 복사
    doc["X"] = Sensor.acc_X;
    doc["Y"] = Sensor.acc_Y;
    doc["Z"] = Sensor.acc_Z;

    // JSON 객체를 시리얼 포트로 출력
    serializeJson(doc, Serial);
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

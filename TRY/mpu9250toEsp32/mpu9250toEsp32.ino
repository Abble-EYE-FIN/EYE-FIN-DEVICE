#include <Arduino.h>
#include <Wire.h>
#include "MPU9250.h"

MPU9250 mpu1; // 첫 번째 MPU9250 센서 객체 생성
MPU9250 mpu2; // 두 번째 MPU9250 센서 객체 생성

const float accelThreshold = 0.5; // 가속도계 데이터 필터링을 위한 임의의 임계값
const float gyroThreshold = 5.0;  // 자이로스코프 데이터 필터링을 위한 임의의 임계값

struct SensorValue {
  String name;  // 센서 값의 이름 (예: "mpu1_accelX")
  float value;  // 센서 값
};

struct SensorInfo {
  String name;                 // 센서의 식별자 (예: "mpu1", "mpu2")
  SensorValue sensorValues[6]; // 센서 값 배열
};

void setupSensorValues(SensorInfo &sensorInfo, const String &sensorName) {
  String valueNames[6] = {"accelX", "accelY", "accelZ", "gyroX", "gyroY", "gyroZ"};
  for (int i = 0; i < 6; i++) {
    sensorInfo.sensorValues[i].name = sensorName + "_" + valueNames[i];
  }
}

SensorInfo getFilteredSensorData(MPU9250& mpu, const String& sensorName) {
  SensorInfo sensorInfo;
  sensorInfo.name = sensorName;
  setupSensorValues(sensorInfo, sensorName);

  float sensorReadings[6] = {
    mpu.getAccBiasX(), mpu.getAccBiasY(), mpu.getAccBiasZ(),
    mpu.getGyroBiasX(), mpu.getGyroBiasY(), mpu.getGyroBiasZ()
  };

  for (int i = 0; i < 3; i++) { // 가속도계 값 필터링
    sensorInfo.sensorValues[i].value = (abs(sensorReadings[i]) > accelThreshold) ? sensorReadings[i] : 0.0;
  }

  for (int i = 3; i < 6; i++) { // 자이로스코프 값 필터링
    sensorInfo.sensorValues[i].value = (abs(sensorReadings[i]) > gyroThreshold) ? sensorReadings[i] : 0.0;
  }

  return sensorInfo;
}

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작
  Wire.begin();         // I2C 통신 시작
  delay(2000);          // 센서 초기화 지연

  if (!mpu1.setup(0x68)) { // 첫 번째 센서 초기화 실패 시
    Serial.println("MPU1 connection failed!");
  }

  if (!mpu2.setup(0x69)) { // 두 번째 센서 초기화 실패 시
    Serial.println("MPU2 connection failed!");
  }
}

void loop() {
  SensorInfo filteredSensorInfo1 = getFilteredSensorData(mpu1, "mpu1");
  SensorInfo filteredSensorInfo2 = getFilteredSensorData(mpu2, "mpu2");
 
  printSensorData(filteredSensorInfo1); // 첫 번째 센서 데이터 출력
  printSensorData(filteredSensorInfo2); // 두 번째 센서 데이터 출력

  delay(1000); // 1초 대기
}

void printSensorData(const SensorInfo& sensorInfo) {
  Serial.println(sensorInfo.name + " Data:"); // 센서 식별자 출력
  for (int i = 0; i < 6; i++) {
    Serial.print(sensorInfo.sensorValues[i].name + ": ");
    Serial.println(sensorInfo.sensorValues[i].value, 6); // 센서 값 출력
  }
}

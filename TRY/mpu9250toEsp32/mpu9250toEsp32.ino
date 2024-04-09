// 필요한 라이브러리 포함
#include <Arduino.h> // Arduino 기능을 사용하기 위한 기본 헤더 파일
#include <Wire.h>    // Arduino의 I2C 통신을 위한 헤더 파일
#include "MPU9250.h" // MPU9250 센서를 제어하기 위한 라이브러리

// 센서 객체 생성
MPU9250 mpu1; // 첫 번째 MPU9250 센서 객체
MPU9250 mpu2; // 두 번째 MPU9250 센서 객체

// 필터링을 위한 임계값 설정
const float accelThreshold = 0.5; // 가속도 데이터를 필터링하기 위한 임계값
const float gyroThreshold = 5.0;  // 자이로스코프 데이터를 필터링하기 위한 임계값

// 센서 값 저장을 위한 구조체 정의
struct SensorValue {
  String name;  // 센서 값의 이름 (예: "mpu1_accelX"는 첫 번째 센서의 X축 가속도를 의미)
  float value;  // 센서 값
};

struct SensorInfo {
  String name;                 // 센서의 식별자 (예: "mpu1", "mpu2")
  SensorValue sensorValues[6]; // 가속도계 X, Y, Z와 자이로스코프 X, Y, Z 축의 값을 저장
};

struct SensorsData {
  SensorInfo sensor1; // 첫 번째 센서 정보
  SensorInfo sensor2; // 두 번째 센서 정보
};

// 센서 값 구조체를 초기화하는 함수
void setupSensorValues(SensorInfo &sensorInfo, const String &sensorName) {
  // 각 축에 대한 데이터 이름을 설정. 예: "accelX", "accelY" 등
  String valueNames[6] = {"accelX", "accelY", "accelZ", "gyroX", "gyroY", "gyroZ"};
  for (int i = 0; i < 6; i++) {
    sensorInfo.sensorValues[i].name = sensorName + "_" + valueNames[i]; // 센서 값 이름 설정. '센서명_데이터유형' 
  }
}

// 센서로부터 데이터를 읽고 필터링하는 함수
SensorInfo getFilteredSensorData(MPU9250& mpu, const String& sensorName) {
  SensorInfo sensorInfo;// 센서 데이터를 저장할 임시 구조체를 생성
  sensorInfo.name = sensorName; // 센서 이름 설정
  setupSensorValues(sensorInfo, sensorName); // 센서 값 이름 초기화

  // MPU 센서로부터 데이터 읽기
  float sensorReadings[6] = {
    mpu.getAccBiasX(), mpu.getAccBiasY(), mpu.getAccBiasZ(),
    mpu.getGyroBiasX(), mpu.getGyroBiasY(), mpu.getGyroBiasZ()
  };

  // 데이터 필터링
  for (int i = 0; i < 3; i++) { // 가속도계 데이터 필터링
    sensorInfo.sensorValues[i].value = (abs(sensorReadings[i]) > accelThreshold) ? sensorReadings[i] : 0.0;
  }
  for (int i = 3; i < 6; i++) { // 자이로스코프 데이터 필터링
    sensorInfo.sensorValues[i].value = (abs(sensorReadings[i]) > gyroThreshold) ? sensorReadings[i] : 0.0;
  }

  return sensorInfo;// 필터링된 데이터를 포함하는 구조체를 반환
}

// SensorsData 구조체를 반환하는 함수
SensorsData getSensorsData() {
  SensorsData sensors; // 두 센서 데이터 구조체 생성
  sensors.sensor1 = getFilteredSensorData(mpu1, "mpu1"); // 첫 번째 센서 데이터 필터링
  sensors.sensor2 = getFilteredSensorData(mpu2, "mpu2"); // 두 번째 센서 데이터 필터링
  return sensors; // 필터링된 데이터를 포함하는 구조체 반환
}

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작, baud rate는 115200
  Wire.begin();         // I2C 통신 시작
  delay(2000);          // 센서 초기화를 위해 2초간 대기

  // 센서 연결 상태 확인
  if (!mpu1.setup(0x68)) { // 첫 번째 센서 초기화 (I2C 주소 0x68)
    Serial.println("MPU1 connection failed!"); // 연결 실패 메시지 출력
  }
  if (!mpu2.setup(0x69)) { // 두 번째 센서 초기화 (I2C 주소 0x69)
    Serial.println("MPU2 connection failed!"); // 연결 실패 메시지 출력
  }
}

void loop() {
  SensorsData sensors = getSensorsData(); // 센서 데이터 구조체 받기

  printSensorData(sensors.sensor1); // 첫 번째 센서 데이터 출력
  printSensorData(sensors.sensor2); // 두 번째 센서 데이터 출력

  delay(1000); // 데이터 읽기 사이의 시간 지연은 1초
}

// 센서 데이터를 시리얼 포트를 통해 출력하는 함수
void printSensorData(const SensorInfo& sensorInfo) {
  Serial.println(sensorInfo.name + " Data:"); // 센서 식별자 출력
  for (int i = 0; i < 6; i++) { // 각 축에 대한 데이터 출력
    Serial.print(sensorInfo.sensorValues[i].name + ": ");
    Serial.println(sensorInfo.sensorValues[i].value, 6); // 소수점 아래 6자리까지 표시
  }
}

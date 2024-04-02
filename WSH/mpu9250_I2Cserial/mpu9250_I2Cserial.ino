#include <Arduino.h>
#include <Wire.h>
#include "MPU9250.h"

MPU9250 mpu; // MPU9250 센서를 제어하기 위한 MPU9250 객체입니다.

void setup() {
  Serial.begin(115200); // 시리얼 통신을 115200 보레이트로 초기화합니다.
  Wire.begin(); // I2C 통신을 시작합니다.
  delay(2000); // 2초 대기합니다.

  if (!mpu.setup(0x68)) { // MPU9250 센서를 주소 0x68로 설정하여 초기화합니다.
    while (1) { // 센서 초기화에 실패하면 무한 루프에 진입합니다.
      Serial.println("MPU connection failed!");
      delay(5000); // 5초 대기합니다.
    }
  }
}

void loop() {
  if (!mpu.update()) return; // MPU9250 센서에서 데이터를 업데이트합니다. 실패하면 함수를 종료합니다.

  float ax = mpu.getAccelX(); // 가속도계 X 축의 값을 가져옵니다.
  float ay = mpu.getAccelY(); // 가속도계 Y 축의 값을 가져옵니다.
  float az = mpu.getAccelZ(); // 가속도계 Z 축의 값을 가져옵니다.
  float gx = mpu.getGyroX();  // 자이로스코프 X 축의 값을 가져옵니다.
  float gy = mpu.getGyroY();  // 자이로스코프 Y 축의 값을 가져옵니다.
  float gz = mpu.getGyroZ();  // 자이로스코프 Z 축의 값을 가져옵니다.

  // 시리얼 모니터를 통해 데이터 출력
  Serial.print("AccelX: "); Serial.println(ax);
  Serial.print("AccelY: "); Serial.println(ay);
  Serial.print("AccelZ: "); Serial.println(az);
  Serial.print("GyroX: "); Serial.println(gx);
  Serial.print("GyroY: "); Serial.println(gy);
  Serial.print("GyroZ: "); Serial.println(gz);
  
  delay(1000); // 1초마다 데이터 출력을 위해 대기합니다.
}


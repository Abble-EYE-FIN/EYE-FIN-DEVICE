#include <Arduino.h>
#include <Wire.h>
#include "MPU9250.h"

MPU9250 mpu;        // MPU9250 센서를 제어하기 위한 MPU9250 객체입니다.

void setup() {
  Serial.begin(115200);  // 시리얼 통신을 115200 보레이트로 초기화합니다.
  Wire.begin();          // I2C 통신을 시작합니다.
  delay(2000);           // 2초 대기합니다.

  while (!Serial) {}  // 시리얼 통신이 준비될 때까지 대기합니다

  if (!mpu.setup(0x68)) {  // MPU9250 센서를 주소 0x68로 설정하여 초기화합니다.
    while (1) {            // 센서 초기화에 실패하면 무한 루프에 진입합니다.
      Serial.println("MPU connection failed!");
      delay(5000);  // 5초 대기합니다.
    }
  }

  
}


void loop() {
  if (!mpu.update()) return;  // MPU9250 센서에서 데이터를 업데이트합니다. 실패하면 함수를 종료합니다.

  float acc_x = mpu.getAccBiasX();   // 가속도계 X 축의 값을 가져옵니다.
  float acc_y = mpu.getAccBiasY();   // 가속도계 Y 축의 값을 가져옵니다.
  float acc_z = mpu.getAccBiasZ();   // 가속도계 Z 축의 값을 가져옵니다.
  float deg_x = mpu.getGyroBiasX();  // 자이로스코프 X 축의 값을 가져옵니다.
  float deg_y = mpu.getGyroBiasY();  // 자이로스코프 Y 축의 값을 가져옵니다.
  float deg_z = mpu.getGyroBiasZ();  // 자이로스코프 Z 축의 값을 가져옵니다.
  float mag_x = mpu.getMagScaleX();  // 자기계 X 축의 값을 가져옴
  float mag_y = mpu.getMagScaleY();  // 자기계 Y 축의 값을 가져옴
  float mag_z = mpu.getMagScaleZ();  // 자기계 Z 축의 값을 가져옴

  // 데이터 전송
  sendSensorData(acc_x, acc_y, acc_z, deg_x, deg_y, deg_z, mag_x, mag_y, mag_z);  // 센서 데이터를 서버로 전송합니다.

  delay(1000);  // 1초마다 데이터 전송을 위해 대기합니다.
}

void sendSensorData(float accelX, float accelY, float accelZ, float gyroX, float gyroY, float gyroZ, float magX, float magY, float magZ) {
  
 
    // HTTP GET 요청을 생성하여 서버로 데이터를 전송합니다.
    Serial.print("GET /update/accelX=");
    Serial.print(accelX,6);
    Serial.print("\t");
    Serial.print("accelY=");
    Serial.print(accelY,6);
    Serial.print("\t");
    Serial.print("accelZ=");
    Serial.print(accelZ,6);
    Serial.print("\t");
    Serial.print("gyroX=");
    Serial.print(gyroX,6);
    Serial.print("\t");
    Serial.print("gyroY=");
    Serial.print(gyroY,6);
    Serial.print("\t");
    Serial.print("gyroZ=");
    Serial.print(gyroZ,6);
    Serial.print("\t");
    Serial.print("magX=");
    Serial.print(magX,6);
    Serial.print("\t");
    Serial.print("magY=");
    Serial.print(magY,6);
    Serial.print("\t");
    Serial.print("magZ=");
    Serial.print(magZ,6);
    
    print_calibration();
    delay(100);


  
}
void print_calibration() {
    Serial.println("< calibration parameters >");  // 보정 매개변수 출력 시작을 나타내는 메시지를 출력합니다.
    
    // 가속도계 보정값 출력
    Serial.println("accel bias [g]: ");  // 가속도계 보정값의 단위를 표시하는 메시지를 출력합니다.
    Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);  // x축 가속도계 보정값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);  // y축 가속도계 보정값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);  // z축 가속도계 보정값을 출력합니다.
    Serial.println();  // 줄 바꿈을 수행합니다.

    // 자이로스코프 보정값 출력
    Serial.println("gyro bias [deg/s]: ");  // 자이로스코프 보정값의 단위를 표시하는 메시지를 출력합니다.
    Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);  // x축 자이로스코프 보정값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);  // y축 자이로스코프 보정값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);  // z축 자이로스코프 보정값을 출력합니다.
    Serial.println();  // 줄 바꿈을 수행합니다.

    // 자기계 보정값 출력
    Serial.println("mag bias [mG]: ");  // 자기계 보정값의 단위를 표시하는 메시지를 출력합니다.
    Serial.print(mpu.getMagBiasX());  // x축 자기계 보정값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getMagBiasY());  // y축 자기계 보정값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getMagBiasZ());  // z축 자기계 보정값을 출력합니다.
    Serial.println();  // 줄 바꿈을 수행합니다.

    // 자기계 스케일링 값 출력
    Serial.println("mag scale []: ");  // 자기계 스케일링 값을 표시하는 메시지를 출력합니다.
    Serial.print(mpu.getMagScaleX());  // x축 자기계 스케일링 값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getMagScaleY());  // y축 자기계 스케일링 값을 출력합니다.
    Serial.print(", ");
    Serial.print(mpu.getMagScaleZ());  // z축 자기계 스케일링 값을 출력합니다.
    Serial.println();  // 줄 바꿈을 수행합니다.
}



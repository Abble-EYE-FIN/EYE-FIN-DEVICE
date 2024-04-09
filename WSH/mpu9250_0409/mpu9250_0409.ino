#include <Wire.h>
#include <MPU9250.h> // MPU9250 라이브러리 추가

MPU9250 mpu; // MPU9250 객체 선언

// 가속도계 및 자이로스코프 값을 저장하는 구조체 정의
struct SensorData {
    float acc_x;
    float acc_y;
    float acc_z;
    float deg_x;
    float deg_y;
    float deg_z;
};

void setup() {
  Serial.begin(9600);
  Wire.begin();

    
 
}

void printSensorData(SensorData data) {
    Serial.print("Accelerometer (m/s^2): ");
    Serial.print("X = "); Serial.print(data.acc_x);
    Serial.print(", Y = "); Serial.print(data.acc_y);
    Serial.print(", Z = "); Serial.println(data.acc_z);

    Serial.print("Gyroscope (deg/s): ");
    Serial.print("X = "); Serial.print(data.deg_x);
    Serial.print(", Y = "); Serial.print(data.deg_y);
    Serial.print(", Z = "); Serial.println(data.deg_z);
}


void loop() {
    // 가속도계 및 자이로스코프 값을 가져오는 함수 호출
    SensorData data = getSensorData();

    // 데이터 출력
    printSensorData(data);
    Serial.println("1"); //데이터 출력이 제대로 되는지 확인
    // 데이터를 바이너리 형태로 전송
    sendSensorData(data);
    Serial.println("2"); //데이터가 바이너리 형태로 전송이 되는 함수가 제대로 작동하는지 확인
    delay(1000); // 1초에 한 번씩 값을 가져옴
}

// 가속도계 및 자이로스코프 값을 반환하는 함수
SensorData getSensorData() {
    // 가속도계 및 자이로스코프 값을 저장할 구조체 선언
    SensorData data;

    // 가속도계 및 자이로스코프 값 가져오기
    data.acc_x = mpu.getAccX();
    data.acc_y = mpu.getAccY();
    data.acc_z = mpu.getAccZ();
    data.deg_x = mpu.getGyroX();
    data.deg_y = mpu.getGyroY();
    data.deg_z = mpu.getGyroZ();

    // 구조체 반환
    return data;
}

// 센서 데이터를 바이너리 형태로 변환하고 전송하는 함수
void sendSensorData(SensorData data) {
    // 데이터를 바이트 배열로 변환
    byte dataBytes[sizeof(SensorData)];
    int index = 0;

    // 가속도계 값 인덱싱
    memcpy(&dataBytes[index], &data.acc_x, sizeof(data.acc_x));
    index += sizeof(data.acc_x);
    memcpy(&dataBytes[index], &data.acc_y, sizeof(data.acc_y));
    index += sizeof(data.acc_y);
    memcpy(&dataBytes[index], &data.acc_z, sizeof(data.acc_z));
    index += sizeof(data.acc_z);

    // 자이로스코프 값 인덱싱
    memcpy(&dataBytes[index], &data.deg_x, sizeof(data.deg_x));
    index += sizeof(data.deg_x);
    memcpy(&dataBytes[index], &data.deg_y, sizeof(data.deg_y));
    index += sizeof(data.deg_y);
    memcpy(&dataBytes[index], &data.deg_z, sizeof(data.deg_z));
    index += sizeof(data.deg_z);

    // I2C 통신을 사용하여 바이트 데이터 전송
    // Wire.beginTransmission(); 
    // Wire.write(dataBytes, sizeof(dataBytes)); // 바이트 데이터 전송
    // Wire.endTransmission();
}

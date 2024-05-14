#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "MPU9250.h" 

MPU9250 mpu1; // 첫 번째 MPU9250 센서 객체
MPU9250 mpu2; // 두 번째 MPU9250 센서 객체
// 필터링을 위한 임계값 설정
const float accelThreshold = 0.5; // 가속도 데이터를 필터링하기 위한 임계값
const float gyroThreshold = 5.0;  // 자이로스코프 데이터를 필터링하기 위한 임계값

// 센서 값 저장을 위한 구조체 정의
struct SensorValue {
  String name;  // 센서 값의 이름 (예: "mpu1_accelX"는 첫 번째 센서의 X축 가속도를 의미)
  float value;  // 센서 값.
};
struct SensorInfo {
  String name;                 // 센서의 식별자 (예: "mpu1", "mpu2")
  SensorValue sensorValues[6]; // 가속도계 X, Y, Z와 자이로스코프 X, Y, Z 축의 값을 저장
};
struct SensorsData {
  SensorInfo sensor1; // 첫 번째 센서 정보
  SensorInfo sensor2; // 두 번째 센서 정보
};

//////////
//와이파이 연결을 위한 와이파이 ssid와 pasword입
const char* ssid = "Galaxy S24U";
const char* password = "01035307712";

const char* mqtt_server = "192.168.206.32"; // MQTT 통신을 위한 브로커 IP 
const int mqtt_port = 1883; // MQTT 브로커의 포트 번호를 설정, 일반적으로 1883
const char* mqtt_topic = "righthand/input"; //토픽 정의 

WiFiClient espClient; 
PubSubClient client(espClient); //esp모듈을 클라이언트로 지정. 
//client : mqtt에 연결된 모든 것 

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


  StaticJsonDocument<1024> doc;
  char buffer[512];
  serializeJson(doc, buffer);

  JsonObject sensor1 = doc.createNestedObject("sensor1");
  JsonObject sensor2 = doc.createNestedObject("sensor2");
  SensorsData sensors = getSensorsData(); // 센서 데이터 구조체 받기

  printSensorData(sensors.sensor1); // 첫 번째 센서 데이터 출력
  printSensorData(sensors.sensor2); // 두 번째 센서 데이터 출력
 
  addSensorDataToJson(sensor1, sensors.sensor1);
  addSensorDataToJson(sensor2, sensors.sensor2);

  // JSON 데이터를 시리얼 포트로 예쁘게 출력
  serializeJsonPretty(doc, Serial);
  Serial.println(); // 줄 바꿈 추가

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
if (client.publish(mqtt_topic, buffer)) { //토픽 발행 
    Serial.println("JSON message sent to the server");
  } else {
    Serial.println("Failed to send JSON message");
  }

  delay(5000);  // Send every 5 seconds
 


 
}
 
void addSensorDataToJson(JsonObject& json, const SensorInfo& sensorInfo) {
  for (int i = 0; i < 6; i++) {
    json[sensorInfo.sensorValues[i].name] = sensorInfo.sensorValues[i].value;
  }
}


void printSensorData(const SensorInfo& sensorInfo) {
  Serial.println(sensorInfo.name + " Data:"); // 센서 식별자 출력
  for (int i = 0; i < 6; i++) { // 각 축에 대한 데이터 출력
    Serial.print(sensorInfo.sensorValues[i].name + ": ");
    Serial.println(sensorInfo.sensorValues[i].value, 6); // 소수점 아래 6자리까지 표시
  }
}

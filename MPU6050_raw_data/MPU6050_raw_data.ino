#include<Wire.h> 
//I2C을 위한 헤더파일

void setup() {
  Serial.begin(9600);
  Wire.begin(); //I2C 마스터 설정하는 초기화 함수, Wire.begin(addr)는 슬레이브 설정 함수 
  //Power Management(PWR MGMT)
  Wire.beginTransmission(0x68); //The default buffer size is 32 bytes; writing bytes beyond 32 before calling endTransmission() will be ignored. The buffer size can be increased by using acquireWireBuffer().
  Wire.write(107);
  Wire.write(0);
  Wire.endTransmission();

  /* Register 27 - Gyro_CONFIG : x,y,z self-test + scale range(감도)
  Wire.beginTransmission(0x6B); //slave internal register addr / Wire.write() : memory queue에 쌓임 (FIFO buffer)
  Wire.write(27); //I2C의 SDA는 모든 data가 포함되며, slave internal register addr도 포함 가능 
  Wire.write(0);
  Wire.endTransmission(); //Stop : I2C는 두개의 마스터 통신이 가능하므로, 데이터를 보내고, I2C 통신을 그만(stop)할 건지, 데이터를 연속해서 보낼건지(False) 결정

  // Register 28 - ACCEL_CONFIG : self-test + full scale range selection
  Wire.beginTransmission(0x6B);
  Wire.write(28);
  Wire.write(0);
  Wire.endTransmission();
*/
}

void loop() {
  uint8_t i;
  float acc_raw[3], gyro_raw[3];

  //Gel Accel
  Wire.beginTransmission(0x68);
  Wire.write(59); // starting with register 59 = accel_xout[15:8]
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6); // request 6 register(6 byte) 
  for(i=0; i<3; i++) acc_raw[i] = (Wire.read()<<8 | Wire.read())/16384.0; //FIFO buffer에서 하나씩 빠짐

  //Get Gyro
  Wire.beginTransmission(0x68);
  Wire.write(67); // register 59 = accel_xout[15:8]
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6);
  for(i=0; i<3; i++) gyro_raw[i] = (Wire.read()<<8 | Wire.read())/131.0;

  //Calculate
  float angle[2], vec;
  vec = sqrt(pow(acc_raw[0], 2) + pow(acc_raw[2],2));
  angle[0] = atan2(acc_raw[1],vec) * RAD_TO_DEG;
  vec = sqrt(pow(acc_raw[1],2) + pow(acc_raw[2],2));
  angle[1] = atan2(acc_raw[0], vec) * RAD_TO_DEG;


  //Serial print
  char str[50], a1[10], a2[10];
  dtostrf(angle[0],4, 3, a1);
  dtostrf(angle[1], 4, 3, a2);

  sprintf(str, "X:%s Y:%s", a1, a2);
  Serial.println(str);
  delay(333);

}

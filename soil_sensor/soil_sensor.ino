// Author: Myrul Deen
#include <SoftwareSerial.h>

#define RS485Power          18
#define RS485RX             23
#define RS485TX             22

#define sensorFrameSize     19
#define sensorWaitingTime   1000
#define sensorID            0x01
#define sensorFunction      0x03
#define sensorByteResponse  0x0E

SoftwareSerial sensor(RS485RX, RS485TX);

unsigned char byteRequest[8] = {0X01, 0X03, 0X00, 0X00, 0X00, 0X07, 0X04, 0X08};
unsigned char byteResponse[19] = {};

float moisture, temperature, ph, nitrogen, phosphorus, potassium;

void setup() {
  Serial.begin(9600);
  sensor.begin(4800);
  pinMode(RS485Power, OUTPUT);
  
  digitalWrite(RS485Power, HIGH);
  delay(1000);
  Serial.println();
  Serial.println("Agriculture Kit Sensor Ready");
}

void loop() {
  sensor.write(byteRequest, 8);

  unsigned long resptime = millis();
  while ((sensor.available() < sensorFrameSize) && ((millis() - resptime) < sensorWaitingTime)) {
    delay(1);
  }
  
  while (sensor.available()) {
    for (int n = 0; n < sensorFrameSize; n++) {
      byteResponse[n] = sensor.read();
    }

    if(byteResponse[0] != sensorID && byteResponse[1] != sensorFunction && byteResponse[2] != sensorByteResponse){
      return;
    }
  }

  Serial.println();
  Serial.println("===== SOIL PARAMETERS");
  Serial.print("Byte Response: ");

  String responseString;
  for (int j = 0; j < 19; j++) {
    responseString += byteResponse[j] < 0x10 ? " 0" : " ";
    responseString += String(byteResponse[j], HEX);
    responseString.toUpperCase();
  }
  Serial.println(responseString);

//  moisture = sensorValue((int)byteResponse[3], (int)byteResponse[4]) * 0.1 ;
//  temperature = sensorValue((int)byteResponse[5], (int)byteResponse[6]) * 0.1;
//  ph = sensorValue((int)byteResponse[9], (int)byteResponse[10]) * 0.1;
//  nitrogen = sensorValue((int)byteResponse[11], (int)byteResponse[12]);
//  phosphorus = sensorValue((int)byteResponse[13], (int)byteResponse[14]);
//  potassium = sensorValue((int)byteResponse[15], (int)byteResponse[16]);

  nitrogen = sensorValue((int)byteResponse[3], (int)byteResponse[4]);
  phosphorus = sensorValue((int)byteResponse[5], (int)byteResponse[6]);
  potassium = sensorValue((int)byteResponse[7], (int)byteResponse[8]);
  
//  Serial.println("Moisture: " + (String)moisture + " %");
//  Serial.println("Temperature: " + (String)temperature + " °C");
//  Serial.println("pH: " + (String)ph);
  Serial.println("Nitrogen (N): " + (String)nitrogen + " mg/kg");
  Serial.println("Phosporus (P): " + (String)phosphorus + " mg/kg");
  Serial.println("Potassium (K): " + (String)potassium + " mg/kg");

  delay(3000);
}

int sensorValue(int x, int y) {
  int t = 0;
  t = x * 256;
  t = t + y;
  
  return t;
}

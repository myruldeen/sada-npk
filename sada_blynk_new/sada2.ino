
// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

#define BLYNK_TEMPLATE_ID "TMPL6gAo-GPg"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "BL6fc0aEQAQ2njZTzLDqzUXhuHTeJ0AN"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <SoftwareSerial.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "4GMIFI_1022";
char pass[] = "1234567890";

#define RS485Power          18
#define RS485RX             10
#define RS485TX             9

#define sensorFrameSize     19
#define sensorWaitingTime   1000
#define sensorID            0x01
#define sensorFunction      0x03
#define sensorByteResponse  0x0E


#define ONE_WIRE_BUS 2

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin D6     //GPIO-0 D3 pin of nodemcu
#define buzzer 9        //buzzer to esp8266 gpio pin 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial sensor(RS485RX, RS485TX);

unsigned char byteRequest[8] = {0X01, 0X03, 0X00, 0X00, 0X00, 0X07, 0X04, 0X08};
unsigned char byteResponse[19] = {};

float moisture, temperature, ph, nitrogen, phosphorus, potassium;

int sensorPin = A0;
int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;
int sensorValue4 = 0;
int sensorValue5 = 0;
int light = 0;
#define S0 D0
#define S1 D1
#define S2 D2
#define S3 D3
#define ON_OFF   D5  // Relay 1 Solenoid
#define SW       D7  // Relay 2 Light
//#define Relay3   D3
DHT dht(dht_dpin, DHTTYPE);

long now = millis();
long lastMeasure = 0;

void setup()
{
  // Debug console
  Serial.begin(115200);
  sensors.begin();

  sensor.begin(4800);
  pinMode(RS485Power, OUTPUT);

  digitalWrite(RS485Power, HIGH);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ON_OFF, OUTPUT);
  pinMode(SW, OUTPUT);
  pinMode(buzzer, OUTPUT);
  dht.begin();

  //  delay(10);
  delay(1000);
  Serial.println();
  Serial.println("Agriculture Kit Sensor Ready");
}

void loop()
{
  now = millis();
  if (now - lastMeasure > 10000) {
    lastMeasure = now;

    // NPK Process
    sensor.write(byteRequest, 8);

    unsigned long resptime = millis();
    while ((sensor.available() < sensorFrameSize) && ((millis() - resptime) < sensorWaitingTime)) {
      delay(1);
    }

    while (sensor.available()) {
      for (int n = 0; n < sensorFrameSize; n++) {
        byteResponse[n] = sensor.read();
      }

      if (byteResponse[0] != sensorID && byteResponse[1] != sensorFunction && byteResponse[2] != sensorByteResponse) {
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

    nitrogen = sensorValue((int)byteResponse[3], (int)byteResponse[4]);
    phosphorus = sensorValue((int)byteResponse[5], (int)byteResponse[6]);
    potassium = sensorValue((int)byteResponse[7], (int)byteResponse[8]);

    Serial.println("Nitrogen (N): " + (String)nitrogen + " mg/kg");
    Serial.println("Phosporus (P): " + (String)phosphorus + " mg/kg");
    Serial.println("Potassium (K): " + (String)potassium + " mg/kg");

    //

    sensors.requestTemperatures();
    //digitalWrite(ON_OFF, HIGH);
    float h = 0.0; //Humidity level
    float t = 0.0; //Temperature in celcius
    float f = 0.0; //Temperature in fahrenheit
    float m = 0.0; // moisture from soil sensor
    // Match the request
    int i = 0;
    int moisture_percentage = 0;

    int m1 = 0;
    int m2 = 0;

    h = dht.readHumidity();    //Read humidity level
    t = dht.readTemperature(); //Read temperature in celcius
    // f = (h * 1.8) + 32;        //Temperature converted to Fahrenheit

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    // Serial.print("Sensor 1 ");
    // Serial.println(analogRead(sensorPin));
    sensorValue1 = ( 100 - ( (analogRead(sensorPin) / 1024) * 100 ) ); delay(1);
    m1 = analogRead(sensorPin);
    //  sensorValue1 = (analogRead(sensorPin)); delay(1);
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    //  Serial.print("Sensor 2 ");
    // Serial.println(analogRead(sensorPin));
    sensorValue2 = ( 100 - ( (analogRead(sensorPin) / 1024) * 100 ) ); delay(1);
    m2 = analogRead(sensorPin);
    //  sensorValue2 = (analogRead(sensorPin)); delay(1);
    digitalWrite(S0, LOW);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    // Serial.print("Sensor 3 ");
    // Serial.println(analogRead(sensorPin));
    //   sensorValue3 = ( 100 - ( (analogRead(sensorPin)/1024) * 100 ) );
    sensorValue3 = (analogRead(sensorPin)); delay(1);
    light = map(sensorValue3, 0, 1023, 10, 0);


    if (m1 > 700)
    {
      digitalWrite(SW, LOW);
    }
    else
    {
      digitalWrite(SW, HIGH);
      tone(buzzer, 1000);
    }

    delay(10);
    // int j;
    if (m2 > 700)
    {
      digitalWrite(ON_OFF, LOW);
      // j=1;digitalWrite(ON_OFF, HIGH);
    } else {
      digitalWrite(ON_OFF, HIGH);
      tone(buzzer, 1000);
      //   j=0;digitalWrite(ON_OFF, LOW);
    }
    Blynk.run();
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!
    Blynk.virtualWrite(V6, map(m1, 0, 1023, 100, 0)); // soil moisture 1
    Blynk.virtualWrite(V7, map(m2, 0, 1023, 100, 0)); // soil moisture 2
    Blynk.virtualWrite(V9, sensors.getTempCByIndex(0));  // soil temp 1
    Blynk.virtualWrite(V5, sensors.getTempCByIndex(1));  // soil temp 2
    Blynk.virtualWrite(V8, light);  // light
    Blynk.virtualWrite(V10, t); // temperature
    Blynk.virtualWrite(V11, h); // humidity

    //    NPK Part add blynk virtual pin
    Blynk.virtualWrite(V12, nitrogen);  // Nitrogen (N)
    Blynk.virtualWrite(V13, phosphorus); // Phosphorus (P)
    Blynk.virtualWrite(V14, potassium); // Potassium (K)
  }


}

int sensorValue(int x, int y) {
  int t = 0;
  t = x * 256;
  t = t + y;

  return t;
}

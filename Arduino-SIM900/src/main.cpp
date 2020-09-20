#include <Arduino.h>
#include <gsm.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <TimerOne.h>
#define powerPin    9
#define gaugePin    3
#define echoPin     4
#define trigPin     5

volatile double tip, debit, banjir;
const float pertip = 1.346;
SoftwareSerial data(7, 8); //RX,TX
GSM gsm;

int measureRange()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distance = duration / 58.2;
  return distance;
}

void countTip()
{
  tip += pertip;
  Serial.print("curah hujan :");
  Serial.println(tip);
}
void calculateReading()
{
  debit = tip / 10;
  tip = 0;
}
void setup()
{
  Serial.begin(9600);
  data.begin(9600);
  gsm.log(&Serial);
  gsm.init(&data, powerPin);
  gsm.initGPRS("3gprs"); //APN
  // Timer1.initialize(10000000);
  // Timer1.attachInterrupt(calculateReading);
  pinMode(2, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(gaugePin),countTip,FALLING);
}

void loop()
{
  String myData;
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(3));
  // doc["tempat"] = "tembalang";
  // doc["hujan"] = debit;
  // doc["banjir"] = measureRange();
  doc["hujan"] = 5;
  doc["debit"] = rand() % 10;
  doc["tinggi"] = rand() % 10;
  serializeJson(doc, myData);
  // gsm.post("https://us-central1-bisa-b2497.cloudfunctions.net/api/sensor?type=hujan", myData, true);
  Serial.println(gsm.get("http://worldtimeapi.org/api/ip",false));
  delay(5000);
}
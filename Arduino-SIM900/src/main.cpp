#include "main.h"

float tinggiAir()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distance = duration / 5.82;
  data.tinggi = abs(setting.tinggipipa - distance) / 1000.00;
  return data.tinggi;
}

void tipHujan()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > setting.debounce)
  {
    data.tip += setting.pertip;
    Serial.println("tip : " + (String)data.tip);
  }
  last_interrupt_time = interrupt_time;
}

void calculateReading()
{
  if (setting.count == setting.samplingtime)
  {
    data.debit = data.tinggi * (setting.luas / (setting.samplingtime / 3600)) * 0.278;
    data.lastTip = data.tip;
    data.intensitas = (data.lastTip / 24.00) * (pow((24.000 / (setting.samplingtime / 3600)), 0.67));
    data.tip = 0.00;
    setting.count = 0;
    Serial.println("data hujan terakhir :" + (String)data.lastTip);
    Serial.println("intensitas :" + (String)data.intensitas);
  }
  setting.count++;
}
void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  sim900.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  gsm.log(&Serial);
  gsm.init(&sim900, powerPin);
  gsm.initGPRS("3gprs"); //APN
  Timer1.initialize(setting.second);
  Timer1.attachInterrupt(calculateReading);
  pinMode(gaugePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(gaugePin), tipHujan, FALLING);
}

void loop()
{
  String myData;
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(3));
  // doc["hujan"] = data.lastTip;
  // doc["debit"] = data.debit;
  // doc["tinggi"] = tinggiAir();
  doc["hujan"] = 2;
  doc["debit"] = 5;
  doc["tinggi"] = 6;
  serializeJson(doc, myData);
  serializeJsonPretty(doc, Serial);
  // if (data.debit > 0)
    gsm.post("https://us-central1-bisa-b2497.cloudfunctions.net/api/sensor", myData, true);
  delay(20000);
}
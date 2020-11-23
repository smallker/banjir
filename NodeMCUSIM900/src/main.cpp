#include "main.h"
float tinggiAir()
{
  #if defined(VL)
  int distance = sensor.readRangeContinuousMillimeters();
  #else
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distance = duration / 5.82;
  #endif
  data.tinggi = abs(setting.tinggipipa - distance) / 1000.00;
  return data.tinggi;
}

void ICACHE_RAM_ATTR tipHujan()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > setting.debounce)
  {
    data.tip += setting.pertip;
    // Serial.println("tip : " + (String)data.tip);
  }
  last_interrupt_time = interrupt_time;
}

void ICACHE_RAM_ATTR calculateReading()
{
  if (setting.count == setting.samplingtime)
  {
    data.debit = data.tinggi * (setting.luas / 0.017) * 0.278;
    data.lastTip = data.tip;
    data.intensitas = (data.lastTip / 24.00) * (pow((24.000 / 0.017), 0.67));
    // Serial.println("tip : "+(String)data.lastTip+" intensitas : "+(String)data.intensitas+" debit : "+(String)data.debit);
    data.tip = 0.00;
    setting.count = 0;
  }
  setting.count++;
}

void setup()
{
  Serial.begin(9600);
  pinMode(RAIN_GAUGE, INPUT_PULLUP);
  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous(100);
  attachInterrupt(RAIN_GAUGE, tipHujan, FALLING);
  task.attach_ms(1000, calculateReading);
  sim.begin(9600);
  gsm.log(&Serial);
  gsm.init(&sim, PWR_PIN);
  gsm.initGPRS("3gprs");
}

void loop()
{
  /*
    firebase url = "https://us-central1-bisa-b2497.cloudfunctions.net/api/sensor"
    test url = "http://47.241.6.200:3000/tes"
  */
  String url = "https://us-central1-bisa-b2497.cloudfunctions.net/api/sensor";
  tinggiAir();
  String buffer;
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(4));
  doc["hujan"] = data.lastTip;
  doc["debit"] = data.debit;
  doc["tinggi"] = data.tinggi;
  doc["intensitas"] = data.intensitas;
  serializeJson(doc, buffer);
  gsm.post(url, buffer, true);
  delay(40000);
}

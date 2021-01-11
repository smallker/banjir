#include "main.h"
#include <Regexp.h>

float tinggiAir()
{
  float distance = sensor.readRangeContinuousMillimeters();
  if(distance < 2000) data.tinggi = (abs(setting.tinggipipa - distance) / 1000.00);
  // if(data.tinggi > 2) ESP.restart();
  return data.tinggi;
}
void hw_wdt_disable()
{
  *((volatile uint32_t *)0x60000900) &= ~(1); // Hardware WDT OFF
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
  ESP.wdtDisable();
  hw_wdt_disable();
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RAIN_GAUGE, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.begin(D2, D1);
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    delay(200);
    Serial.println("Failed to detect and initialize sensor!");
    ESP.restart();
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
  // ESP.wdtFeed();
  digitalWrite(LED_BUILTIN, LOW);
  String url = "https://us-central1-bisa-b2497.cloudfunctions.net/api/sensor";
  // Serial.println("tinggi : "+(String)tinggiAir());
  tinggiAir();
  String buffer;
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(4));
  doc["hujan"] = data.lastTip;
  doc["debit"] = data.debit;
  doc["tinggi"] = data.tinggi;
  doc["intensitas"] = data.intensitas;
  serializeJson(doc, buffer);
  Serial.println(buffer);
  if(data.debit >0){
    gsm.post(url, buffer, true);
  }
  int count = 0;
  while (true)
  {
    if (count >= 40)
      break;
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(1000);
    count++;
  }
}

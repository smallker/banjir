#include <Arduino.h>
#include <gsm.h>
#include <SoftwareSerial.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#define VL

#if defined(VL)
#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;
#else
#define echoPin     D1
#define trigPin     D2
#endif

#define SIM_RX      D7
#define SIM_TX      D8
#define PWR_PIN     D5
#define RAIN_GAUGE  D6
GSM gsm;

SoftwareSerial sim(SIM_RX, SIM_TX);
Ticker task;
struct Setting
{
  unsigned int tinggipipa = 1000;   //dalam milimeter
  float luas = 0.551; //dalam kilometer persegi
  const float pertip = 0.053;
  int samplingtime = 60; // dalam detik
  int count, interruptTime = 10; //dalam detik
  const float second = 1000000;
  byte debounce = 200;
} setting;

struct Data
{
 volatile float tip, lastTip, debit, banjir, tinggi = 0.00, intensitas = 0.0;
 String buffer;
} data;
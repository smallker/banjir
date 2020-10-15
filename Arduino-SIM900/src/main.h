#include <Arduino.h>
#include <gsm.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <TimerOne.h>
#define powerPin    9
#define gaugePin    3
#define echoPin     4
#define trigPin     5
#define sim_rx      7
#define sim_tx      8

SoftwareSerial sim900(sim_rx, sim_tx); //RX,TX
GSM gsm;

struct Setting
{
  unsigned int tinggipipa = 1000;   //dalam milimeter
  float luas = 0.55; //dalam kilometer persegi
  const float pertip = 0.053;
  int samplingtime = 60; // dalam detik
  unsigned int count, interruptTime = 10; //dalam detik
  const float second = 1000000;
  byte debounce = 200;
} setting;

struct Data
{
 volatile float tip, lastTip, debit, banjir, tinggi = 0.00, intensitas = 0.0;
 String buffer;
} data;
// ==================================================================================================
//  1. Ambil data setting dari Firebase(tinggipipa,panjang,lebar,waktu,status123)
//              contoh data yg diambil =    110   ,   60  ,  60 ,  10 , 20,40,60
//  2. Hitung tinggi muka air (tinggipipa - jarak = tinggi muka air)
//  3. Hitung Debit ( V / t ) -> V = (panjang(dm)*lebar(dm)*tinggi(dm) / t)
//     -> Jadiin desimeter(dm) semua dulu         -> t = waktu, dari menit jadiin jam alias dikali 3600
//  4. Hitung Hujan
//      1 tip sensor = 0.053 inch of rain,
//                    Hitung total tip tiap jam, dikali * 0.053
//      didapat hasil xxx,xxx inch of rain setiap jam nya
//  5. Set Status | Bahaya=> tinggi > 40 | Siaga=> tinggi > 20 | Aman => 0-20
//  5. Kirim Data Tinggi, Debit, Hujan setiap perubahan ke realtime
//  6. Kirim Data Tinggi, Debit, Hujan setiap jam ke database utama(sensor)
//  7. Halaman Web update tiap Jam \\ Uwis
//  8. Monitoring Android tiap perubahan, tiap berubah ke bahaya ada notifikasi \\ Kurang sitik
// ===================================================================================================
#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define WIFI_SSID "y"
#define WIFI_PASSWORD "11111111"
#define FIREBASE_HOST "bisa-b2497.firebaseio.com"
#define FIREBASE_AUTH "RWiICJnjlpsVomTY9po6AYoAMYuWV4IhEgkMDSgE"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
Ticker tick;
// Define yang mau diambil
String tanggal, statusnya, ambilstatus, log_tanggal, log_jam;
double tinggi, debit, jedawaktu, hujan, tinggipipa, lebar, panjang, jarak, status1, status2, status3;
int rain;
long duration, _delay = 5;
String hujan2;
// defines pins numbers
#define trigPin D5 //D8
#define echoPin D6 //D7
#define rainPin D2 //D2
FirebaseData firebaseData;

void ICACHE_RAM_ATTR rainGaugeSensor(void);
void ICACHE_RAM_ATTR dataHujan(void);

void getSettings()
{
  DynamicJsonDocument output(1024);
  if (Firebase.getJSON(firebaseData, "/setting"))
  {
    String response = firebaseData.jsonString();
    DeserializationError error = deserializeJson(output, response);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    // _start  = output["start"];
    lebar = output["lebar"];
    panjang = output["panjang"];
    status1 = output["status1"];
    status2 = output["status2"];
    status3 = output["status3"];
    tinggipipa = output["tinggipipa"];
    _delay = output["delay"];
    _delay = _delay * 1000;
    Serial.println("Tes setting :\ntinggi pipa => " + (String)tinggipipa + "\nlebar => " + (String)lebar);
  }
}

void firebaseSetup()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
}
void setup()
{
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(rainPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rainPin), rainGaugeSensor, FALLING);
  firebaseSetup();
  getSettings();
  timeClient.begin();
  timeClient.setTimeOffset(25200); // Set Timezone indonesian WIB
  // tick.attach(1,dataHujan);
}

void loop()
{
  //--------------------------------------------------- Set Waktu timestamp --------------------------------------------
  Serial.println(" ");
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentYear = ptm->tm_year + 1900;
  int currentMonth = ptm->tm_mon + 1;
  if (currentMonth < 10)
  {
    tanggal = "0" + String(currentMonth);
  }
  else
  {
    tanggal = String(currentMonth);
  }

  //Print complete date:
  String currentDate = String(currentYear) + "-" + tanggal + "-" + String(monthDay) + " " + formattedTime;
  Serial.println(currentDate);

  //----------------------------- Hitung jarak sensor dengan Muka Air ----- ----------------------------------------------
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  //jarak = ( duration / 29 ) / 2;
  jarak = (duration / 2) / 29.1;
  //distance_in = distance_cm * 0,393701;

  //------------------------------ Hitung Tinggi -----------------------------------------------------------------------
  tinggi = tinggipipa - jarak;
  Serial.println((String) "Tinggi  ->" + tinggipipa + "-" + jarak + "=" + tinggi + "cm");

  //----------------------------- Hitung Debit--------------------------------------------------------------------------
  panjang = panjang / 10; // jadiin desimeter(dm)
  lebar = lebar / 10;
  tinggi = tinggi;
  jedawaktu = jedawaktu * 60; //dari menit jadiin jam
  debit = panjang * lebar * tinggi / 10;
  log_tanggal = String(currentYear) + "-" + tanggal + "-" + String(monthDay);
  log_jam = formattedTime;
  Serial.println((String) "Debit   ->" + panjang + "X" + lebar + "X" + tinggi + "/" + jedawaktu + "=" + debit + "dm3");
  Serial.println("jeda waktu => "+(String)jedawaktu + "Debit => "+(String)debit);
  //----------------------------- Hitung Curah Hujan -------------------------------------------------------------------
  //hujan = 20; //masih dummy ya
  Serial.println((String) "Hujan   ->" + hujan + " (Masih dummy hujannya)");
  //hujan2 = ((String)""+tinggi+"="+tinggipipa+"-"+jarak+"");
  //"" + tinggi + "=" + tinggipipa + "-" + jarak;

  //----------------------------- Tentukan Status ----------------------------------------------------------------------
  if (tinggi > status2)
  {
    statusnya = "Bahaya";
  }
  else if (tinggi > status1)
  {
    statusnya = "Siaga";
  }
  else
  {
    statusnya = "Aman";
  }
  Serial.println((String) "Status  ->" + statusnya + "");

  FirebaseJson sensor;
  // ===================
  sensor.add("tinggi", tinggi);
  sensor.add("debit", debit);
  sensor.add("hujan", rain * 0.053);
  sensor.add("status", statusnya);
  sensor.add("jamtanggal", currentDate);
  Firebase.setJSON(firebaseData,"/realtime/tembalang",sensor);
  // Log debit
  FirebaseJson log_debit;
  log_debit.add("debit", debit);
  log_debit.add("tinggi", tinggi);
  log_debit.add("tanggal", log_tanggal);
  log_debit.add("jam", log_jam);
  Firebase.pushJSON(firebaseData, "/sensor/tembalang/debit", log_debit);

  // Log hujan
  FirebaseJson log_hujan;
  log_hujan.add("nilai", rain * 0.053);
  log_hujan.add("tanggal", log_tanggal);
  log_hujan.add("jam", log_jam);
  //----------------------------- Kirim Data Semuanya Tiap Jam -------------------------------------------------------
  // pake pushInt
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  if (currentMinute == 0  && currentSecond <10)
  {
    Firebase.pushJSON(firebaseData, "/sensor/tembalang/hujan", log_hujan);
    rain = 0;
  }
  delay(10000);
}

void ICACHE_RAM_ATTR rainGaugeSensor()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    Serial.println("Hujan => " + (String)rain);
    rain++;
  }
  last_interrupt_time = interrupt_time;
}
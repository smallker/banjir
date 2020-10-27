// =============================== Alur Kerja ========================================================

//              contoh data yg diambil =    110   ,   60  ,  60 ,  10 , 20,40,60
//  2. Hitung tinggi muka air (tinggipipa - jarak = tinggi muka air)
//  3. Hitung Debit ( V / t ) -> V = (panjang(dm)*lebar(dm)*tinggi(dm) / t)
//     -> Jadiin desimeter(dm) semua dulu         -> t = waktu, dari menit jadiin jam alias dikali 3600
//  4. Hitung Hujan
//      1 tip sensor = 0.053 inch of rain atau 1.346 mm,
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
// #define WIFI_SSID "@wifi.id"
// #define WIFI_PASSWORD "siap8689"
// #define WIFI_SSID "takon bob"
// #define WIFI_PASSWORD "satusatuakusayangibu"
#define WIFI_SSID "Kuning 24"
#define WIFI_PASSWORD "WIFIBARU20"

#define FIREBASE_HOST "bisa-b2497.firebaseio.com"
#define FIREBASE_AUTH "RWiICJnjlpsVomTY9po6AYoAMYuWV4IhEgkMDSgE"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
Ticker tick;
// Define yang mau diambil
String tanggal, statusnya, ambilstatus, log_tanggal, log_jam;
float tinggi, debit, luas, debitjam, debithari, jedawaktu, jedawaktujam, hujan, intensitas, tinggipipa, lebar, panjang, jarak, status1, status2, status3;
volatile int rain;
volatile int rainjam;
volatile int rainhari;
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
    luas = output["luas"];
    tinggipipa = output["tinggipipa"];
    _delay = output["delay"];
    _delay = _delay * 1000;
    Serial.println("Tes setting :\ntinggi pipa => " + (String)tinggipipa + "\nlebar => " + (String)lebar);
    panjang = panjang / 100; // jadiin desimeter(dm)
    lebar = lebar / 100;
    jedawaktu = 10; //jedawaktu * 60; //dari menit jadiin jam
    jedawaktujam = 60;
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
  Serial.println(jarak);
  //distance_in = distance_cm * 0,393701;

  //------------------------------ Hitung Tinggi -----------------------------------------------------------------------
  tinggi = (tinggipipa - jarak) / 100;

  //----------------------------- Hitung Debit--------------------------------------------------------------------------
  char buff[5];
  sprintf(buff, "%.3f", tinggi);
  String tinggibuff = (String)buff;
  tinggi = tinggibuff.toFloat();

  Serial.println("Tinggi ===> " + (String)tinggi);
  debit = tinggi * (luas / 0.017) * 0.278;  // panjang * lebar * tinggi / 60;
  debitjam = tinggi * luas * 0.278;         // asline (luas/1)
  debithari = tinggi * (luas / 24) * 0.278; // panjang * lebar * tinggi / 60;panjang * lebar * tinggi / 3600;

  //("%.2f", 37.777779)
  //debit = ("%.2f", debit);
  log_tanggal = String(currentYear) + "-" + tanggal + "-" + String(monthDay);
  log_jam = formattedTime;

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
  int t = tinggi * 100;
  sensor.add("tinggi", t / 100.000);
  sensor.add("debit", debit);
  sensor.add("hujan", rain * 1.346);
  sensor.add("intensitas", ((rain * 1.346) / 24) * (pow((24.000 / 0.017), 0.67)));
  sensor.add("status", statusnya);
  sensor.add("jamtanggal", currentDate);
  sensor.add("jarak", jarak);
  Firebase.setJSON(firebaseData, "/realtime/tembalang", sensor);
  //----------------------------- Data Per Menit -------------------------------------------------------
  FirebaseJson log_permenit;
  log_permenit.add("debit", debit);
  log_permenit.add("tinggi", t / 100.00);
  log_permenit.add("tanggal", log_tanggal);
  log_permenit.add("hujan", rain * 1.346);
  // (data.lastTip / 24.00) * (pow((24.000 / 0.017), 0.67));
  log_permenit.add("intensitas", ((rain * 1.346) / 24) * (pow((24.000 / 0.017), 0.67)));
  log_permenit.add("jam", log_jam);
  log_permenit.add("timestamp", (String)timeClient.getEpochTime());
  //---------------------------- Data Per Jam -------------------------------------------------------------------------
  // FirebaseJson log_perjam;
  // log_perjam.add("debit", debitjam);    // Nilai debitjam yg diupload gasesuai, tpi
  // log_perjam.add("tinggi", t / 100.00); // dicek diterminal udah sesuai, gatau kenapa
  // log_perjam.add("tanggal", log_tanggal);
  // log_perjam.add("hujan", rainjam * 1.346);
  // log_perjam.add("intensitas", ((rainjam * 1.346) / 24) * (pow((24.000 / 1), 0.67)));
  // log_perjam.add("jam", log_jam);

  //---------------------------- Data Per harian -----------------------------------------------------------------------
  // FirebaseJson log_perhari;
  // log_perjam.add("debit", debithari);
  // log_perjam.add("tinggi", t / 100.00);
  // log_perjam.add("tanggal", log_tanggal);
  // log_perjam.add("hujan", rainhari * 1.346);
  // log_perhari.add("intensitas", ((rainhari * 1.346) / 24) * (pow((24.000 / 24), 0.67)));
  // log_perjam.add("jam", log_jam);

  int currentSecond = timeClient.getSeconds();
  // int currentMinute = timeClient.getMinutes();
  // int currentHours = timeClient.getHours();
  //----------------------------- Kirim Data Per Menit -------------------------------------------------------
  // pake pushInt
  if (currentSecond < 10)
  {
    Serial.println("mengirim data per menit");
    Firebase.pushJSON(firebaseData, "/sensor/tembalang/menit", log_permenit);
    rain = 0;
  }
  // //----------------------------- Kirim Data Per Jam -------------------------------------------------------
  // if (currentMinute == 0 && currentSecond < 10)
  // {
  //   Serial.println("mengirim data per jam");
  //   Firebase.pushJSON(firebaseData, "/sensor/tembalang/jam", log_perjam);
  //   rainjam = 0;
  // }
  // //----------------------------- Kirim Data Per Harian -----------------------------------------------------
  // if (currentMinute == 0 && currentSecond < 10 && currentHours == 0)
  // {
  //   Serial.println("mengirim data per Harian");
  //   Firebase.pushJSON(firebaseData, "/sensor/tembalang/harian", log_perhari);
  //   rainhari = 0;
  // }
  Serial.println((String) "Tinggi  ->" + tinggipipa + "-" + jarak + "=" + tinggi + "m");
  Serial.println((String) "Debitjam = " + debitjam);

  Serial.println((String) "Debit   ->" + tinggi + " x (" + luas + "/0.017) x 0.278" + " = " + debit + "m3/s");
  Serial.println("jeda waktu => " + (String)jedawaktu + " Debit => " + (String)debit);
  // //----------------------------- Hitung Curah Hujan -------------------------------------------------------------------
  // //hujan = 20; //masih dummy ya
  Serial.println((String) "Hujan   ->" + hujan + " (Masih dummy hujannya)");
  // //hujan2 = ((String)""+tinggi+"="+tinggipipa+"-"+jarak+"");
  // //"" + tinggi + "=" + tinggipipa + "-" + jarak;
  delay(10000);
}

void ICACHE_RAM_ATTR rainGaugeSensor()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    Serial.println("Hujan Per Menit => " + (String)rain);
    Serial.println("Hujan Per Jam => " + (String)rainjam);
    Serial.println("Hujan Per Hari => " + (String)rainhari);

    //(data.lastTip / 24.00) * (pow((24.000 / 0.017), 0.67));
    rain++;
    rainjam++;
    rainhari++;
  }
  last_interrupt_time = interrupt_time;
}
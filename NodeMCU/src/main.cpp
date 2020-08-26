#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#define WIFI_SSID                 "MSP"
#define WIFI_PASSWORD             "desmosedici"
#define FIREBASE_HOST             "bisa-b2497.firebaseio.com"
#define FIREBASE_AUTH             "RWiICJnjlpsVomTY9po6AYoAMYuWV4IhEgkMDSgE"
#define FIREBASE_FCM_SERVER_KEY   "AAAAptP-Wxw:APA91bHMxI0A7eRx5t-odsfuyaEsh_PK7LZOfU0Kxhpf-s0Zl_kY4FlKnZ4p6Z10W8clyV9ONb_TavF062XJKak-5onri1wXhqb1tSbOov45TvoqTaGmLe91TRuXnRoGFUPWKU4Fyy9V"
#define rainGauge                 D5
#define trigPin                   D2
#define echoPin                   D3

FirebaseData firebaseData;

int ultrasonic(byte trigPin, byte echoPin);
void setup()
{
  Serial.begin(9600);
  pinMode(rainGauge,INPUT_PULLUP);
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
void loop()
{
    FirebaseJson sensor;
    int banjir = ultrasonic(trigPin,echoPin);
    sensor.add("sensor", banjir);
    Firebase.pushJSON(firebaseData, "/realtime/logs/", sensor);
    delay(5000);
}

int ultrasonic(byte trigPin,byte echoPin){
  long duration;
  int distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
  Serial.println("Jarak => "+(String)distance);
  return distance;
}
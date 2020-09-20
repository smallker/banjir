#include "gsm.h"
#include "Arduino.h"

/// Initialize SIM900 with manual startup button
void GSM::init(Stream *swSerial)
{
    send = swSerial;
}

/// Initialize SIM900 with software startup
void GSM::init(Stream *swSerial, byte powerPin)
{
    send = swSerial;
    _powerPin = powerPin;
    togglePower();
}

void GSM::togglePower(){
    response -> println("Powering up for 23 sec. .");
    pinMode(_powerPin, OUTPUT); 
    digitalWrite(_powerPin,LOW);
    delay(1000);
    digitalWrite(_powerPin,HIGH);
    delay(2000);
    digitalWrite(_powerPin,LOW);
    delay(20000);
}

/// Configure serial interface for debugging
void GSM::log(Stream *hwSerial)
{
    response = hwSerial;
}

/// Initialize APN setup for SIM900
void GSM::initGPRS(String apn)
{
    char buffer[200];
    response->println("Initializing . .");
    while(send->available()==false)
    {
        send->println("AT");
        if(send->available()!=0)break;
        response->println("Check your wiring");
        delay(3000);
    }
    cmd("AT",1000);
    cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"",1000);
    sprintf(buffer, "AT+SAPBR=3,1,\"APN\",\"%s\"", apn.c_str());
    cmd(buffer,6000);
    cmd("AT+SAPBR=1,1",1000);
    for(int i=0;i<3;i++){
        cmd("AT+SAPBR=2,1",1000);
    }
}

/// Set to true for https connection
void GSM::post(String url, String data, bool isHttps)
{
    char buffer[200];
    isHttps == true ? cmd("AT+HTTPSSL=1",1000) : cmd("AT+HTTPSSL=0",1000);
    cmd("AT+HTTPINIT",1000);
    cmd("AT+HTTPPARA=\"CID\",1",1000);
    sprintf(buffer, "AT+HTTPPARA=\"URL\",\"%s\"", url.c_str());
    cmd(buffer,1000);
    cmd("AT+HTTPPARA=\"CONTENT\",\"application/json\"",1000);
    cmd("AT+HTTPDATA=" + String(data.length()) + ",100000",3000);
    cmd(data,6000);
    cmd("AT+HTTPACTION=1",6000);
    cmd("AT+HTTPREAD",6000);
    cmd("AT+HTTPTERM",10000);
}

/// Set to true for https connection
String GSM::get(String url, bool isHttps)
{
    char buffer[200];
    String data;
    isHttps == true ? cmd("AT+HTTPSSL=1",1000) : cmd("AT+HTTPSSL=0",1000);
    cmd("AT+HTTPINIT",1000);
    cmd("AT+HTTPPARA=\"CID\",1",1000);
    sprintf(buffer,"AT+HTTPPARA=\"URL\",\"%s\"",url.c_str());
    cmd(buffer,1000);
    cmd("AT+HTTPACTION=0",6000);
    delay(5000);
    data = cmd("AT+HTTPREAD",10000);
    cmd("AT+HTTPTERM",10000);
    return data;
}
String GSM::cmd(String command, const long retryInterval)
{
    unsigned long previousMillis = 0;
    String data;
    for(;;)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= retryInterval) {
            previousMillis = currentMillis;
            response->print("Sending command : ");
            response->println(command);
            send->println(command);
        }
        if(send->available()>0){
            // response->println(send->readString());
            data = send -> readString();
            response->println(data);
            break;
        }
    }
    return data;
}

void GSM::sleep()
{
    response->println("AT+SAPBR=0,1");
    send->println("AT+SAPBR=0,1");
    readSerial();
}

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
    this->powerPin = powerPin;
    togglePower();
}

void GSM::togglePower()
{
    response->println("Power ON");
    pinMode(powerPin, OUTPUT);
    digitalWrite(powerPin, LOW);
    delay(1000);
    digitalWrite(powerPin, HIGH);
    delay(2000);
    digitalWrite(powerPin, LOW);
    delay(1000);
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
    while (send->available() == false)
    {
        send->println("AT");
        if (send->available() != 0)
            break;
        response->println("Check your wiring");
        delay(3000);
    }
    char buff[20];
    cmd("AT", 1000);
    while (true)
    {
        cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", 1000);
        sprintf(buffer, "AT+SAPBR=3,1,\"APN\",\"%s\"", apn.c_str());
        cmd(buffer, 1000);
        cmd("AT+SAPBR=1,1", 1000);
        if (cmd("AT+SAPBR=2,1", 1000).substring(15, 22) != "0.0.0.0")
            {
                response->println("Connected to Network");
                break;
            }
    }
    
}

/// Set to true for https connection
void GSM::post(String url, String data, bool isHttps)
{
    char buffer[200];
    isHttps == true ? cmd("AT+HTTPSSL=1", 1000) : cmd("AT+HTTPSSL=0", 1000);
    cmd("AT+HTTPINIT", 1000);
    cmd("AT+HTTPPARA=\"CID\",1", 1000);
    sprintf(buffer, "AT+HTTPPARA=\"URL\",\"%s\"", url.c_str());
    cmd(buffer, 1000);
    cmd("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 1000);
    cmd("AT+HTTPDATA=" + String(data.length()) + ",100000", 5000);
    cmd(data, 6000);
    cmd("AT+HTTPACTION=1", 6000);
    cmd("AT+HTTPREAD", 6000);
    cmd("AT+HTTPTERM", 10000);
}

String GSM::cmd(String command, const long retryInterval)
{
    unsigned long previousMillis = 0;
    String data;
    for (;;)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= retryInterval)
        {
            previousMillis = currentMillis;
            response->print("Sending command : ");
            response->println(command);
            send->println(command);
        }
        if (send->available() > 0)
        {
            // response->println(send->readString());
            data = send->readString();
            response->println(data);
            break;
        }
        delay(50);
    }
    return data;
}
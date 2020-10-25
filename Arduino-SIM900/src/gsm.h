#ifndef gsm_h
#define gsm_h
#include "Arduino.h"
class GSM
{
    public:
        void init(Stream *swSerial);
        void init(Stream *swSerial, byte powerPin);
        void log(Stream *hwSerial);
        void initGPRS(String apn);
        void post(String url,String data, bool isHttps);
        
    private:
        void readSerial();
        String cmd(String command,const long retryInterval);
        void togglePower();
        Stream *response;
        Stream *send;
        byte powerPin;
};
#endif
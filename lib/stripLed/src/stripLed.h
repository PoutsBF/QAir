/******************************************************************************
Librairie pour la gestion ddes néoleds
                                                                  02/12/2022

******************************************************************************/

#include<Arduino.h>

#include <Adafruit_NeoPixel.h>

class StripLed
{
private:
    Adafruit_NeoPixel strip;

    uint8_t device_ok;
    unsigned long delta;
    uint32_t couleurStrip;
    uint8_t nbLedStrip;

    uint32_t Wheel(byte WheelPos);

    TaskHandle_t id_tache_ntc;
    static void tacheMAJ(void *pvParameters);

public:

    StripLed(/* args */);
    ~StripLed();

    void init(uint8_t pin);
    void afficheStrip(uint16_t eCO2);
};

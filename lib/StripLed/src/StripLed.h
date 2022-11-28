/******************************************************************************
Librairie pour la gestion ddes néoleds

******************************************************************************/

#include<Arduino.h>

#include <Adafruit_NeoPixel.h>

class StripLed
{
private:
    Adafruit_NeoPixel strip;
    uint8_t device_ok;
    uint32_t Wheel(byte WheelPos);

public:
    StripLed(/* args */);
    ~StripLed();

    void init(void);
    void afficheStrip(uint16_t eCO2);
};

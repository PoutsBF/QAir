

#include <Arduino.h>
#include <RTClib.h>

#include <extEEPROM.h>        //https://github.com/PaoloP74/extEEPROM

struct sdata_bddGestion
{
    const uint8_t debutTrame = 0x00;

    ulong epochtime;
    char iso8601[19];

    float temperature;
    float humidite;
    uint16_t pression;
    uint32_t hygroAbsolue;
    uint16_t eCO2;
    uint16_t TVOC;
    const uint8_t finTrame = 0xFF;
};

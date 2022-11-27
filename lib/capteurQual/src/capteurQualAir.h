/******************************************************************************
Librairie pour la gestion du capteur SGP30

******************************************************************************/

#include <Arduino.h>

#include <Adafruit_SGP30.h>


//---------------------------------------------------------
// Fonctions et variables pour la gestion du capteur SGP30
//
struct sdata_env_qualite
{
    uint16_t eCO2;
    uint16_t TVOC;
};

class CapteurQualAir
{
private:
    Adafruit_SGP30 sgp;
    uint8_t device_OK;
    void moyenneCO2(void);

public:
    CapteurQualAir();
    ~CapteurQualAir();

    uint8_t init();
    uint8_t lecture();
};

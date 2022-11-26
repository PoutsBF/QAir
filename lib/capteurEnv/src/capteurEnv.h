/******************************************************************************
Librairie pour la gestion du capteur BME280

******************************************************************************/

#include <Arduino.h>

#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

//---------------------------------------------------------
// Modèle de donnée pour la gestion du capteur BME280
//
struct sdata_env
{
    float temperature;
    float humidite;
    float pression;
    uint32_t hygroAbsolue;
};


class CapteurEnv
{
private:
    Adafruit_BME280 bme;        // I2C
    uint8_t device_OK;          // Etat du capteur
    unsigned long delayTime;
    uint32_t absoluteHumidity;

    // calcul de l'humidité absolue
    void calcAbsoluteHumidity(float temperature, float humidity);

    /* data */
public:
    CapteurEnv(/* args */);
    ~CapteurEnv();

    uint8_t capteur_OK();
    void init(uint16_t arg_delay_time);
    uint8_t lecture(sdata_env *);
    uint32_t getAbsoluteHumidity();
};
#pragma once 
/******************************************************************************
Librairie pour la gestion du capteur BME280

******************************************************************************/

#include <Arduino.h>

#include <Adafruit_BME280.h>

#include <variableShared.h>

#define SEALEVELPRESSURE_HPA (1013.25)

//---------------------------------------------------------
// Modèle de donnée pour la gestion du capteur BME280
//
struct sdata_env
{
    float temperature;
    float humidite;
    uint16_t pression;
    uint32_t hygroAbsolue;
};


class CapteurEnv
{
private:
    Adafruit_BME280 bme;        // I2C
    uint8_t device_OK;          // Etat du capteur

    unsigned long delayTime;            // délai entre les mesures
    uint8_t chgt;

    // calcul de l'humidité absolue
    static uint32_t calcAbsoluteHumidity(float temperature, float humidity);

    VariableShared<float> * _temperature;        // Heure au format 1970 + nb secondes
    VariableShared<float> * _humidite;        // Heure au format 1970 + nb secondes
    VariableShared<uint16_t> * _pression;        // Heure au format 1970 + nb secondes
    VariableShared<uint32_t> * _hygroAbsolue;        // Heure au format 1970 + nb secondes

    TaskHandle_t id_tache;                       // Handle de la tache de maj du ntc
    static void tacheMAJ(void *pvParameters);        // tache de maj du ntc

public:
    CapteurEnv();
    ~CapteurEnv();

    uint8_t capteur_OK();
    void init(uint16_t arg_delay_time);
    uint8_t lecture();
    void get(sdata_env * t_data_env);
    void set(sdata_env t_data_env);

    uint32_t getAbsoluteHumidity();
};
#pragma once 
/******************************************************************************
Librairie pour la gestion du capteur SGP30

******************************************************************************/

#include <Arduino.h>

#include <Adafruit_SGP30.h>

#include <variableShared.h>

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
    Adafruit_SGP30 sgp;             // Objet pour la gestion du capteur SGP30
    uint8_t _device_OK;

    VariableShared <uint16_t> * _eCO2;
    VariableShared <uint16_t> * _TVOC;

    void moyenneCO2(void);

    unsigned long _delayTime;        // délai entre les mesures
    uint8_t _chgt;                   // Indicateur d'une mise à jour

    TaskHandle_t id_tache;                       // Handle de la tache de maj du ntc
    static void tacheMAJ(void *pvParameters);        // tache de maj du ntc

public:
    CapteurQualAir() {}
    ~CapteurQualAir() {}

    void init(ulong _delayTime);
    uint8_t lecture();
    void get(sdata_env_qualite *data_env_qualite);

    void setHumidity(uint32_t absolute_humidity);
};

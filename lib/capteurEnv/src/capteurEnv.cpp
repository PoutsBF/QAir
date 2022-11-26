/******************************************************************************
Librairie pour la gestion du capteur BME280

******************************************************************************/

#ifdef DEBUG_SERIAL
#endif

#include <Arduino.h>
#include <capteurEnv.h>
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_BME280.h>

CapteurEnv::CapteurEnv(/* args */)
{
    device_OK = 0;
}

CapteurEnv::~CapteurEnv()
{
}

/// @brief 
/// Initialise le capteur d'environnement (température, hygrométrie, pression)
/// @param arg_delay_time 
/// Intervale de mesure
void CapteurEnv::init(uint16_t arg_delay_time)
{
    // suggested rate is 1/60Hz (1m)
    delayTime = arg_delay_time;        // in milliseconds

    uint8_t nbTest = 10;            // 10 essais

    // Par défaut, l'état du capteur est "défaut" dans le constructeur

    //-------------------------------------------------------------------------
    // Démarrage du BME280
    do
    {
    #ifdef DEBUG_SERIAL
        Serial.println("Essai de connexion du BME280");
    #endif
        // Délai avant d'agir
        delay(200);
        // Teste la connexion avec le BME280
        device_OK = bme.begin(0x76, &Wire);
    } 
    while (!device_OK && nbTest--);

    if (!device_OK)
    {
#ifdef DEBUG_SERIAL
        Serial.println("Capteur BME280 non trouvé, vérifier le câblage !");
#endif
        return;
    }

#ifdef DEBUG_SERIAL
    Serial.println("-- Configuration du BME280 --");
    Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling, filter off");
#endif
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,        // temperature
                    Adafruit_BME280::SAMPLING_X1,        // pressure
                    Adafruit_BME280::SAMPLING_X1,        // humidity
                    Adafruit_BME280::FILTER_OFF);

    return;
}

void CapteurEnv::calcAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float cAbsoluteHumidity =                                  // [g/m^3]
        216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / 
        (243.12f + temperature)) / (273.15f + temperature));        
    absoluteHumidity  =                                             // [mg/m^3]
        static_cast<uint32_t>(1000.0f * cAbsoluteHumidity);
}

//////////////////////////////////////////

uint32_t getAbsoluteHumidity()
{
}

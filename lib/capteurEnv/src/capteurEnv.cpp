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


CapteurEnv::CapteurEnv() {}
CapteurEnv::~CapteurEnv() {}

/// @brief 
/// Initialise le capteur d'environnement (température, hygrométrie, pression)
/// @param arg_delay_time 
/// Intervale de mesure
void CapteurEnv::init(uint16_t arg_delay_time)
{
    uint8_t nbTest = 10;        // 10 essais

    // suggested rate is 1/60Hz (1m)
    delayTime = arg_delay_time;        // in milliseconds

    device_OK = 0;
    chgt = false;

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

    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,        // temperature
                    Adafruit_BME280::SAMPLING_X1,        // pressure
                    Adafruit_BME280::SAMPLING_X1,        // humidity
                    Adafruit_BME280::FILTER_OFF);

    _temperature = new VariableShared<float>(0.0);
    _humidite = new VariableShared<float>(0.0);
    _pression = new VariableShared<uint16_t>(0.0);
    _hygroAbsolue = new VariableShared<uint32_t>(0);

    // Démarrer la tâche de mise à jour
    xTaskCreatePinnedToCore(
        (TaskFunction_t)tacheMAJ,        // Task function.
        "temp Task",                     // name of task.
        2048,                            // Stack size of task
        this,                            // parameter of the task
        2,                               // priority of the task
        &id_tache,                       // Task handle to keep track of created task
        0);                              // sur CPU1
}

/// @brief tache de lecture régulière des données d'environnement 
/// @param pvParameters 
void CapteurEnv::tacheMAJ(void *pvParameters)
{
    // Pointeur sur la tache appelante
    CapteurEnv *moi = (CapteurEnv *)pvParameters;

    // Délai en millisecondes
    TickType_t xDelay = moi->delayTime / portTICK_PERIOD_MS;

    sdata_env t_data_env;

    // Boucle infinie
    while (true)
    {
        // Only needed in forced mode! In normal mode, you can remove the next line.
        moi->bme.takeForcedMeasurement();        // has no effect in normal mode
        
        // Lit les données
        moi->_temperature->set(moi->bme.readTemperature());
        moi->_humidite->set(moi->bme.readHumidity());
        moi->_pression->set(moi->bme.readPressure() / 100.0F);

        // Calcul l'hygrométrie absolue et l'enregistre
        moi->_hygroAbsolue->set(
            calcAbsoluteHumidity(
                moi->_temperature->get(),
                moi->_humidite->get()));

        moi->chgt = true;
        // Pause avant relance
        vTaskDelay(xDelay);
    }
}

/// @brief Calcule le niveau d'humidité absolu en mg/m3
/// @param temperature 
/// @param humidity 
uint32_t CapteurEnv::calcAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    // [g/m^3]
    const float cAbsoluteHumidity =                                  
        216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / 
        (243.12f + temperature)) / (273.15f + temperature));
    // [mg/m^3]
    return static_cast<uint32_t>(1000.0f * cAbsoluteHumidity);
}

/// @brief Procédure appelée régulièrement (voir si possible avec un timer ?)
/// @param data_env 
/// @return 
uint8_t CapteurEnv::lecture()
{    
    if (chgt)
    {
        chgt = false;
        return true;
    }
    return false;
}

void CapteurEnv::get(sdata_env * t_data_env)
{
    t_data_env->temperature = _temperature->get();
    t_data_env->humidite = _humidite->get();
    t_data_env->hygroAbsolue = _hygroAbsolue->get();
    t_data_env->pression = _pression->get();    
}

uint8_t CapteurEnv::capteur_OK()
{
    return device_OK;
}

uint32_t CapteurEnv::getAbsoluteHumidity()
{
    return _hygroAbsolue->get();
}



    //////////////////////////////////////////

/******************************************************************************
    Librairie pour la gestion du capteur SGP30

******************************************************************************/

#ifdef DEBUG_SERIAL
#endif

#define PROFONDEUR_HISTO 30

#include <Arduino.h>
#include <capteurQualAir.h>
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_BME280.h>

/******************************************************************************
    Méthodes
******************************************************************************/

/// @brief 
/// @param _delayTime 
void CapteurQualAir::init(ulong delayTime)
{    
    uint8_t nbTest = 10;            // 10 essais

    _delayTime = delayTime;     // Sauvegarde le délai sélectionné
    _device_OK = 0;              // état de la connexion au SGP30, état inversé

    //-------------------------------------------------------------------------
    // Démarrage du SGP30
    do
    {
#ifdef DEBUG_SERIAL
        Serial.println("Essai de connexion du SGP30");
#endif
        // Délai avant d'agir
        delay(200);
        // Teste la connexion avec le BME280
        _device_OK = sgp.begin();
    } while (!_device_OK && nbTest--);

    if (!_device_OK)
    {
#ifdef DEBUG_SERIAL
        Serial.println("Could not find a valid SGP30 sensor, check wiring!");
#endif
        return;
    }

#ifdef DEBUG_SERIAL
    Serial.print("Found SGP30 serial #");
    Serial.print(sgp.serialnumber[0], HEX);
    Serial.print(sgp.serialnumber[1], HEX);
    Serial.println(sgp.serialnumber[2], HEX);
#endif

    _eCO2 = new VariableShared<uint16_t>(0);
    _TVOC = new VariableShared<uint16_t>(0);

    _chgt = 0;

    // Démarrer la tâche de mise à jour
    xTaskCreatePinnedToCore(
        (TaskFunction_t)tacheMAJ,        // Task function.
        "qual Task",                     // name of task.
        2048,                            // Stack size of task
        this,                            // parameter of the task
        2,                               // priority of the task
        &id_tache,                       // Task handle to keep track of created task
        0);                              // sur CPU1
}

/// @brief Met à jour l'affichage des leds néopixels.
/// Tache infinie avec délai
void CapteurQualAir::tacheMAJ(void *pvParameters)
{
    //-------- calcul filtre K = 0,5
    uint16_t fn_1 = 0;

    // Pointeur sur la tache appelante
    CapteurQualAir *moi = (CapteurQualAir *)pvParameters;

    // Délai en millisecondes
    TickType_t xDelay = moi->_delayTime / portTICK_PERIOD_MS;

    // Boucle infinie
    while (true)
    {
        if (!moi->sgp.IAQmeasure())
        {
            Serial.println("Measurement failed");
        }
        else
        {
            uint16_t vtemp;

            // K = 0,5 donc décalage de 1 pour division par 2
            vtemp = (moi->sgp.eCO2 + fn_1) >> 1;

            moi->_eCO2->set(vtemp);
            fn_1 = vtemp;
            moi->_TVOC->set(moi->sgp.TVOC);

            if (!moi->sgp.IAQmeasureRaw())
            {
#ifdef DEBUG_SERIAL
                Serial.println("Raw Measurement failed");
#endif
            }
            else
            {
                moi->moyenneCO2();
            }
            // Serial.println();
        }

        moi->_chgt = true;
        // Pause avant relance
        vTaskDelay(xDelay);
    }
}

/// @brief 
/// @param data_env_qualite 
/// @return 
uint8_t CapteurQualAir::lecture()
{
    if (_chgt)
    {
        _chgt = false;
        return true;
    }
    return false;
}

void CapteurQualAir::get(sdata_env_qualite *data_env_qualite)
{
    data_env_qualite->eCO2 = _eCO2->get();
    data_env_qualite->TVOC = _TVOC->get();
}

void CapteurQualAir::moyenneCO2()
{
    static int counter = 0;

#ifdef DEBUG_SERIAL
    Serial.print("Raw H2 ");
    Serial.print(sgp.rawH2);
    Serial.print(" | ");
    Serial.print("Raw Ethanol ");
    Serial.print(sgp.rawEthanol);
    Serial.print(" | ");

    counter++;
    if (counter == 30)
    {
        counter = 0;

        uint16_t TVOC_base, eCO2_base;
        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            Serial.println("Failed to get baseline readings");
        }
        else
        {
            static uint16_t histo_eCO2[PROFONDEUR_HISTO] = {0};
            static uint16_t histo_TCOV[PROFONDEUR_HISTO] = {0};
            static uint16_t pos_eCO2 = 0;
            static uint16_t pos_TCOV = 0;
            static uint16_t min_eCO2 = 0xFFFF;
            static uint16_t max_eCO2 = 0;
            static uint16_t min_TCOV = 0xFFFF;
            static uint16_t max_TCOV = 0;
            uint16_t diff_eCO2;
            uint16_t diff_TCOV;

            if (eCO2_base > max_eCO2) max_eCO2 = eCO2_base;
            if (TVOC_base > max_TCOV) max_TCOV = TVOC_base;

            if (eCO2_base < min_eCO2) min_eCO2 = eCO2_base;
            if (TVOC_base < min_TCOV) min_TCOV = TVOC_base;

            diff_eCO2 = eCO2_base - histo_eCO2[((pos_eCO2) ? (pos_eCO2 - 1) : (PROFONDEUR_HISTO - 1))];
            diff_TCOV = TVOC_base - histo_TCOV[((pos_TCOV) ? (pos_TCOV - 1) : (PROFONDEUR_HISTO - 1))];

            histo_eCO2[pos_eCO2++] = eCO2_base;
            if (pos_eCO2 == PROFONDEUR_HISTO)
            {
                pos_eCO2 = 0;
            }
            uint32_t moyenneCO2 = 0;
            uint8_t indices = 0;
            for (int i = 0; i < PROFONDEUR_HISTO; i++)
            {
                moyenneCO2 += histo_eCO2[i];
                if (histo_eCO2[i] != 0)
                    indices++;
            }
            moyenneCO2 /= indices;

            histo_TCOV[pos_TCOV++] = TVOC_base;
            if (pos_TCOV == PROFONDEUR_HISTO)
            {
                pos_TCOV = 0;
            }

            Serial.print(" Baseline values: eCO2: ");
            Serial.print(eCO2_base, DEC);
            Serial.print(" min: ");
            Serial.print(min_eCO2, DEC);
            Serial.print(" moy: ");
            Serial.print(moyenneCO2, DEC);
            Serial.print(" max: ");
            Serial.print(max_eCO2, DEC);
            Serial.print(" diff: ");
            Serial.print((int16_t)diff_eCO2, DEC);
            Serial.print(" & TVOC: ");
            Serial.print(TVOC_base, DEC);
            Serial.print(" diff: ");
            Serial.println((int16_t)diff_TCOV, DEC);
        }
    }
#endif
}

void CapteurQualAir::setHumidity(uint32_t absolute_humidity)
{
    sgp.setHumidity(absolute_humidity);
}

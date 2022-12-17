/******************************************************************************
Librairie pour la supervision de l'alimentation
    Lors des mesures de tension de batterie, si la tension est inférieure à 
    3,3V (tension d'alimentation) + 0,3V de drop out max @ 600mA

******************************************************************************/

#include <Arduino.h>

#include <supervAlim.h>


/// @brief non utilisé
SupervAlim::SupervAlim() {}

/// @brief non utilisé
SupervAlim::~SupervAlim() {}

/// @brief Initialise l'objet de supervision de la batterie
/// @param _delayTime : délai entre chaque mesure
void SupervAlim::init(ulong _delayTime)
{
    delayTime = _delayTime;
    chgt = false;

    _niveau = new VariableShared<uint8_t>(0);
    _valeur = new VariableShared<float>(0);

    // Démarrer la tâche de mise à jour
    xTaskCreatePinnedToCore(
        (TaskFunction_t)tacheMAJ,        // Task function.
        "alim Task",                     // name of task.
        2048,                            // Stack size of task
        this,                            // parameter of the task
        2,                               // priority of the task
        &id_tache,                       // Task handle to keep track of created task
        0);                              // sur CPU1
}

/// @brief Tache pour la lecture de l'alimentation
/// @param pvParameter pointeur sur l'objet qui crée la tache
void SupervAlim::tacheMAJ(void *pvParameter)
{
    // Pointeur sur la tache appelante
    SupervAlim *moi = (SupervAlim *)pvParameter;

    // Délai en millisecondes
    TickType_t xDelay = moi->delayTime / portTICK_PERIOD_MS;

    uint8_t t_niveau;
    float t_valeur;

    // Boucle infinie
    while (true)
    {
        // Mesure la tension, multiplié par 2 (pont diviseur de tension) et
        // converti en float
        t_valeur = (float)(analogRead(VBAT) << 1);
        // Conversion en tension
        t_valeur *= 3.3;
        t_valeur /= 4095.0;

        if (t_valeur < 3.60)
        {
            t_niveau = 10;        // Inutile, just for fun
            // Passe en sommeil profond, avec pour seule sortie le reset
            esp_deep_sleep_start();
        }
        // Découle le niveau de tension
        else if (t_valeur < 3.70)
            t_niveau = 20;
        else if (t_valeur < 3.75)
            t_niveau = 30;
        else if (t_valeur < 3.79)
            t_niveau = 40;
        else if (t_valeur < 3.83)
            t_niveau = 50;
        else if (t_valeur < 3.87)
            t_niveau = 60;
        else if (t_valeur < 3.92)
            t_niveau = 70;
        else if (t_valeur < 3.97)
            t_niveau = 80;
        else if (t_valeur < 4.10)
            t_niveau = 90;
        else if (t_valeur < 4.20)
            t_niveau = 100;
        else
            t_niveau = 200;

        moi->_valeur->set(t_valeur);
        moi->_niveau->set(t_niveau);
        moi->chgt = true;

        // Pause avant relance
        vTaskDelay(xDelay);
    }
}

/// @brief Regarde si une nouvelle valeur est arrivée
/// @param
/// @return vrai ou faux
uint8_t SupervAlim::lecture()
{
    if (chgt)
    {
        chgt = false;
        return true;
    }
    return false;
}

/// @brief Interroge le niveau de charge en %
/// @return valeur entre 0 et 100%
uint8_t SupervAlim::niveau()
{
    return _niveau->get();
}

/// @brief Interroge la tension de la batterie en Volt
/// @return valeur en volt
float SupervAlim::valeur()
{
    return _valeur->get();
}


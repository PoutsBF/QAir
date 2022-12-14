/******************************************************************************
Librairie pour la supervision de l'alimentation
    Lors des mesures de tension de batterie, si la tension est inférieure à 
    3,3V (tension d'alimentation) + 0,3V de drop out max @ 600mA

******************************************************************************/

#include <Arduino.h>

#include <supervAlim.h>

/// @brief non utilisé
SupervAlim::SupervAlim()
{}

/// @brief non utilisé
SupervAlim::~SupervAlim()
{}

/// @brief Fonction appelée régulièrement pour évaluer la tension
/// TODO : transformer en task avec un delay entre chaques lectures
/// @return vrai si une lecture est effectuée, faux sinon.
uint8_t SupervAlim::lecture()
{
    // Test le délai ?
    if (millis() - lastDelay >= delayTime)
    {
        // Note le prochain délai
        lastDelay = millis();

        // Mesure la tension, multiplié par 2 (pont diviseur de tension) et 
        // converti en float
        _valeur = (float)(analogRead(VBAT) << 1);
        // Conversion en tension
        _valeur *= 3.3;
        _valeur /= 4095.0;

        if (_valeur < 3.60) 
        {
            _niveau = 10;   // Inutile, just for fun
            // Passe en sommeil profond, avec pour seule sortie le reset
            esp_deep_sleep_start();
        }
        // Découle le niveau de tension
        else if (_valeur < 3.70) _niveau = 20;
        else if (_valeur < 3.75) _niveau = 30;
        else if (_valeur < 3.79) _niveau = 40;
        else if (_valeur < 3.83) _niveau = 50;
        else if (_valeur < 3.87) _niveau = 60;
        else if (_valeur < 3.92) _niveau = 70;
        else if (_valeur < 3.97) _niveau = 80;
        else if (_valeur < 4.10) _niveau = 90;
        else if (_valeur < 4.20) _niveau = 100;
        else _niveau = 200;

        return true;
    }
    return false;
}

/// @brief Interroge le niveau de charge en %
/// @return valeur entre 0 et 100%
uint8_t SupervAlim::niveau()
{
    return _niveau;
}

/// @brief Interroge la tension de la batterie en Volt
/// @return valeur en volt
float SupervAlim::valeur()
{
    return _valeur;
}

/// @brief Initialise l'objet de supervision de la batterie
/// @param _delayTime : délai entre chaque mesure
void SupervAlim::init(ulong _delayTime)
{
    delayTime = _delayTime;
    lastDelay = 0 - delayTime;

    _valeur = 0;
    _niveau = 0;
}

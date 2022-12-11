/******************************************************************************
Librairie pour la supervision de l'alimentation

******************************************************************************/

#include <Arduino.h>

#include <supervAlim.h>

SupervAlim::SupervAlim(/* args */)
{}

SupervAlim::~SupervAlim()
{}

uint8_t SupervAlim::lecture()
{
    if (millis() - lastDelay >= delayTime)
    {
        lastDelay = millis();

        _valeur = (float)(analogRead(VBAT) << 1);
        Serial.printf("\nTension : 0x%.0f ", _valeur);
        _valeur *= 3.3;
        _valeur /= 4095.0;

        Serial.printf("\nsoit : %.2f V\n", _valeur);

        if(_valeur < 3.00) _niveau = 0;
        else if (_valeur < 3.30) _niveau = 5;
        else if (_valeur < 3.60) _niveau = 10;
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

uint8_t SupervAlim::niveau()
{
    return _niveau;
}

float SupervAlim::valeur()
{
    return _valeur;
}

void SupervAlim::init(ulong _delayTime)
{
    delayTime = _delayTime;
    lastDelay = 0 - delayTime;

    _valeur = 0;
    _niveau = 0;
}

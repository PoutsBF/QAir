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

        _valeur = analogRead(VBAT);

        return true;
    }
    return false;
}

uint8_t SupervAlim::valeur()
{
    return _valeur;
}

void SupervAlim::init(ulong _delayTime)
{
    delayTime = _delayTime;
    lastDelay = 0 - delayTime;

    _valeur = 0;
}

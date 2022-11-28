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
    if (millis() - lastTime >= delayTime)
    {
        return true;
    }
    return false;
}

void SupervAlim::init(ulong _delayTime)
{
    delayTime = _delayTime;
    lastTime = 0 - delayTime;
}

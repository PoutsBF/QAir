/******************************************************************************
Librairie pour la gestion ddes néoleds

******************************************************************************/

#include <Arduino.h>

#include "StripLed.h"

StripLed::StripLed(/* args */)
{
    strip.setPin(15);       // A MODIFIER !!
    strip.updateType(NEO_GRBW + NEO_KHZ800);
    strip.updateLength(8);

}

StripLed::~StripLed()
{
}

uint32_t StripLed::Wheel(byte WheelPos)
{    
}
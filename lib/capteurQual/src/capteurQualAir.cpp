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
    Constructeurs & destructeurs
******************************************************************************/
CapteurQualAir::CapteurQualAir()
{
    device_OK = 0;
}

CapteurQualAir::~CapteurQualAir()
{
}

/******************************************************************************
    Méthodes
******************************************************************************/

uint8_t CapteurQualAir::init()
{

}

uint8_t CapteurQualAir::lecture()
{
    
}
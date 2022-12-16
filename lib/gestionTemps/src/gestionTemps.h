#pragma once
/******************************************************************************
Librairie pour la gestion ddes néoleds
                                                                  02/12/2022

******************************************************************************/

#include <Arduino.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

class GestionTemps
{
private:
    /* data */
    WiFiUDP *ntpUDP;
    NTPClient *timeClient;

public:
    GestionTemps() {}
    ~GestionTemps() {}

    void init(void);
    ulong lecture(void);
};


/******************************************************************************
Librairie pour la gestion ddes néoleds
                                                                  02/12/2022

******************************************************************************/

#include <Arduino.h>
#include <stdio.h>
#include <gestionTemps.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "gestionTemps.h"

void GestionTemps::init(void)
{
    ntpUDP = new WiFiUDP;
    timeClient = new NTPClient(*ntpUDP, "europe.pool.ntp.org",3600);

    timeClient->begin();
}

unsigned long GestionTemps::lecture(void)
{
    timeClient->update();
    return timeClient->getEpochTime();
}

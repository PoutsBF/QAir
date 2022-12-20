#pragma once
/******************************************************************************
Librairie pour la gestion ddes néoleds
                                                                  02/12/2022

 TODO : utiliser le RTC, mettre à jour une seule fois au démarrage et supprimer
            l'objet NTPClient et WifiUDP quand on n'en a plus besoin...

******************************************************************************/

#include <Arduino.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <variableShared.h>

#include <RTClib.h>

class GestionTemps
{
private:
    RTC_DS3231 rtc;
    uint8_t _device_ok;

    unsigned long delta;        // Délai entre 2 appels
    uint8_t chgt;               // Indicateur d'une mise à jour

    VariableShared<ulong> *epochTime;   // Heure au format 1970 + nb secondes

    TaskHandle_t id_tache_rtc;                       // Handle de la tache de maj du ntc
    static void tacheRTC(void *pvParameters);        // tache de maj du ntc

    TaskHandle_t id_tache_ntc;        // Handle de la tache de maj du ntc
    static void reglageRTC(void *pvParameters);

public:
    GestionTemps()
    {
    }                           // Constructeur, non utilisé
    ~GestionTemps() {}          // Destructeur, non utilisé

    void init(void);            // Procédure d'initialisation
    String get();                // Lecture de la valeur du temps en epochtime
    uint8_t lecture(void);      // Regarde si une nouvelle valeur est disponible
};


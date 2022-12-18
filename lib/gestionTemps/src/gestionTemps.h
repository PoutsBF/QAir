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
    WiFiUDP *ntpUDP;            // Connecteur UDP
    NTPClient *timeClient;      // Objet pour la gestion du ntp

    unsigned long delta;        // Délai entre 2 appels
    uint8_t chgt;               // Indicateur d'une mise à jour

    VariableShared<ulong> *epochTime;   // Heure au format 1970 + nb secondes

    TaskHandle_t id_tache_ntc;          // Handle de la tache de maj du ntc
    static void tacheMAJ(void *pvParameters);   // tache de maj du ntc

public:
    GestionTemps() {}           // Constructeur, non utilisé
    ~GestionTemps() {}          // Destructeur, non utilisé

    void init(void);            // Procédure d'initialisation
    ulong get();                // Lecture de la valeur du temps en epochtime
    uint8_t lecture(void);      // Regarde si une nouvelle valeur est disponible
};


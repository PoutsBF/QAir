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

/// @brief Méthode initialisant l'objet de gestion du temps
/// @param  
void GestionTemps::init(void)
{
    if(!rtc.begin())
    {
        Serial.println("RTC non trouvé...");
        _device_ok = false;
    }
    _device_ok = true;

    epochTime = new VariableShared<ulong>(0);

    // Démarrer la tâche de mise à jour du NTC
    xTaskCreatePinnedToCore(
        (TaskFunction_t)reglageRTC,        // Task function.
        "ntp Task",                      // name of task.
        2048,                            // Stack size of task
        this,                            // parameter of the task
        2,                               // priority of the task
        &id_tache_ntc,                   // Task handle to keep track of created task
        0);                              // sur CPU1    
}

void GestionTemps::tacheRTC(void *pvParameters)
{
    // Pointeur sur la tache appelante
    GestionTemps *moi = (GestionTemps *)pvParameters;

    // Délai en millisecondes
    // TickType_t xDelay = moi->_delayTime / portTICK_PERIOD_MS;

    while(true)
    {

        
    }
}

/// @brief Met à jour l'affichage des leds néopixels.
/// Tache infinie avec délai
void GestionTemps::reglageRTC(void *pvParameters)
{
    WiFiUDP *ntpUDP;              // Connecteur UDP
    NTPClient *timeClient;        // Objet pour la gestion du ntp
    DateTime datemaj;

    // Pointeur sur la tache appelante
    GestionTemps *moi = (GestionTemps *)pvParameters;

    ntpUDP = new WiFiUDP;
    timeClient = new NTPClient(*ntpUDP, "europe.pool.ntp.org", 3600);

    timeClient->begin();

    timeClient->update();

    moi->rtc.adjust(DateTime(timeClient->getEpochTime()));

    Serial.print("ntc / epochtime :");
    Serial.println(moi->epochTime->get());

    Serial.printf("rtc : %s\n", DateTime(moi->rtc.now()).timestamp().c_str());

    // todo : mettre le RTC à l'heure avec le résultat du NTC

    delete ntpUDP;
    delete timeClient;

    vTaskDelete(NULL);
}

/// @brief Regarde si une nouvelle valeur est arrivée
/// @param  
/// @return vrai ou faux
uint8_t GestionTemps::lecture(void)
{
    if (chgt)
    {
        chgt = false;
        return true;
    }
    return false;
}

/// @brief Renvoie la valeur epochTime (secondes après 1970)
/// @return
String GestionTemps::get()
{
    printf("heure : %s\n", rtc.now().toString());
    return DateTime(rtc.now()).timestamp();
}

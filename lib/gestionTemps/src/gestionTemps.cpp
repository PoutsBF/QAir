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
    ntpUDP = new WiFiUDP;
    timeClient = new NTPClient(*ntpUDP, "europe.pool.ntp.org",3600);

    timeClient->begin();

    epochTime = new VariableShared<ulong>(0);
    chgt = 0;

    delta = 30000;

    // Démarrer la tâche de mise à jour
    xTaskCreatePinnedToCore(
        (TaskFunction_t)tacheMAJ,        // Task function.
        "ntp Task",                    // name of task.
        2048,                            // Stack size of task
        this,                            // parameter of the task
        2,                               // priority of the task
        &id_tache_ntc,                   // Task handle to keep track of created task
        0);                              // sur CPU1
}

/// @brief Met à jour l'affichage des leds néopixels.
/// Tache infinie avec délai
void GestionTemps::tacheMAJ(void *pvParameters)
{
    // Pointeur sur la tache appelante
    GestionTemps *moi = (GestionTemps *)pvParameters;

    // Délai en millisecondes
    TickType_t xDelay = moi->delta / portTICK_PERIOD_MS;

    // Boucle infinie
    while (true)
    {
        moi->timeClient->update();
        moi->epochTime->set(moi->timeClient->getEpochTime());

        Serial.printf("Heure : %d", moi->epochTime->get());

        moi->chgt = true;

        // Pause avant relance
        vTaskDelay(xDelay);
    }
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
ulong GestionTemps::get()
{
    return epochTime->get();
}

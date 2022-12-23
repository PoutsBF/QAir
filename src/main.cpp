/******************************************************************************
* Stéphane Lepoutère                                                  11-2022 *
*                                   QAir                                      *
*
* Programme de surveillance de la qualité de l'air
*   - lecture 
*       - eCO2
*       - TCOV
*       - °C
*       - %HR
*           -> hygro absolue mg/m3
*       - hPa
*       - niveau batterie
*   - Affichage sur SDD1306
*   - Affichage sur site web
*       - sous forme de webSocket
******************************************************************************/

#include <Arduino.h>
#include <stdio.h>

#define DEBUG_SERIAL

// #include <configWeb.h>

#include <variableShared.h>

#include <capteurEnv.h>
#include <capteurQualAir.h>
#include <stripLed.h>
#include <displayQAir.h>
#include <supervAlim.h>
#include <webServeur.h>
#include <gestionTemps.h>

//---------------------------------------------------------
// Gestion du capteur environnement
//
CapteurEnv capteurEnv;
CapteurQualAir capteurQualAir;

StripLed stripled;
DisplayQAir displayQAir;

SupervAlim supervAlim;
WebServeur webServeur;

GestionTemps gestionTemps;

/******************************************************************************
 *   Début du SETUP
 *
 ******************************************************************************/
void setup()
{
    Serial.begin(115200);

    while ((!Serial) && (millis() < 5000));

    Serial.println("-----------------------------------");
    Serial.println("-- Stéphane Lepoutère ----- 2022 --");
    Serial.println("-- Mesure de la qualité de l'air --");
    Serial.println("-----------------------------------");

    displayQAir.init();

    capteurEnv.init(30000);
    capteurQualAir.init(10000);

    stripled.init(13);

    supervAlim.init(60000);

    webServeur.init();

    gestionTemps.init();
}

/******************************************************************************
 *   LOOP
 *
 ******************************************************************************/
void loop()
{
    static sdata_env_qualite data_env_qualite = {0};        // Stocke les données eCO2 et TCOV
    static sdata_env data_env = {0};
    uint8_t changement = false;

    if(capteurEnv.lecture())
    {
        capteurEnv.get(&data_env);
        capteurQualAir.setHumidity(data_env.hygroAbsolue);

        // Affiche les données sur l'écran lcd s'il est correctement initialisé
        displayQAir.displayAffiche(data_env);
        webServeur.maj_data(JS_temperature, data_env.temperature);
        webServeur.maj_data(JS_hygroRel, data_env.humidite);
        webServeur.maj_data(JS_hygroAbs, data_env.hygroAbsolue);
        webServeur.maj_data(JS_pression, data_env.pression);

        changement = true;
    }

    if(capteurQualAir.lecture())
    {
        capteurQualAir.get(&data_env_qualite);
        
        displayQAir.displayAffiche(data_env_qualite);

        stripled.afficheStrip(data_env_qualite.eCO2);

        webServeur.maj_data(JS_eco2, data_env_qualite.eCO2);
        webServeur.maj_data(JS_tcov, data_env_qualite.TVOC);

        changement = true;
    }

    if (supervAlim.lecture())
    {
        webServeur.maj_data(JS_battNiveau, (uint16_t)supervAlim.niveau());
        webServeur.maj_data(JS_battTension, supervAlim.valeur());

        changement = true;
    }

    if (changement)
    {
        changement = false;
        webServeur.maj_data(JS_TimeStamp, gestionTemps.get());
        webServeur.send(0);
    }

    webServeur.cleanupClients();
}

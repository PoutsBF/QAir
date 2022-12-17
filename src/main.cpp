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

    Serial.println("Bonjour !");

    displayQAir.init();
    Serial.println("display");

    capteurEnv.init(30000);
    Serial.println("bme280");
    capteurQualAir.init(10000);
    Serial.println("sgp30");

    stripled.init(13);
    Serial.println("neopixel");

    supervAlim.init(60000);
    Serial.println("alim'");


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

    if(capteurEnv.lecture(&data_env))
    {
        capteurQualAir.setHumidity(data_env.hygroAbsolue);

        // Affiche les données sur l'écran lcd s'il est correctement initialisé
        displayQAir.displayAffiche(data_env);
        webServeur.maj_data(JS_temperature, data_env.temperature);
        webServeur.maj_data(JS_hygroRel, data_env.humidite);
        webServeur.maj_data(JS_hygroAbs, data_env.hygroAbsolue);
        webServeur.maj_data(JS_pression, data_env.pression);
        webServeur.send(0); 
    }

    if(capteurQualAir.lecture(&data_env_qualite))
    {
        displayQAir.displayAffiche(data_env_qualite);

        stripled.afficheStrip(data_env_qualite.eCO2);

        webServeur.maj_data(JS_eco2, data_env_qualite.eCO2);
        webServeur.maj_data(JS_tcov, data_env_qualite.TVOC);
        webServeur.send(0);
    }

    if (supervAlim.lecture())
    {
        webServeur.maj_data(JS_battNiveau, (uint16_t)supervAlim.niveau());
        webServeur.maj_data(JS_battTension, supervAlim.valeur());

        webServeur.send(0);
    }

    if(gestionTemps.lecture())
    {
        webServeur.maj_data("timeStp", gestionTemps.get());
        webServeur.send(0);
    }

    webServeur.cleanupClients();
}

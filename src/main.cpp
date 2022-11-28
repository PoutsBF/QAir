#include <Arduino.h>

#define DEBUG_SERIAL

#include <SPI.h>
#include <Wire.h>

#include <capteurEnv.h>
#include <capteurQualAir.h>
#include <stripLed.h>
#include <displayQAir.h>

#include <Adafruit_Sensor.h>

#include <WiFi.h>
#include <FS.h>

//---------------------------------------------------------
// Gestion du capteur environnement
//
CapteurEnv capteurEnv;
CapteurQualAir capteurQualAir;

StripLed stripled;
DisplayQAir displayQAir;


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

    capteurEnv.init(30000);
    capteurQualAir.init(10000);

    stripled.init();

}

/******************************************************************************
 *   LOOP
 *
 ******************************************************************************/
void loop()
{
    static unsigned long lastSgp = 0;
    static sdata_env_qualite data_env_qualite = {0};        // Stocke les données eCO2 et TCOV
    static sdata_env data_env = {0};

    if(capteurEnv.lecture(&data_env))
    {
        capteurQualAir.setHumidity(data_env.hygroAbsolue);

        // Affiche les données sur l'écran lcd s'il est correctement initialisé
        displayQAir.displayAffiche(data_env);
    }

    if(capteurQualAir.lecture(&data_env_qualite))
    {
        displayQAir.displayAffiche(data_env_qualite);

        stripled.afficheStrip(data_env_qualite.eCO2);
    }
}

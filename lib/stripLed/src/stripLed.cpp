/******************************************************************************
Librairie pour la gestion ddes néoleds

******************************************************************************/

#include <Arduino.h>

#include <stripLed.h>

/// @brief Constructeur & destructeur // non utilisé
StripLed::StripLed() {}
StripLed::~StripLed() {}

/// @brief Met à jour l'affichage des leds néopixels.
/// Tache infinie avec délai
void StripLed::tacheMAJ(void *pvParameters)
{
    // Pointeur sur la tache appelante
    StripLed * moi = (StripLed *) pvParameters;
    // Variable sur la position dans le cycle
    uint8_t q = 0;

    // Délai en millisecondes
    TickType_t xDelay = moi->delta / portTICK_PERIOD_MS;

    // Boucle infinie
    while (true)
    {
        // Fait tourner la led de 0 à ledmax et ramène à 0
        if (q >= (moi->strip.numPixels() - moi->nbLedStrip))
            q = 0;
        else
            q++;

        // Nettoie le ruban de led
        moi->strip.clear();

        // Affiche les leds adéquates
        for (uint16_t i = 0; 
                        i < moi->strip.numPixels(); 
                        i += 1 + moi->strip.numPixels() - moi->nbLedStrip)
        {
            moi->strip.setPixelColor(i + q, moi->couleurStrip);
        }
        // Affiche sur le ruban
        moi->strip.show();

        // Pause avant relance
        vTaskDelay(xDelay);
    }
}

/// @brief
/// @param WheelPos
/// @return
uint32_t StripLed::Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/// @brief Initialisation de l'objet de gestion du ruban
/// @param pin patte sur laquelle est connectée le ruban
void StripLed::init(uint8_t pin)
{    
    strip.setPin(pin);                      // Configure la patte
    strip.updateType(NEO_GRB + NEO_KHZ800); // Type de led et fréquence
    strip.updateLength(16);                 // Nombre de led

    strip.begin();              // Initialise l'objet NeoPixel
    strip.show();               // Eteint le ruban
    strip.setBrightness(10);    // Configure la luminosité

    delta = 100;                // Délai d'avance

    couleurStrip = 0;           // Couleur par défaut
    nbLedStrip = 0;             // Nombre de led par défaut

    device_ok = true;           // Valide l'initialisation de l'objet

    // Démarrer la tâche de mise à jour 
      xTaskCreatePinnedToCore(
            (TaskFunction_t) tacheMAJ,              // Task function. 
            "strip Task",             // name of task. 
            2048,                   // Stack size of task 
            this,                   // parameter of the task 
            2,                      // priority of the task 
            &id_tache_ntc,          // Task handle to keep track of created task 
            0 );                    // sur CPU1 
}

/// @brief Fonction pour la couleur en fonction du niveau de CO2
/// @param eCO2
void StripLed::afficheStrip(uint16_t eCO2)
{
    // Efface le ruban
    strip.clear();

    if (eCO2 < 750)
    {
        delta = 100;
        couleurStrip = 0x00FF00;
        nbLedStrip = 1;
    }
    else if (eCO2 < 1000)
    {
        delta = 200;
        couleurStrip = 0x3FDC04;
        nbLedStrip = 2;
    }
    else if (eCO2 < 1250)
    {
        delta = 300;
        couleurStrip = 0x79bd08;
        nbLedStrip = 3;
    }
    else if (eCO2 < 1500)
    {
        delta = 400;
        couleurStrip = 0xB39e0c;
        nbLedStrip = 4;
    }
    else if (eCO2 < 2000)
    {
        delta = 500;
        couleurStrip = 0xEd7f10;
        nbLedStrip = 5;
    }
    else if (eCO2 < 2500)
    {
        delta = 600;
        couleurStrip = 0xF44708;
        nbLedStrip = 6;
    }
    else if (eCO2 < 3000)
    {
        delta = 700;
        couleurStrip = 0xf92805;
        nbLedStrip = 7;
    }
    else
    {
        delta = 1000;
        couleurStrip = 0xFF0000;
        nbLedStrip = 7;
    }
}

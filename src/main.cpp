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

    displayOK = initDisplay();

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
        if (displayOK) displayAffiche(data_env);
    }

    if(capteurQualAir.lecture(&data_env_qualite))
    {
        if (displayOK) displayAffiche(data_env_qualite);

        stripled.afficheStrip(data_env_qualite.eCO2);
    }


}

/******************************************************************************
* Initialisation du SSD1306
* retourne ok si présent et démarré
******************************************************************************/
uint8_t initDisplay()
{
    //-------------------------------------------------------------------------
    // Démarrage du oled SSD1306
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    Serial.println("-- Configuration du SSD1306 OK (OLED) --");
    Serial.println(F("SSD1306 config ok"));

    //******************************************************************
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(2000);        // Pause for 2 seconds

    // Clear the buffer
    display.clearDisplay();

    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!
    display.setTextSize(1);                     // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);                    // Start at top-left corner
    display.cp437(true);                        // Use full 256 char 'Code Page 437' font
    delay(2000);
    // display.display() is NOT necessary after every single drawing command,
    // unless that's what you want...rather, you can batch up a bunch of
    // drawing operations and then update the screen all at once by calling
    // display.display(). These examples demonstrate both approaches...

    // Invert and restore display, pausing in-between
    display.invertDisplay(true);
    delay(1000);
    display.invertDisplay(false);
    delay(1000);

    return true;
}

void displayAffiche(sdata_env data_env)
{
#ifdef DEBUG_SERIAL
    Serial.printf("%0.2f°C, %0.2f %%HR, %0.0fhPa, %u m3\n", data_env.temperature, data_env.humidite, data_env.pression, data_env.hygroAbsolue);
#endif

    display.setTextSize(2);        // Double 2:1 pixel scale
    display.fillRect(0, 0, 128, 30, SSD1306_BLACK);
    display.setCursor(0, 0);
    display.print(data_env.temperature, 1);
    display.setCursor(84, 0);
    display.print(data_env.humidite, 0);
    display.setCursor(0, 16);
    display.print(data_env.pression, 0);
    display.setCursor(79, 16);
    display.print(data_env.hygroAbsolue);
    display.setTextSize(1);        // Normal 1:1 pixel scale
    display.setCursor(53, 1);
    display.print("°C");
    display.setCursor(110, 1);
    display.print("%HR");
    display.setCursor(48, 15);
    display.print("hPa");
    display.setCursor(110, 15);
    display.print("mg/");
    display.setCursor(110, 23);
    display.print(" m3");
    display.display();
}

void displayAffiche(sdata_env_qualite data_env_qualite)
{
    Serial.printf("%u ppm eCO2, %u ppb TCOV\n", data_env_qualite.eCO2, data_env_qualite.TVOC);

    display.setTextSize(2);        // Normal 1:1 pixel scale
    display.fillRect(0, 31, 128, 32, SSD1306_BLACK);
    display.setCursor(0, 32);
    display.print(data_env_qualite.TVOC);
    display.setTextSize(1);        // Normal 1:1 pixel scale
    display.print("ppb TCOV");
    display.setCursor(0, 48);
    display.setTextSize(2);        // Normal 1:1 pixel scale
    display.print(data_env_qualite.eCO2);
    display.setTextSize(1);        // Normal 1:1 pixel scale
    display.print("ppm eCO2");
    display.display();
}

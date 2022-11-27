#include <Arduino.h>

#define DEBUG_SERIAL

#include <SPI.h>
#include <Wire.h>

#include <capteurEnv.h>
#include <capteurQualAir.h>

#include <Adafruit_GFX.h>
// #include <FreeMono12pt7b.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_NeoPixel.h>

#include <Adafruit_Sensor.h>

#include <Adafruit_SGP30.h>

#include <WiFi.h>
#include <FS.h>

//---------------------------------------------------------
// Gestion du capteur environnement
//
CapteurEnv capteurEnv;
//---------------------------------------------------------
// Fonctions et variables pour la gestion du capteur SGP30
//
Adafruit_SGP30 sgp;
struct sdata_env_qualite
{
    uint16_t eCO2;
    uint16_t TVOC;
};
uint8_t sgp30_OK = 0;
uint8_t initSGP30(void);
#define PROFONDEUR_HISTO 30
void moyenneCO2(void);

//---------------------------------------------------------
// Configuration néopixel (strip)
// Patte à connecter à définir @todo
Adafruit_NeoPixel strip(8, 15, NEO_GRBW + NEO_KHZ800);
uint8_t stripOK = 0;
uint8_t initStrip();
void afficheStrip(uint16_t eCO2);
uint32_t Wheel(byte WheelPos);

//---------------------------------------------------------
// Gestion du SSD1306
//
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL)
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

#define OLED_RESET 21              // Reset pin # (or -1 if sharing Arduino reset pin) @todo : modifier la patte !
#define SCREEN_ADDRESS 0x3C        ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t displayOK = 0;
uint8_t initDisplay(void);
void displayAffiche(sdata_env);
void displayAffiche(sdata_env_qualite);

/******************************************************************************
 *   Début du SETUP
 *
 ******************************************************************************/
void setup()
{
    
    configTime(0, 0, "pool.ntp.org");
    Serial.begin(115200);

    while ((!Serial) && (millis() < 5000));        

    Serial.println("Bonjour !");

    displayOK = initDisplay();
    capteurEnv.init(10000);

    sgp30_OK = initSGP30();

    stripOK = initStrip();
}

/******************************************************************************
 *   LOOP
 *
 ******************************************************************************/
void loop()
{
    static unsigned long lastSgp = 0;
    static sdata_env_qualite data_env_qualite = {0};         // Stocke les données eCO2 et TCOV
    static sdata_env data_env = {0};

    if(capteurEnv.lecture(&data_env))
    {
        if (sgp30_OK) sgp.setHumidity(data_env.hygroAbsolue);

        // Affiche les données sur l'écran lcd s'il est correctement initialisé
        if (displayOK) displayAffiche(data_env);
    }

    if(millis() - lastSgp >= 1000)    // toutes les secondes
    {
        lastSgp = millis();
        if (!sgp.IAQmeasure())
        {
            Serial.println("Measurement failed");
        }
        else
        {
            //-------- calcul filtre K = 0,5
            static uint16_t fn_1 = 0;

            data_env_qualite.eCO2 = (fn_1 >> 1) + (sgp.eCO2 >> 1);        // K = 0,5 donc décalage de 1 pour division par 2
            fn_1 = data_env_qualite.eCO2;
            data_env_qualite.TVOC = sgp.TVOC;

            displayAffiche(data_env_qualite);

            if (!sgp.IAQmeasureRaw())
            {
                Serial.println("Raw Measurement failed");
            }
            else
            {
                moyenneCO2();
            }
            // Serial.println();
        }
    }

    afficheStrip(data_env_qualite.eCO2);

    //    info_memoire();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
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

uint8_t initSGP30(void)
{
    uint8_t nbTest = 10;            // 10 essais
    uint8_t connexionOK = 0;        // état de la connexion au SGP30, état inversé

    //-------------------------------------------------------------------------
    // Démarrage du SGP30
    do
    {
        Serial.println("Essai de connexion du SGP30");
        // Délai avant d'agir
        delay(200);
        // Teste la connexion avec le BME280
        connexionOK = !sgp.begin();
    } while (connexionOK && nbTest--);

    if (connexionOK)
    {
        Serial.println("Could not find a valid SGP30 sensor, check wiring!");
        return false;
    }

    Serial.print("Found SGP30 serial #");
    Serial.print(sgp.serialnumber[0], HEX);
    Serial.print(sgp.serialnumber[1], HEX);
    Serial.println(sgp.serialnumber[2], HEX);

    return true;
}

//-----------------------------------------------------------------------------
// @todo : renommer
// Fonction pour filtrer la valeur du CO2
//
void moyenneCO2()
{
    static int counter = 0;

    // Serial.print("Raw H2 ");
    // Serial.print(sgp.rawH2);
    // Serial.print(" \t");
    // Serial.print("Raw Ethanol ");
    // Serial.print(sgp.rawEthanol);
    // Serial.println("");

    counter++;
    if (counter == 30)
    {
        counter = 0;

        uint16_t TVOC_base, eCO2_base;
        if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
        {
            Serial.println("Failed to get baseline readings");
        }
        else
        {
            static uint16_t histo_eCO2[PROFONDEUR_HISTO] = {0};
            static uint16_t histo_TCOV[PROFONDEUR_HISTO] = {0};
            static uint16_t pos_eCO2 = 0;
            static uint16_t pos_TCOV = 0;
            static uint16_t min_eCO2 = 0xFFFF;
            static uint16_t max_eCO2 = 0;
            static uint16_t min_TCOV = 0xFFFF;
            static uint16_t max_TCOV = 0;
            uint16_t diff_eCO2;
            uint16_t diff_TCOV;

            if (eCO2_base > max_eCO2) max_eCO2 = eCO2_base;
            if (TVOC_base > max_TCOV) max_TCOV = TVOC_base;

            if (eCO2_base < min_eCO2) min_eCO2 = eCO2_base;
            if (TVOC_base < min_TCOV) min_TCOV = TVOC_base;

            diff_eCO2 = eCO2_base - histo_eCO2[((pos_eCO2) ? (pos_eCO2 - 1) : (PROFONDEUR_HISTO - 1))];
            diff_TCOV = TVOC_base - histo_TCOV[((pos_TCOV) ? (pos_TCOV - 1) : (PROFONDEUR_HISTO - 1))];

            histo_eCO2[pos_eCO2++] = eCO2_base;
            if (pos_eCO2 == PROFONDEUR_HISTO)
            {
                pos_eCO2 = 0;
            }
            uint32_t moyenneCO2 = 0;
            uint8_t indices = 0;
            for (int i = 0; i < PROFONDEUR_HISTO; i++)
            {
                moyenneCO2 += histo_eCO2[i];
                if (histo_eCO2[i] != 0)
                    indices++;
            }
            moyenneCO2 /= indices;

            histo_TCOV[pos_TCOV++] = TVOC_base;
            if (pos_TCOV == PROFONDEUR_HISTO)
            {
                pos_TCOV = 0;
            }

            Serial.print("****Baseline values: eCO2: ");
            Serial.print(eCO2_base, DEC);
            Serial.print(" min: ");
            Serial.print(min_eCO2, DEC);
            Serial.print(" moy: ");
            Serial.print(moyenneCO2, DEC);
            Serial.print(" max: ");
            Serial.print(max_eCO2, DEC);
            Serial.print(" diff: ");
            Serial.print((int16_t)diff_eCO2, DEC);
            Serial.print(" & TVOC: ");
            Serial.print(TVOC_base, DEC);
            Serial.print(" diff: ");
            Serial.println((int16_t)diff_TCOV, DEC);
        }
    }
}

//-----------------------------------------------------------------------------
// Initialise les néopixels
//
uint8_t initStrip()
{
    strip.begin();                  // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();                   // Turn OFF all pixels ASAP
    strip.setBrightness(50);        // Set BRIGHTNESS to about 1/5 (max = 255)

    return true;
}

//-----------------------------------------------------------------------------
// Fonction pour la couleur en fonction du niveau de CO2
//
void afficheStrip(uint16_t eCO2)
{
    static unsigned long delta = 500;
    static unsigned long tempo = 0 - delta;
    static uint8_t q = 0;
    static uint32_t couleurStrip = 0;
    static uint8_t nbLedStrip = 0;

    strip.clear();
    if (eCO2 < 750)
    {
        delta = 1000;
        couleurStrip = 0x00FF00;
        nbLedStrip = 1;
    }
    else if (eCO2 < 1000)
    {
        delta = 900;
        couleurStrip = 0x3FDC04;
        nbLedStrip = 2;
    }
    else if (eCO2 < 1250)
    {
        delta = 800;
        couleurStrip = 0x79bd08;
        nbLedStrip = 3;
    }
    else if (eCO2 < 1500)
    {
        delta = 800;
        couleurStrip = 0xB39e0c;
        nbLedStrip = 4;
    }
    else if (eCO2 < 2000)
    {
        delta = 700;
        couleurStrip = 0xEd7f10;
        nbLedStrip = 5;
    }
    else if (eCO2 < 2500)
    {
        delta = 700;
        couleurStrip = 0xF44708;
        nbLedStrip = 6;
    }
    else if (eCO2 < 3000)
    {
        delta = 600;
        couleurStrip = 0xf92805;
        nbLedStrip = 7;
    }
    else
    {
        delta = 500;
        couleurStrip = 0xFF0000;
        nbLedStrip = 7;
    }

    if (millis() - tempo > delta)
    {
        tempo = millis();

        if (q >= (strip.numPixels() - nbLedStrip))
            q = 0;
        else
            q++;

        for (uint16_t i = 0; i < strip.numPixels(); i += 1 + strip.numPixels() - nbLedStrip)
        {
            strip.setPixelColor(i + q, couleurStrip);        // turn every third pixel on
        }
        strip.show();
    }
}
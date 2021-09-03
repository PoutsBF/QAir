#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
//#include <FreeMono12pt7b.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_SGP30.h>

Adafruit_SGP30 sgp;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;        // I2C
unsigned long delayTime;

#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4               // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C        ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10        // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

Adafruit_NeoPixel strip(8, 15, NEO_GRBW + NEO_KHZ800);

// calcul de l'humiditÔøΩ absolue
uint32_t getAbsoluteHumidity(float temperature, float humidity);
uint32_t Wheel(byte WheelPos);
void info_memoire(void);

void setup()
{

    Serial.begin(9600);

    while ((!Serial) && (millis() < 5000));        

    Serial.println("Bonjour !");

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
  //      for (;;)
            ;        // Don't proceed, loop forever
    }
    else 
    {
        Serial.println(F("SSD1306 config ok"));
    }

    while (!bme.begin(0x77))
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        delay(2000);
    }

    // if ()
    // //    if (!bme.begin(0x77, &Wire))
    //     {
    //         while (1)
    //             ;
    //     }
    // weather monitoring
    Serial.println("-- Weather Station Scenario --");
    Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    Serial.println("filter off");
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,        // temperature
                    Adafruit_BME280::SAMPLING_X1,        // pressure
                    Adafruit_BME280::SAMPLING_X1,        // humidity
                    Adafruit_BME280::FILTER_OFF);

    // suggested rate is 1/60Hz (1m)
    delayTime = 60000;        // in milliseconds

    if (!sgp.begin())
    {
        Serial.println("Sensor not found");
        while (1)
            ;
    }
    Serial.print("Found SGP30 serial #");
    Serial.print(sgp.serialnumber[0], HEX);
    Serial.print(sgp.serialnumber[1], HEX);
    Serial.println(sgp.serialnumber[2], HEX);

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

    strip.begin();                  // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();                   // Turn OFF all pixels ASAP
    strip.setBrightness(50);        // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop()
{
    static unsigned long lastBme = 0 - delayTime;
    static unsigned long lastSgp = 0;

    if(millis() - lastBme >= delayTime)
    {
        float temperature;
        float humidite;
        float pression;
        uint32_t hygroAbsolue;

        lastBme = millis();

        // Only needed in forced mode! In normal mode, you can remove the next line.
        bme.takeForcedMeasurement();        // has no effect in normal mode
        temperature = bme.readTemperature();
        humidite = bme.readHumidity();
        pression = bme.readPressure() / 100.0F;

        hygroAbsolue = getAbsoluteHumidity(temperature, humidite);

        sgp.setHumidity(hygroAbsolue);
        hygroAbsolue /= 100;

        display.setTextSize(2);        // Normal 1:1 pixel scale
        display.fillRect(0, 0, 128, 30, SSD1306_BLACK);
        display.setCursor(0, 0);
        display.print(temperature, 1);
        display.setCursor(84, 0);
        display.print(humidite, 0);
        display.setCursor(0, 16);
        display.print(pression, 0);
        display.setCursor(79, 16);
        display.print(hygroAbsolue);
        display.setTextSize(1);        // Normal 1:1 pixel scale
        display.setCursor(53, 1);
        display.print("¯C");
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

    if(millis() - lastSgp >= 1000)    // toutes les secondes
    {
        static int counter = 0;

        lastSgp = millis();
        if (!sgp.IAQmeasure())
        {
            Serial.println("Measurement failed");
        }
        else
        {
            //-------- calcul filtre K = 0,5
            static uint16_t fn_1 = 0;
            uint16_t eCO2;

            eCO2 = (fn_1 >> 1) + (sgp.eCO2 >> 1);   // K = 0,5 donc dÇcalage de 1 pour division par 2
            fn_1 = eCO2;

            display.setTextSize(2);        // Normal 1:1 pixel scale
            display.fillRect(0, 31, 128, 32, SSD1306_BLACK);
            display.setCursor(0, 32);
            display.print(sgp.TVOC);
            display.setTextSize(1);        // Normal 1:1 pixel scale
            display.print("ppb TCOV");
            display.setCursor(0, 48);
            display.setTextSize(2);        // Normal 1:1 pixel scale
            display.print(eCO2);
            display.setTextSize(1);        // Normal 1:1 pixel scale
            display.print("ppm eCO2");
            display.display();

            if (!sgp.IAQmeasureRaw())
            {
                Serial.println("Raw Measurement failed");
            }
            else
            {
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
                        #define profondeurHisto 30
                        static uint16_t histo_eCO2[profondeurHisto] = {0};
                        static uint16_t histo_TCOV[profondeurHisto] = {0};
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

                        diff_eCO2 = eCO2_base - histo_eCO2[((pos_eCO2) ? (pos_eCO2 - 1) : (profondeurHisto - 1))];
                        diff_TCOV = TVOC_base - histo_TCOV[((pos_TCOV) ? (pos_TCOV - 1) : (profondeurHisto - 1))];

                        histo_eCO2[pos_eCO2++] = eCO2_base;
                        if (pos_eCO2 == profondeurHisto)
                        {
                            pos_eCO2 = 0;
                        }
                        uint32_t moyenneCO2 = 0;
                        uint8_t indices = 0;
                        for (int i = 0; i < profondeurHisto; i++)
                        {
                            moyenneCO2 += histo_eCO2[i];
                            if (histo_eCO2[i] != 0)
                                indices++;
                        }
                        moyenneCO2 /= indices;

                        histo_TCOV[pos_TCOV++] = TVOC_base;
                        if(pos_TCOV == profondeurHisto)
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
            // Serial.println();
        }
    }

    static unsigned long delta = 500;
    static unsigned long tempo = 0 - delta;
    static uint8_t q = 0;
    static uint32_t couleurStrip = 0;
    static uint8_t nbLedStrip = 0;

    strip.clear();
    if (sgp.eCO2 < 750)
    {
        delta = 1000;
        couleurStrip = 0x00FF00;
        nbLedStrip = 1;
    }
    else if (sgp.eCO2 < 1000)
    {
        delta = 900;
        couleurStrip = 0x3FDC04;
        nbLedStrip = 2;
    }
    else if (sgp.eCO2 < 1250)
    {
        delta = 800;
        couleurStrip = 0x79bd08;
        nbLedStrip = 3;
    }
    else if (sgp.eCO2 < 1500)
    {
        delta = 800;
        couleurStrip = 0xB39e0c;
        nbLedStrip = 4;
    }
    else if (sgp.eCO2 < 2000)
    {
        delta = 700;
        couleurStrip = 0xEd7f10;
        nbLedStrip = 5;
    }
    else if (sgp.eCO2 < 2500)
    {
        delta = 700;
        couleurStrip = 0xF44708;
        nbLedStrip = 6;
    }
    else if (sgp.eCO2 < 3000)
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

        if (q >= ( strip.numPixels() - nbLedStrip ))
            q = 0;
        else
            q++;

        for (uint16_t i = 0; i < strip.numPixels(); i += 1 + strip.numPixels() - nbLedStrip)
        {
            strip.setPixelColor(i + q, couleurStrip);        //turn every third pixel on
        }
        strip.show();
    }

    info_memoire();
}

uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature));        // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                       // [mg/m^3]
    return absoluteHumidityScaled;
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

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else         // __ARM__
extern char *__brkval;
#endif        // __arm__

void info_memoire(void)
{
    static int p_free = 0;

    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    if((&top - __brkval) != p_free)
    {
        p_free = &top - __brkval;
        Serial.print("m√©moire libre : ");
        Serial.println(p_free);
    }
#else         // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif        // __arm__
    }
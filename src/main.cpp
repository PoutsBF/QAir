#include <Arduino.h>
#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
//#include <FreeMono12pt7b.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#include <SparkFun_SGP30_Arduino_Library.h>        // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Zanshin_BME680.h>  // Include the BME680 Sensor library

SGP30 mySensor;        //create an instance of the SGP30 class
BME680_Class BME680;        ///< Create an instance of the BME680 class

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

// calcul de l'humiditï¿½ absolue
uint32_t getAbsoluteHumidity(float temperature, float humidity);
uint32_t Wheel(byte WheelPos);
void info_memoire(void);
void checkIaqSensorStatus(void);
void loadState(void);
void updateState(void);

void setup()
{
    EEPROM.begin();        // 1st address for the length
    Wire.begin();
    Serial.begin(9600);

    while ((!Serial) && (millis() < 5000));        

    Serial.println("Bonjour !");

    //-------------------------------------------------------------------------
    // Initialisation de l'afficheur SSD1306
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);        // Don't proceed, loop forever
    }
    else 
    {
        Serial.println(F("SSD1306 config ok"));
    }
    
    //-------------------------------------------------------------------------
    // Initialisation du capteur BME680
    while (!BME680.begin(I2C_STANDARD_MODE))
    {        // Start BME680 using I2C, use first device found
        Serial.print(F("-  Unable to find BME680. Trying again in 5 seconds.\n"));
        delay(5000);
    }        // of loop until device is located

    Serial.print(F("- Setting 16x oversampling for all sensors\n"));
    BME680.setOversampling(TemperatureSensor, Oversample16);        // Use enumerated type values
    BME680.setOversampling(HumiditySensor, Oversample16);           // Use enumerated type values
    BME680.setOversampling(PressureSensor, Oversample16);           // Use enumerated type values
    Serial.print(F("- Setting IIR filter to a value of 4 samples\n"));
    BME680.setIIRFilter(IIR4);                                                          // Use enumerated type values
    Serial.print(F("- Setting gas measurement to 320\xC2\xB0\x43 for 150ms\n"));        // "?C" symbols
    BME680.setGas(320, 150);                                                            // 320?c for 150 milliseconds

    // suggested rate is 1/60Hz (1m)
    delayTime = 60000;        // in milliseconds

    //-------------------------------------------------------------------------
    // Initialisation du capteur SGP30
    if (mySensor.begin() == false)
    {
        Serial.println("No SGP30 Detected. Check connections.");
        while (1)
            ;
    }

    //Initializes sensor for air quality readings
    //measureAirQuality should be called in one second increments after a call to initAirQuality
    mySensor.initAirQuality();

    Serial.print("Found SGP30 serial #");

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
        int32_t temp, humidity, pressure, gas;        // BME readings
        float temperature;
        float humidite;
        int16_t pression;
        uint32_t hygroAbsolue;

        lastBme = millis();

        BME680.getSensorData(temp, humidity, pressure, gas);        // Get readings
        pression = (int16_t) pressure / 100;
        temperature = temperature / 100;
        humidite = humidity / 1000;

        hygroAbsolue = getAbsoluteHumidity(temperature, humidite);

        //Set the humidity compensation on the SGP30 to the measured value
        //If no humidity sensor attached, sensHumidity should be 0 and sensor will use default
        mySensor.setHumidity(hygroAbsolue);
        Serial.print("Absolute humidity compensation set to: ");
        Serial.print(hygroAbsolue);
        Serial.println("g/m^3 ");

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
        display.print("øC");
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

        lastSgp = millis();
        SGP30ERR result = mySensor.measureAirQuality();
        if (result != SGP30_SUCCESS)
        {
            switch (result)
            {
                case SGP30_ERR_BAD_CRC:
                    Serial.println("Measurement failed, bad CRC");
                    break;
                case SGP30_ERR_I2C_TIMEOUT:
                    Serial.println("Measurement failed, timeout");
                    break;
                case SGP30_SELF_TEST_FAIL:
                    Serial.println("Measurement failed, test fail");
                    break;

                default:
                    Serial.println("Measurement failed, d‚faut non recens‚");
                    break;
            }
        }
        else
        {
            //-------- calcul filtre K = 0,5
            static uint16_t fn_1 = 0;
            uint16_t eCO2;

            eCO2 = (fn_1 >> 1) + (mySensor.CO2 >> 1);        // K = 0,5 donc d‚calage de 1 pour division par 2
            fn_1 = eCO2;

            display.setTextSize(2);        // Normal 1:1 pixel scale
            display.fillRect(0, 31, 128, 32, SSD1306_BLACK);
            display.setCursor(0, 32);
            display.print(mySensor.TVOC);
            display.setTextSize(1);        // Normal 1:1 pixel scale
            display.print("ppb TCOV");
            display.setCursor(0, 48);
            display.setTextSize(2);        // Normal 1:1 pixel scale
            display.print(eCO2);
            display.setTextSize(1);        // Normal 1:1 pixel scale
            display.print("ppm eCO2");
            display.display();

            // Serial.println();
        }
    }

    static unsigned long delta = 500;
    static unsigned long tempo = 0 - delta;
    static uint8_t q = 0;
    static uint32_t couleurStrip = 0;
    static uint8_t nbLedStrip = 0;

    strip.clear();
    if (mySensor.CO2 < 750)
    {
        delta = 1000;
        couleurStrip = 0x00FF00;
        nbLedStrip = 1;
    }
    else if (mySensor.CO2 < 1000)
    {
        delta = 900;
        couleurStrip = 0x3FDC04;
        nbLedStrip = 2;
    }
    else if (mySensor.CO2 < 1250)
    {
        delta = 800;
        couleurStrip = 0x79bd08;
        nbLedStrip = 3;
    }
    else if (mySensor.CO2 < 1500)
    {
        delta = 800;
        couleurStrip = 0xB39e0c;
        nbLedStrip = 4;
    }
    else if (mySensor.CO2 < 2000)
    {
        delta = 700;
        couleurStrip = 0xEd7f10;
        nbLedStrip = 5;
    }
    else if (mySensor.CO2 < 2500)
    {
        delta = 700;
        couleurStrip = 0xF44708;
        nbLedStrip = 6;
    }
    else if (mySensor.CO2 < 3000)
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
    if ((&top - __brkval) != p_free)
    {
        p_free = &top - __brkval;
        Serial.print("m??moire libre : ");
        Serial.println(p_free);
    }
#else         // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif        // __arm__
}
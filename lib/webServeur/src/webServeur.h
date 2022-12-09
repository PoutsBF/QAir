/******************************************************************************
Librairie pour la gestion du serveur web

******************************************************************************/

#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

class WebServeur
{
private:
    AsyncWebServer *server;

    /* data */
public:
    WebServeur(/* args */);
    ~WebServeur();

    uint8_t init(void);
    static void notFound(AsyncWebServerRequest *request);
};


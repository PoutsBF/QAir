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

#include"configWeb.h"

class WebServeur
{
private:
    AsyncWebServer server;

    const char *PARAM_MESSAGE = "message";
    /* data */
public:
    WebServeur(/* args */);
    ~WebServeur();
};


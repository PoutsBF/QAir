/******************************************************************************
Librairie pour la gestion du serveur web

******************************************************************************/

#include <Arduino.h>
#include <webServeur.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>


WebServeur::WebServeur(/* args */)
{
}

WebServeur::~WebServeur()
{
}

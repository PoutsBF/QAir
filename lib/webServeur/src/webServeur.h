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

//#include <json_parser.h>

class WebServeur
{
private:
    AsyncWebServer *server;
    AsyncWebSocket *ws;
    AsyncEventSource *events;

    static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    static void notFound(AsyncWebServerRequest *request);

public:
    WebServeur(/* args */);
    ~WebServeur();

    uint8_t init(void);
};


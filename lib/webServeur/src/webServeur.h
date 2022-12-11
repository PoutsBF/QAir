/******************************************************************************
Librairie pour la gestion du serveur web

******************************************************************************/

#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
    #include <AsyncTCP.h>
    #include <SPIFFS.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>
#include <jsonDoc.h>

class WebServeur
{
private:
    AsyncWebServer *server;
    static AsyncWebSocket *ws;
    AsyncEventSource *events;

    static DynamicJsonDocument *doc;

    static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    static void notFound(AsyncWebServerRequest *request);

    static void heap(AsyncWebServerRequest *request);
    static void reset(AsyncWebServerRequest *request);

public:
    WebServeur(/* args */);
    ~WebServeur();

    uint8_t init(void);
    void maj_data(const char *key, const char *valeur);
    void maj_data(const char *key, float valeur);
    void maj_data(const char *key, uint16_t valeur);
    void maj_data(const char *key, uint32_t valeur);
    static void send(uint32_t id);

    void cleanupClients(void);
};


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

    template <typename T>
    void maj_data(const char *key, T valeur)
    {
        (*doc)[key] = valeur;

        Serial.print("[json] capa : ");
        Serial.print(doc->memoryUsage());
        Serial.print(" valeur : ");
        Serial.print(valeur);
        Serial.print(" clé : ");
        Serial.print(key);
        Serial.print(" enregisté : ");
        T var_temp = (*doc)[key];
        Serial.println(var_temp);

        //serializeJson(*doc, Serial);
    }

    static void send(uint32_t id);

    void cleanupClients(void);
};


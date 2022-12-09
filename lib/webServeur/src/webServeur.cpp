/******************************************************************************
Librairie pour la gestion du serveur web

******************************************************************************/

#include <Arduino.h>
#include <webServeur.h>

#define DEBUG_SERIAL

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <configWeb.h>

const char *PARAM_MESSAGE = "message";

WebServeur::WebServeur(/* args */)
{}

WebServeur::~WebServeur()
{}

uint8_t WebServeur::init()
{
    server = new AsyncWebServer(80);

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }

#ifdef DEBUG_SERIAL
    /* Détection des fichiers présents sur l'Esp32 */
    File root = SPIFFS.open("/");    /* Ouverture de la racine */
    File file = root.openNextFile(); /* Ouverture du 1er fichier */
    while (file)                     /* Boucle de test de présence des fichiers - Si plus de fichiers la boucle s'arrête*/
    {
        Serial.print("File: ");
        Serial.println(file.name());
        file.close();
        file = root.openNextFile(); /* Lecture du fichier suivant */
    }
#endif

    WiFi.mode(WIFI_STA);
    WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
#ifdef DEBUG_SERIAL
        Serial.printf("WiFi Failed!\n");
#endif
        return false;
    }

#ifdef DEBUG_SERIAL
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
#endif

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Hello, world"); });

    // Send a GET request to <IP>/get?message=<message>
    server->on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String message;
            if (request->hasParam(PARAM_MESSAGE)) {
                message = request->getParam(PARAM_MESSAGE)->value();
            } else {
                message = "No message sent";
            }
            request->send(200, "text/plain", "Hello, GET: " + message); 
        });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server->on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
        {
            String message;
            if (request->hasParam(PARAM_MESSAGE, true)) {
                message = request->getParam(PARAM_MESSAGE, true)->value();
            } else {
                message = "No message sent";
            }
            request->send(200, "text/plain", "Hello, POST: " + message); 
        });

    server->onNotFound(notFound);

    server->begin();

    return true;
}

void WebServeur::notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

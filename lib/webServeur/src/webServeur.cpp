/******************************************************************************
Librairie pour la gestion du serveur web

******************************************************************************/

#include <Arduino.h>
#include <stdio.h>
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
#include <jsonDoc.h>

const char *PARAM_MESSAGE = "message";

WebServeur::WebServeur(/* args */)
{}

WebServeur::~WebServeur()
{}

/// @brief initialise par défaut le serveur web, le web socket et
///             la variable json avec les valeurs envoyées aux
///             pages connectées
/// @return ok ou non ?
uint8_t WebServeur::init()
{
    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");
    events = new AsyncEventSource("/events");

    doc = new DynamicJsonDocument(1024);

    doc[JS_temperature] = "-";
    doc[JS_eco2] = "-";
    doc[JS_hygroAbs] = "-";
    doc[JS_hygroRel] = "-";
    doc[JS_niveauBatt] = "-";
    doc[JS_pression] = "-";
    doc[JS_tcov] = "-";

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

    // Configuration des évènements web server
    ws->onEvent(onWsEvent);
    server->addHandler(ws);

    events->onConnect([](AsyncEventSourceClient *client)
                     { client->send("hello!", NULL, millis(), 1000); });
    server->addHandler(events);

    server->on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        char texte[128];
        snprintf(texte, 128, "heap %d/%d, clients %d, dB %d",
                 ESP.getFreeHeap(), 
                 ESP.getHeapSize(),
                //  ws.count(),
                 WiFi.RSSI());
        request->send(200, "text/plain", String(texte)); });

    server->on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        request->send(200, "text/plain", String("Reset en cours..."));
        ESP.restart(); });

    server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

    server->onNotFound(notFound);

    server->begin();

    ws->text(0, "qrfq");

    return true;
}

void WebServeur::notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void WebServeur::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
//    extern CValeurs valeurs;

    if (type == WS_EVT_CONNECT)
    {
#ifdef DEBUG_SERIAL
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
#endif        
        //        valeurs.envoie(client->id());
        //        valeurs.envoie(client->id());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
#ifdef DEBUG_SERIAL
        Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
#endif
    }
    else if (type == WS_EVT_ERROR)
    {
#ifdef DEBUG_SERIAL
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
#endif
    }
    else if (type == WS_EVT_PONG)
    {
#ifdef DEBUG_SERIAL
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
#endif
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len)
        {
            // the whole message is in a single frame and we got all of it's data
#ifdef DEBUG_SERIAL
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
#endif
            if (info->opcode == WS_TEXT)
            {
                for (size_t i = 0; i < info->len; i++)
                {
                    msg += (char)data[i];
                }
            }
            else
            {
                char buff[3];
                for (size_t i = 0; i < info->len; i++)
                {
                    sprintf(buff, "%02x ", (uint8_t)data[i]);
                    msg += buff;
                }
            }
#ifdef DEBUG_SERIAL
            Serial.printf("%s\n", msg.c_str());
#endif
        }
        else
        {
            // message is comprised of multiple frames or the frame is split into multiple packets
            if (info->index == 0)
            {
                if (info->num == 0)
                    Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
            }

            Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

            if (info->opcode == WS_TEXT)
            {
                for (size_t i = 0; i < len; i++)
                {
                    msg += (char)data[i];
                }
            }
            else
            {
                char buff[3];
                for (size_t i = 0; i < len; i++)
                {
                    sprintf(buff, "%02x ", (uint8_t)data[i]);
                    msg += buff;
                }
            }
            Serial.printf("%s\n", msg.c_str());

            if ((info->index + len) == info->len)
            {
                Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                if (info->final)
                {
                    Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                }
            }
        }
    }
}

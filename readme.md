# QAir / surveillance de la qualité de l'air

Stéphane Lepoutère 
(c)2021

12/2022 : reprise développement sur ESP32, nettoyage, encapsulation...

Objets en librairie :
- capteur environnemental (BME280 : t° / %HR / pression atmosphérique)
- capteur qualité d'air (SGP30 : eCO2 / TCOV)
- Afficheur OLed SSD1306
- Bandeau néopixel 8 leds
- Supervision de l'alimentation (batterie LiPo)
- WebServeur (site + websocket)

Sources :
Gestion du JSON sur ESP32 ([site ArduinoJson](https://arduinojson.org/))

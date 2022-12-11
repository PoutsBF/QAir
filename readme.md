# QAir / surveillance de la qualité de l'air

Stéphane Lepoutère
(c)2021

## 12/2022 reprise développement sur ESP32, nettoyage, encapsulation...

## A faire

- réglage du RTC à partir d'un serveur NTP
- Archivage des données dans une EEPROM type 24Cxx sur une structure à définir
- Affichage de courbes dans la page HTML à base de l'historique
- ...

## Objets en librairie

- capteur environnemental (BME280 t° / %HR / pression atmosphérique)
- capteur qualité d'air (SGP30 eCO2 / TCOV)
- Afficheur OLed SSD1306
- Bandeau néopixel 8 leds
- Supervision de l'alimentation (batterie LiPo)
- WebServeur (site + websocket)

## Sources

### FreeRTOS

- [FreeRTOS](https://www.freertos.org/index.html)

### La gestion du webserveur par le proejet ESPHome

- [Dépot Github](https://github.com/esphome/ESPAsyncWebServer)
- [Exemple simple](https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/)

### Divers

- Gestion du JSON sur ESP32 ([site ArduinoJson](https://arduinojson.org/))
- Modèle HTML ([site BootStrap](https://getbootstrap.com/))
- Référence pour le HTML, JavaScript, CSS, etc. ([lien Mozilla](https://developer.mozilla.org/fr/))

### Sur les batteries

- [site pobot](https://pobot.org/Les-batteries-Li-Ion-et-Li-PO.html)

### Sur le NTP

- [NTP sur ESP32](https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/)

### Gestion de l'ADC

- [Notice sur le site Espressif](https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html)
- [l'ADC sur ESP32](https://microcontrollerslab.com/adc-esp32-measuring-voltage-example/)
- [Calibration de l'ADC de l'ESP32](https://github.com/e-tinkers/esp32-adc-calibrate)


# QAir / surveillance de la qualité de l'air

[Stéphane Lepoutère \(linkedIn\)](https://www.linkedin.com/in/st%C3%A9phane-lepout%C3%A8re-403a01b1/)
(c)2021

## 12/2022 reprise développement sur ESP32, nettoyage, encapsulation...

Mon objectif : travailler l'encapsulation, l'utilisation des templates, les tâches et sémaphores...

## A faire

- réglage du RTC à partir d'un serveur NTP
- [x] connexion serveur NTP
- [ ] Programmation RTC, lecture RTC, client NTP au démarrage puis suppression
- [ ] changement orientation : tâche NTC lancée une fois au démarrage
- Gestion des charges batteries basses (sleep, bilan énergétique...)
- [x] passage en deepSleep si VCC \< \(3,3V + Vdpo 0,3V \)
- [ ] Bilan énergétique
- [ ] Essai passage en lightSleep \(sans redémarrage)
- Archivage des données dans une EEPROM type 24Cxx sur une structure à définir
- Affichage de courbes dans la page HTML à base de l'historique
- Capteur de particules
- ...

## Objets en librairie


* capteur environnemental (BME280 t° / %HR / pression atmosphérique)
    * tâche à mettre en oeuvre
* capteur qualité d'air (SGP30 eCO2 / TCOV)
    * tâche à mettre en oeuvre
* Afficheur OLed SSD1306
* Bandeau néopixel 8 leds
    * géré par une tâche FreeRTOS
* Supervision de l'alimentation (batterie LiPo)
    * géré par une tâche FreeRTOS
* WebServeur (site + websocket)

## Capteurs & cartes

### Carte HUZZAH32 Feather (Adafruit)
| |
|:--:|
|![Huzzah32](/doc/images/feather.jpg)|
|[site adafruit](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather)|

### BME280
| |
|:--:|
|![BME280](/doc/images/BME280.jpg)|
|[site seeed](https://wiki.seeedstudio.com/Grove-Barometer_Sensor-BME280/)|

### SGP30
| |
|:--:|
|![SGP30](/doc/images/SGP30.jpg)|
|[site seeed](https://wiki.seeedstudio.com/Grove-VOC_and_eCO2_Gas_Sensor-SGP30/)|

### HM3301 qualité de l'air
| |
|:--:|
|![HM3301](/doc/images/HM3301.jpg)|
|[site seeed](https://wiki.seeedstudio.com/Grove-Laser_PM2.5_Sensor-HM3301/)|

## Sources

### FreeRTOS

- [FreeRTOS](https://www.freertos.org/index.html)
- [Notions](http://tvaira.free.fr/esp32/esp32-freertos.html)

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

## Résultat (site responsive)

![Capture horizontale](/doc/images/Capture%20d%E2%80%99%C3%A9cran%202022-12-15%20%C3%A0%2018.30.48.png)

|haut|bas|
|----|---|
|![capture verticale 1/2](/doc/images/Capture%20d%E2%80%99%C3%A9cran%202022-12-15%20%C3%A0%2018.31.22.png)|![capture verticale 2/2](/doc/images/Capture%20d%E2%80%99%C3%A9cran%202022-12-15%20%C3%A0%2018.31.05.png)|

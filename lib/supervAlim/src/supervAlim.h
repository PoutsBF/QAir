/******************************************************************************
Librairie pour la supervision de l'alimentation

******************************************************************************/

#include<Arduino.h>

class SupervAlim
{
private:
    unsigned long delayTime;        // délai entre les mesures
    unsigned long lastTime;          // Timer pour les délais entre mesures

    uint8_t _valeur;                 // Niveau en %

public:
    SupervAlim(/* args */);
    ~SupervAlim();

    void init(ulong delay);
    uint8_t lecture();

    uint8_t valeur();
};


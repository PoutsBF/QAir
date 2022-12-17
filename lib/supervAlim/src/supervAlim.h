/******************************************************************************
Librairie pour la supervision de l'alimentation
    Lors des mesures de tension de batterie, si la tension est inférieure à
    3,3V (tension d'alimentation) + 0,3V de drop out max @ 600mA

******************************************************************************/

#include <Arduino.h>

#include <variableShared.h>

// Entrée A13-I35 : ADC1_CH7 – GPIO35
#define VBAT A13

class SupervAlim
{
private:
    unsigned long delayTime;        // délai entre les mesures
    uint8_t chgt;                   // Indicateur du changement de valeur

    VariableShared<float> * _valeur;    // Tension en volt
    VariableShared<uint8_t> * _niveau;  // Niveau en %

    TaskHandle_t id_tache;
    static void tacheMAJ(void *pvParameter);

public:
    /// @brief non utilisé
    SupervAlim();
    /// @brief non utilisé
    ~SupervAlim();

    /// @brief Initialise l'objet de supervision de la batterie
    /// @param _delayTime : délai entre chaque mesure
    void init(ulong delay);

    /// @brief Fonction appelée régulièrement pour évaluer la tension
    /// TODO : transformer en task avec un delay entre chaques lectures
    /// @return vrai si une lecture est effectuée, faux sinon.
    uint8_t lecture();

    /// @brief Interroge le niveau de charge en %
    /// @return valeur entre 0 et 100%
    uint8_t niveau();

    /// @brief Interroge la tension de la batterie en Volt
    /// @return valeur en volt
    float valeur();
};


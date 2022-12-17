#pragma once
/******************************************************************************
Librairie pour la gestion d'une variable partagée entre tâches

******************************************************************************/
#include <Arduino.h>

template <typename T>
class VariableShared
{
private:
    T variable;
    xSemaphoreHandle xMutex;

public:
    VariableShared(T _valeur);
    ~VariableShared();

    T get(void);
    void set(T);
};

template <typename T>
VariableShared<T>::VariableShared(T _valeur)
{
    variable = _valeur;
    xMutex = xSemaphoreCreateMutex ( ) ;
}

template <typename T>
VariableShared<T>::~VariableShared()
{
}

template <typename T>
inline T VariableShared<T>::get(void)
{
    return variable;
}

template <typename T>
inline void VariableShared<T>::set(T _valeur)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    variable = _valeur;
    xSemaphoreGive(xMutex);
}

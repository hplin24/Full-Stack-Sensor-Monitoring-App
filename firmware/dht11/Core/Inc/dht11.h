#ifndef DHT11_H_
#define DHT11_H_

#include "main.h"
// #include "stm32f4xx_hal.h"

typedef struct Dht11Cfg
{
	GPIO_TypeDef* port;
	uint16_t pin;
	TIM_HandleTypeDef* htim;
} Dht11Cfg;

typedef struct Dht11Data
{
    uint8_t temp_int;
    uint8_t temp_frac;
    uint8_t rh_int;
    uint8_t rh_frac;
    uint8_t checksum;
} Dht11Data;

typedef enum Dht11GPIOMode
{
	DHT11_GPIO_INPUT,
	DHT11_GPIO_OUTPUT
} Dht11GPIOMode;

static const uint8_t DHT11_DATA_SIZE = 5;

void DelayMicroseconds(TIM_HandleTypeDef* htim, uint16_t milliseconds);
void SetDht11GPIOMode(Dht11Cfg* cfg, Dht11GPIOMode mode);
void ReadDht11(Dht11Cfg* cfg, Dht11Data* outData);

#endif

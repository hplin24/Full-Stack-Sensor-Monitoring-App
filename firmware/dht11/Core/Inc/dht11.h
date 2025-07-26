#ifndef DHT11_H_
#define DHT11_H_

#include "main.h"

typedef struct DHT11Obj
{
	GPIO_TypeDef* port;
	uint16_t pin;
	TIM_HandleTypeDef* htim;

	uint8_t temp;
	uint8_t humidity;
	uint8_t chksum;

} DHT11Obj;

void DelayUntil(TIM_HandleTypeDef* htim, uint16_t us);
void InitDHT11Obj(DHT11Obj* dht11Obj, GPIO_TypeDef* port, uint16_t pin, TIM_HandleTypeDef* htim);
void SetDHT11GPIOToInputMode(DHT11Obj* dht11Obj);
void SetDHT11GPIOToOutputMode(DHT11Obj* dht11Obj);
void ReadDHT11(DHT11Obj* dht11Obj);

#endif

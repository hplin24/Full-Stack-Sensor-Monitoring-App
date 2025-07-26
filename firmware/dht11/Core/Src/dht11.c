#include "dht11.h"

void DelayUntil(TIM_HandleTypeDef* htim, uint16_t us)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
	while (__HAL_TIM_GET_COUNTER(htim) < us);
}

void InitDHT11Obj(DHT11Obj* dht11Obj, GPIO_TypeDef* port, uint16_t pin, TIM_HandleTypeDef* htim)
{
	dht11Obj->port = port;
	dht11Obj->pin = pin;
	dht11Obj->htim = htim;
	dht11Obj->temp = 0;
	dht11Obj->humidity = 0;
	dht11Obj->chksum = 0;
}

void SetDHT11GPIOToInputMode(DHT11Obj* dht11Obj)
{
	GPIO_InitTypeDef gpioInitTypeDef = { 0 };
	gpioInitTypeDef.Pin = dht11Obj->pin;
	gpioInitTypeDef.Mode = GPIO_MODE_INPUT;
	gpioInitTypeDef.Pull = GPIO_NOPULL;
	gpioInitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(dht11Obj->port, &gpioInitTypeDef);
}

void SetDHT11GPIOToOutputMode(DHT11Obj* dht11Obj)
{
	GPIO_InitTypeDef gpioInitTypeDef = { 0 };
	gpioInitTypeDef.Pin = dht11Obj->pin;
	gpioInitTypeDef.Mode = GPIO_MODE_OUTPUT_PP;
	gpioInitTypeDef.Pull = GPIO_NOPULL;
	gpioInitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(dht11Obj->port, &gpioInitTypeDef);
}

void ReadDHT11(DHT11Obj* dht11Obj)
{
	SetDHT11GPIOToOutputMode(dht11Obj);

	// MCU Sends out Start Signal (low-voltage-level) to DHT
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	DelayUntil(dht11Obj->htim, 18000);

	SetDHT11GPIOToInputMode(dht11Obj);

	// MCU pulls up voltage and wait 20-40us for DHT's response
	DelayUntil(dht11Obj->htim, 40);
	if (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_RESET) {
		// DHT detects the start signal then send out a low-voltage-level response signal, which lasts 80us.
		DelayUntil(dht11Obj->htim, 80);
	}
	while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin)) {
		// Wait until DHT sends out high-voltage-level and keeps it for 80us
	}

	// DHT sends out data

	// Humidity integral
	uint8_t humidityInt = 0;
	for (int i = 0; i < 8; i++) {
		while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_RESET) {
			// Wait until the 50us before data transmit
		}
		// Now DGT is sending data
		DelayUntil(dht11Obj->htim, 40);
		if (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
			// 26-28us is 0, 70us is 1
			humidityInt |= (1 << (7 - i)); // 1
			while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
				// Wait until 70us high signal pass
			}
		}
	}

	// Humidity decimal
	uint8_t humidityDec = 0;
	for (int i = 0; i < 8; i++) {
		while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_RESET) {
			// Wait until the 50us before data transmit
		}
		// Now dht11 is sending data
		DelayUntil(dht11Obj->htim, 40);
		if (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
			// 26-28us is 0, 70us is 1
			humidityDec |= (1 << (7 - i)); // 1
			while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
				// Wait until 70us high signal pass
			}
		}
	}
	// Temperature integral
	uint8_t temperatureInt = 0;
	for (int i = 0; i < 8; i++) {
		while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_RESET) {
			// Wait until the 50us before data transmit
		}
		// Now dht11 is sending data
		DelayUntil(dht11Obj->htim, 40);
		if (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
			// 26-28us is 0, 70us is 1
			temperatureInt |= (1 << (7 - i)); // 1
			while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
				// Wait until 70us high signal pass
			}
		}
	}
	// Temperature decimal
	uint8_t temperatureDec = 0;
	for (int i = 0; i < 8; i++) {
		while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_RESET) {
			// Wait until the 50us before data transmit
		}
		// Now dht11 is sending data
		DelayUntil(dht11Obj->htim, 40);
		if (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
			// 26-28us is 0, 70us is 1
			temperatureDec |= (1 << (7 - i)); // 1
			while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
				// Wait until 70us high signal pass
			}
		}
	}
	// Checksum
	uint8_t chksum = 0;
	for (int i = 0; i < 8; i++) {
		while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_RESET) {
			// Wait until the 50us before data transmit
		}
		// Now dht11 is sending data
		DelayUntil(dht11Obj->htim, 40);
		if (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
			// 26-28us is 0, 70us is 1
			chksum |= (1 << (7 - i)); // 1
			while (HAL_GPIO_ReadPin(dht11Obj->port, dht11Obj->pin) == GPIO_PIN_SET) {
				// Wait until 70us high signal pass
			}
		}
	}

	dht11Obj->temp = temperatureInt;
	dht11Obj->humidity = humidityInt;
	dht11Obj->chksum = temperatureInt + humidityInt;
}

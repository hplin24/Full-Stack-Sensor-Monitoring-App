#include "dht11.h"

void DelayMicroseconds(TIM_HandleTypeDef* htim, uint16_t milliseconds)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
	while (__HAL_TIM_GET_COUNTER(htim) < milliseconds);
}

void SetDht11GPIOMode(Dht11Cfg* cfg, Dht11GPIOMode mode)
{
	GPIO_InitTypeDef gpioInitTypeDef = { 0 };
	gpioInitTypeDef.Pin = cfg->pin;
	gpioInitTypeDef.Pull = GPIO_NOPULL;
	gpioInitTypeDef.Mode = (mode == DHT11_GPIO_INPUT) ? GPIO_MODE_INPUT : DHT11_GPIO_OUTPUT;
	gpioInitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(cfg->port, &gpioInitTypeDef);
}

void ReadDht11(Dht11Cfg* cfg, Dht11Data* outData)
{
	SetDht11GPIOMode(cfg, DHT11_GPIO_OUTPUT);

	// Start signal. MCU->DHT. Pull data line low for at least 18ms
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	DelayMicroseconds(cfg->htim, 18000);

	// MCU releases line when mode changes
	SetDht11GPIOMode(cfg, DHT11_GPIO_INPUT);

	// Prepare to read. MCU->DHT. Release line, wait 20–40us
	DelayMicroseconds(cfg->htim, 40);

	// Sensor presence response. DHT->MCU. Responds with 80us low + 80us high
	if (HAL_GPIO_ReadPin(cfg->port, cfg->pin) == GPIO_PIN_RESET)
	{
		// 80us low
		DelayMicroseconds(cfg->htim, 80);
	}
	while (HAL_GPIO_ReadPin(cfg->port, cfg->pin))
	{
		// 80us high
	}

	// DHT->MCU. Sends 40 bits of data (humidity + temp + checksum)
	// 26-28us high means bit '0'
	// 70us high means bit '1'
	// Wait 50us, if it is still high, it is a bit '1', otherwise, bit '0'

	uint8_t rh_int = 0;
	uint8_t rh_frac = 0;
	uint8_t temp_int = 0;
	uint8_t temp_frac = 0;
	uint8_t checksum = 0;
	for (int i = 0; i < 40; i++)
	{
		while (HAL_GPIO_ReadPin(cfg->port, cfg->pin) == GPIO_PIN_RESET)
		{
			// Wait 50us in low for the start of bit
		}
		// Wait 50us to know whether it is a bit '1' or '0'
		DelayMicroseconds(cfg->htim, 50);
		if (HAL_GPIO_ReadPin(cfg->port, cfg->pin) == GPIO_PIN_SET)
		{
			// It is a bit '1'

			// index 0-7   is Integral humidity
			// index 8-15  is Decimal  humidity
			// index 16-23 is Integral temperature
			// index 24-31 is Decimal  temperature
			// index 32-40 is checksum (sum of bytes above)

			if      (i >= 0 && i < 8)   rh_int    |= (1 << (7 - i));
			else if (i >= 8 && i < 16)  rh_frac   |= (1 << (7 - (i - 8)));
			else if (i >= 16 && i < 24) temp_int  |= (1 << (7 - (i - 16)));
			else if (i >= 24 && i < 32) temp_frac |= (1 << (7 - (i - 24)));
			else if (i >= 32 && i < 40) checksum  |= (1 << (7 - (i - 32)));

			while (HAL_GPIO_ReadPin(cfg->port, cfg->pin) == GPIO_PIN_SET)
			{
				// Wait until the bit transmission ends
			}
		}
	}

	outData->rh_int = rh_int;
	outData->rh_frac = rh_frac;
	outData->temp_int = temp_int;
	outData->temp_frac = temp_frac;
	outData->checksum = checksum;
}

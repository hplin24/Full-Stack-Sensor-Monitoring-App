#include "DHT11Reader.h"
#include "Logger.h"
#include <iostream>
#include <Windows.h>

int main()
{
	utils::Logger logger(L"main");
	DHT11Reader dht11;

	if (!dht11.IsComPortOk())
	{
		logger.Log(L"dht11 com port is not ok");
		return -1;
	}

	while (1)
	{
		DHT11Data dht11Data;
		dht11Data.temp = 0;
		dht11Data.rh = 0;

		if (!dht11.RetrieveData(dht11Data))
		{
			logger.Log(L"retrieve data failed");
			return -1;
		}

		logger.Log(L"from main: temp=" +
			std::to_wstring(dht11Data.temp) + L", rh=" + std::to_wstring(dht11Data.rh));

		Sleep(10000);
	}


	return 0;
}
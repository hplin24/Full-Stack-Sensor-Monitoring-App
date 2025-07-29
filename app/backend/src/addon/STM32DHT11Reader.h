/**
* 
* STM32DHT11Reader.h
* 
* This header defines the STM32DHT11Reader class for retrieving temperature and humidity data through a
* COM port from an STM32 board connected to a DHT11 sensor
*
* */

#pragma once

#include <Windows.h>
#include <string>
#include "Logger.h"

enum class EC
{
	None = 0,
	Ok,
	STLinkPortNotFound,
	OpenComPortFailed,
	GetCommStateFailed,
	SetCommStateFailed,
	ComPortNotInitialized,
	WriteToStm32Failed,
	ReadFromStm32Failed,
	ChecksumFailed
};

std::string PrintErrorString(EC ec);

class STM32DHT11Reader
{
public:
	STM32DHT11Reader();
	~STM32DHT11Reader();
	STM32DHT11Reader(const STM32DHT11Reader&) = delete;
	STM32DHT11Reader& operator=(const STM32DHT11Reader&) = delete;
     
	EC Init();
	EC ReInit();
	bool IsInit() const;
	EC UpdateData();
	int GetTemperature() const;
	int GetHumidity() const;

private:
	HANDLE m_hCom;

	int m_rh_int;
	int m_rh_frac;
	int m_temp_int;
	int m_temp_frac;

	Logger m_logger;
};

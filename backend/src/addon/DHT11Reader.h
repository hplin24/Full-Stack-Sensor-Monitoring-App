#pragma once

#include <Windows.h>
#include <string>
#include "Logger.h"
#include <memory>

enum class ErrorCode
{
	None = 0,
	OpenComPortFailed,
	GetCommStateFailed,
	SetCommStateFailed,
	ComPortNotInitialized,
	WriteToStm32Failed,
	ReadFromStm32Failed,
	IncorrectBufferFormat,
	ChecksumFailed,
	Ok
};

struct DHT11Data
{
	unsigned char temp;
	unsigned char rh;
};

std::string ErrorToString(ErrorCode code);

class DHT11Reader
{
public:
	DHT11Reader();
	~DHT11Reader();
	DHT11Reader(const DHT11Reader&) = delete;
	DHT11Reader& operator=(const DHT11Reader&) = delete;
    
	ErrorCode Init();
	bool IsInit() const;
	ErrorCode ReInit();
	ErrorCode GetData(DHT11Data& data);

private:
	HANDLE m_hCom;
	const unsigned int DATA_BUFFER_SIZE = 10;
    std::unique_ptr<Logger> m_upLogger;
};

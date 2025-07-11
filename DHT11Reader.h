#pragma once

#include <Windows.h>
#include <string>

struct DHT11Data
{
	unsigned char temp;
	unsigned char rh;
};

class DHT11Reader
{
public:
	DHT11Reader();
	~DHT11Reader();
	DHT11Reader(const DHT11Reader&) = delete;
	DHT11Reader& operator=(const DHT11Reader&) = delete;

	bool IsComPortOk() const;
	bool ReInitComPort();
	bool RetrieveData(DHT11Data& data);

private:
	HANDLE m_hCom;
	const unsigned int DATA_BUFFER_SIZE = 10;

	bool InitComPort();
};

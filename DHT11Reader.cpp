#include "DHT11Reader.h"
#include <iostream>


DHT11Reader::DHT11Reader() : m_hCom(INVALID_HANDLE_VALUE)
{
	InitComPort();
}

DHT11Reader::~DHT11Reader()
{
	CloseHandle(m_hCom);
}

bool DHT11Reader::IsComPortOk() const
{
	return m_hCom != INVALID_HANDLE_VALUE;
}

bool DHT11Reader::ReInitComPort()
{
	//PurgeComm(m_hCom, PURGE_RXCLEAR);
	CloseHandle(m_hCom);
	return InitComPort();
}

bool DHT11Reader::RetrieveData(DHT11Data& data)
{
	if (!IsComPortOk())
	{
		std::cout << "IsComPortOk(): false" << std::endl;
		return false;
	}
		

	unsigned char sig = 0xAA;
	if (!WriteFile(m_hCom, &sig, 1, NULL, NULL))
	{
		std::cout << "write file to stm32 failed: GetLastError()= " << GetLastError() << std::endl;
		return false;
	}

	const int DATA_LENGTH = 3;
	unsigned char buffer[DATA_LENGTH];
	memset(buffer, 0, DATA_LENGTH);

	//std::string buffer(DATA_LENGTH, 0);
	DWORD dwByteRead{};

	if (!ReadFile(m_hCom, &buffer[0], DATA_LENGTH, &dwByteRead, NULL))
	{
		std::cout << "readfile failed" << std::endl;
		return false;
	}


	std::cout << "buffer[0](temp) = " << buffer[0] << std::endl;
	std::cout << "buffer[1](delimieter) = " << buffer[1] << std::endl;
	std::cout << "buffer[2](rh) = " << buffer[2] << std::endl;

	if (buffer[1] != 0xAA)
	{
		std::cout << "buffer delim is not 0xAA" << std::endl;
		return false;
	}

	data.temp = buffer[0];
	data.rh = buffer[2];
		
	//buffer.resize(dwByteRead);
	//std::cout << "received: " << buffer << std::endl;
	//const size_t dataStartPos = buffer.find_first_of(';');
	//const size_t dataEndPos = buffer.find_last_of(';');
	//if (dataStartPos == buffer.npos || dataEndPos == buffer.npos)
	//{
	//	std::cout << "data received doesn't contain correct num of ;" << std::endl;
	//	return false;
	//}
	//const std::string dataStr = buffer.substr(dataStartPos + 1, dataEndPos - dataStartPos + 1);
	//const size_t commaPos = dataStr.find_last_of(',');

	//if (commaPos == dataStr.npos)
	//{
	//	std::cout << "data received doesn't contain ," << std::endl;
	//	return false;
	//}

	//data.temp = std::stoi(buffer.substr(0, commaPos));
	//data.rh = std::stoi(buffer.substr(commaPos + 1));

	//std::cout << "data.temp = " << data.temp << std::endl;
	//std::cout << "data.rh = " << data.rh << std::endl;

	return true;
}


bool DHT11Reader::InitComPort()
{
	m_hCom = CreateFile(
		L"\\\\.\\COM3",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		std::cout << "hCom init failed";
		return false;
	}

	DCB dcb;
	//  Initialize the DCB structure.
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(m_hCom, &dcb))
	{
		//  Handle the error.
		std::cout << "GetCommState failed with error: " << GetLastError() << std::endl;
		return false;
	}

	dcb.BaudRate = CBR_9600;     //  baud rate
	dcb.ByteSize = 8;             //  data size, xmit and rcv
	dcb.Parity = NOPARITY;      //  parity bit
	dcb.StopBits = ONESTOPBIT;    //  stop bit

	if (!SetCommState(m_hCom, &dcb))
	{
		//  Handle the error.
		std::cout << "SetCommState failed with error: " << GetLastError() << std::endl;
		return false;
	}

	return true;
}
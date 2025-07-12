#include "DHT11Reader.h"
//#include <iostream>

namespace
{
	const std::wstring TRANSMIT_COM_PORT = L"\\\\.\\COM3";
	const DWORD TRANSMIT_BAUD_RATE = CBR_9600;
	const size_t RECV_DATA_SIZE = 4; // [ temp, rh, chksum, delimiter ]
	const unsigned char REQ_SIGNAL = 0xAA;
}

std::string ErrorToString(ErrorCode code)
{
	switch (code) {
	case ErrorCode::None:					return "None";
	case ErrorCode::OpenComportFailed:		return "OpenComportFailed";
	case ErrorCode::GetCommStateFailed:		return "GetCommStateFailed";
	case ErrorCode::SetCommStateFailed:		return "SetCommStateFailed";
	case ErrorCode::ComportNotInitialized:	return "ComportNotInitialized";
	case ErrorCode::WriteToStm32Failed:		return "WriteToStm32Failed";
	case ErrorCode::ReadFromStm32Failed:	return "ReadFromStm32Failed";
	case ErrorCode::IncorrectBufferFormat:	return "IncorrectBufferFormat";
	case ErrorCode::ChecksumFailed:			return "ChecksumFailed";
	case ErrorCode::Ok:						return "Ok";
	default:								return "UnknownErrorCode";
	}
}

DHT11Reader::DHT11Reader() : m_hCom(INVALID_HANDLE_VALUE)
{
    m_upLogger = std::make_unique<Logger>(L"dht11reader");
}

DHT11Reader::~DHT11Reader()
{
	CloseHandle(m_hCom);
}

bool DHT11Reader::IsComPortOk() const
{
	return m_hCom != INVALID_HANDLE_VALUE;
}

ErrorCode DHT11Reader::ReInitComPort()
{
	CloseHandle(m_hCom);
	return InitComPort();
}

ErrorCode DHT11Reader::RetrieveData(DHT11Data& data)
{
    m_upLogger->Log(L"RetrieveData called");

	if (!IsComPortOk())
	{
        m_upLogger->Log(L"IsComPortOk return false");
		return ErrorCode::ComportNotInitialized;
	}

    m_upLogger->Log(L"Writing signal to STM32");

	DWORD dwWrittenByte{};
	if (!WriteFile(m_hCom, &REQ_SIGNAL, 1, &dwWrittenByte, NULL))
	{
        m_upLogger->Log(L"WriteFile failed: " + std::to_wstring(GetLastError()));
		return ErrorCode::ReadFromStm32Failed;
	}

    m_upLogger->Log(L"WriteFile Ok");

	//std::cout << "byte written: " << dwWrittenByte << std::endl;
	unsigned char buffer[RECV_DATA_SIZE];
	memset(buffer, 0, RECV_DATA_SIZE);

	//std::string buffer(DATA_LENGTH, 0);
	DWORD dwByteRead{};

	//std::cout << "waiting for stm32 response..." << std::endl;

    m_upLogger->Log(L"Waiting data from STM32 ...");

	if (!ReadFile(m_hCom, &buffer[0], RECV_DATA_SIZE, &dwByteRead, NULL))
	{
        m_upLogger->Log(L"ReadFile failed: " + std::to_wstring(GetLastError()));
		return ErrorCode::ReadFromStm32Failed;
	}

	//std::cout << "buffer[0](temp) = " << (unsigned int)buffer[0] << std::endl;
	//std::cout << "buffer[1](rh) = " << (unsigned int)buffer[1] << std::endl;
	//std::cout << "buffer[2](chksum) = " << (unsigned int)buffer[2] << std::endl;
	//std::cout << "buffer[3](delimieter) = " << (unsigned int)buffer[3] << std::endl;

	if (buffer[3] != 0xAA)
	{
        m_upLogger->Log(L"buffer not end with 0xAA");
		//std::cout << "buffer delim is not 0xAA" << std::endl;
		return ErrorCode::IncorrectBufferFormat;
	}
	if (buffer[2] != buffer[0] + buffer[1])
	{
        m_upLogger->Log(L"buffer checksum failed");
		//std::cout << "buffer check sum failed" << std::endl;
		return ErrorCode::ChecksumFailed;
	}

	data.temp = buffer[0];
	data.rh = buffer[1];

	return ErrorCode::Ok;
}

ErrorCode DHT11Reader::InitComPort()
{
    m_upLogger->Log(L"Opening COM port");

	m_hCom = CreateFileW(
		TRANSMIT_COM_PORT.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (m_hCom == INVALID_HANDLE_VALUE)
	{
        m_upLogger->Log(L"Opened COM port failed: " + std::to_wstring(GetLastError()));
		//std::cout << "hCom init failed";
		return ErrorCode::OpenComportFailed;
	}

	DCB dcb;
	//  Initialize the DCB structure.
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(m_hCom, &dcb))
	{
        m_upLogger->Log(L"GetCommState failed: " + std::to_wstring(GetLastError()));
		//std::cout << "GetCommState failed with error: " << GetLastError() << std::endl;
		return ErrorCode::GetCommStateFailed;
	}

    m_upLogger->Log(L"GetCommState Ok");

	dcb.BaudRate = TRANSMIT_BAUD_RATE; // baud rate
	dcb.ByteSize = 8;                  // data size, xmit and rcv
	dcb.Parity = NOPARITY;             // parity bit
	dcb.StopBits = ONESTOPBIT;         // stop bit

	if (!SetCommState(m_hCom, &dcb))
	{
        m_upLogger->Log(L"SetCommState failed: " + std::to_wstring(GetLastError()));
		//std::cout << "SetCommState failed with error: " << GetLastError() << std::endl;
		return ErrorCode::SetCommStateFailed;
	}

    m_upLogger->Log(L"SetCommState Ok");
    m_upLogger->Log(L"InitComPort Ok");

	return ErrorCode::Ok;
}
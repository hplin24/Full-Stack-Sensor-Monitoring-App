#include "DHT11Reader.h"
//#include <iostream>

namespace
{
	const std::wstring TRANSMIT_COM_PORT = L"\\\\.\\COM3";
	const DWORD TRANSMIT_BAUD_RATE = CBR_9600;
	const size_t RECV_DATA_SIZE = 4; // [ temp, rh, chksum, delimiter ]
	const unsigned char REQ_SIGNAL = 0xAA;
	const std::wstring LOGGER_ID = L"dht11";
}

std::string ErrorToString(ErrorCode code)
{
	switch (code) {
	case ErrorCode::None:					return "None";
	case ErrorCode::OpenComPortFailed:		return "OpenComPortFailed";
	case ErrorCode::GetCommStateFailed:		return "GetCommStateFailed";
	case ErrorCode::SetCommStateFailed:		return "SetCommStateFailed";
	case ErrorCode::ComPortNotInitialized:	return "ComPortNotInitialized";
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
    m_upLogger = std::make_unique<Logger>(LOGGER_ID);
}

DHT11Reader::~DHT11Reader()
{
	CloseHandle(m_hCom);
}

bool DHT11Reader::IsInit() const
{
	m_upLogger->Log(L"[IsInit()] CALLED");
	return m_hCom != INVALID_HANDLE_VALUE;
}

ErrorCode DHT11Reader::ReInit()
{
	m_upLogger->Log(L"[ReInit()] CALLED");
	CloseHandle(m_hCom);
	m_upLogger->Log(L"[ReInit()] Initializing ...");
	return Init();
}

ErrorCode DHT11Reader::GetData(DHT11Data& data)
{
    m_upLogger->Log(L"[GetData()] CALLED");

	if (!IsInit())
	{
        m_upLogger->Log(L"[GetData()] error: isInit return false");
		return ErrorCode::ComPortNotInitialized;
	}

	m_upLogger->Log(L"[GetData()] isInit return true");

    m_upLogger->Log(L"[GetData()] Writing starting signal to STM32");

	DWORD dwWrittenByte{};
	if (!WriteFile(m_hCom, &REQ_SIGNAL, 1, &dwWrittenByte, NULL))
	{
        m_upLogger->Log(L"[GetData()] error: WriteFile failed: " + std::to_wstring(GetLastError()));
		return ErrorCode::ReadFromStm32Failed;
	}

    m_upLogger->Log(L"[GetData()] WriteFile successfully");

	//std::cout << "byte written: " << dwWrittenByte << std::endl;
	unsigned char buffer[RECV_DATA_SIZE];
	memset(buffer, 0, RECV_DATA_SIZE);

	//std::string buffer(DATA_LENGTH, 0);
	DWORD dwByteRead{};

	//std::cout << "waiting for stm32 response..." << std::endl;

    m_upLogger->Log(L"[GetData()] Waiting for response from STM32 ...");

	if (!ReadFile(m_hCom, &buffer[0], RECV_DATA_SIZE, &dwByteRead, NULL))
	{
        m_upLogger->Log(L"[GetData()] error: ReadFile failed: " + std::to_wstring(GetLastError()));
		return ErrorCode::ReadFromStm32Failed;
	}

	//std::cout << "buffer[0](temp) = " << (unsigned int)buffer[0] << std::endl;
	//std::cout << "buffer[1](rh) = " << (unsigned int)buffer[1] << std::endl;
	//std::cout << "buffer[2](chksum) = " << (unsigned int)buffer[2] << std::endl;
	//std::cout << "buffer[3](delimieter) = " << (unsigned int)buffer[3] << std::endl;

	m_upLogger->Log(L"[GetData()] buffer[0](temp) = " + std::to_wstring((unsigned int)buffer[0]));
	m_upLogger->Log(L"[GetData()] buffer[1](rh) = " + std::to_wstring((unsigned int)buffer[1]));
	m_upLogger->Log(L"[GetData()] buffer[2](chksum) = " + std::to_wstring((unsigned int)buffer[2]));
	m_upLogger->Log(L"[GetData()] buffer[3](delimieter) = " + std::to_wstring((unsigned int)buffer[3]));

	if (buffer[3] != 0xAA)
	{
        m_upLogger->Log(L"[GetData()] error: buffer not end with 0xAA");
		//std::cout << "buffer delim is not 0xAA" << std::endl;
		return ErrorCode::IncorrectBufferFormat;
	}
	if (buffer[2] != buffer[0] + buffer[1])
	{
        m_upLogger->Log(L"[GetData()] error: buffer checksum failed");
		//std::cout << "buffer check sum failed" << std::endl;
		return ErrorCode::ChecksumFailed;
	}

	data.temp = buffer[0];
	data.rh = buffer[1];

	m_upLogger->Log(L"[GetData()] END");

	return ErrorCode::Ok;
}

ErrorCode DHT11Reader::Init()
{
    m_upLogger->Log(L"[Init()] CALLED");

	m_upLogger->Log(L"[Init()] Opening COM port: " + TRANSMIT_COM_PORT);

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
        m_upLogger->Log(L"[Init()] Opened COM port failed: " + std::to_wstring(GetLastError()));
		//std::cout << "hCom init failed";
		return ErrorCode::OpenComPortFailed;
	}

	m_upLogger->Log(L"[Init()] open COM port successfully");

	DCB dcb;
	//  Initialize the DCB structure.
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(m_hCom, &dcb))
	{
        m_upLogger->Log(L"[Init()] error: GetCommState failed: " + std::to_wstring(GetLastError()));
		//std::cout << "GetCommState failed with error: " << GetLastError() << std::endl;
		return ErrorCode::GetCommStateFailed;
	}

    m_upLogger->Log(L"[Init()] GetCommState successfully");

	dcb.BaudRate = TRANSMIT_BAUD_RATE; // baud rate
	dcb.ByteSize = 8;                  // data size, xmit and rcv
	dcb.Parity = NOPARITY;             // parity bit
	dcb.StopBits = ONESTOPBIT;         // stop bit

	if (!SetCommState(m_hCom, &dcb))
	{
        m_upLogger->Log(L"[Init()]  error: SetCommState failed: " + std::to_wstring(GetLastError()));
		//std::cout << "SetCommState failed with error: " << GetLastError() << std::endl;
		return ErrorCode::SetCommStateFailed;
	}

    m_upLogger->Log(L"[Init()] SetCommState successfully");
    m_upLogger->Log(L"[Init()] Init successfully");
	m_upLogger->Log(L"[Init()] END");

	return ErrorCode::Ok;
}
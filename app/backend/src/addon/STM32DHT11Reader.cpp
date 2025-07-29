#include "STM32DHT11Reader.h"
#include "WMI.h"

/*
* Data received from the STM32 board:
* Totally 5 bytes;
*
* Byte
* |            1           |           2           |          3          |          4         |     5    |
* | temperature (integral) | temperature (decimal) | humidity (integral) | humidity (decimal) | checksum |
*
*/

namespace
{
    const std::wstring TARGET_COM_PORT_NAME = L"STLink Virtual COM Port";
    const DWORD BAUD_RATE = CBR_9600;
    const size_t RECV_DATA_SIZE = 5;
    const unsigned char REQ_SIGNAL = 0xAA;
    const std::wstring LOGGER_ID = L"stm32dht11";
}

std::string PrintErrorString(EC ec)
{
    switch (ec) {
    case EC::None:					return "None";
    case EC::Ok:					return "Ok";
    case EC::STLinkPortNotFound:    return "STLinkPortNotFound";
    case EC::OpenComPortFailed:		return "OpenComPortFailed";
    case EC::GetCommStateFailed:	return "GetCommStateFailed";
    case EC::SetCommStateFailed:	return "SetCommStateFailed";
    case EC::ComPortNotInitialized:	return "ComPortNotInitialized";
    case EC::WriteToStm32Failed:	return "WriteToStm32Failed";
    case EC::ReadFromStm32Failed:	return "ReadFromStm32Failed";
    case EC::ChecksumFailed:		return "ChecksumFailed";
    default:						return "UnknownErrorCode";
    }
}

STM32DHT11Reader::STM32DHT11Reader() :
    m_hCom(INVALID_HANDLE_VALUE),
    m_rh_int(-1),
    m_rh_frac(-1),
    m_temp_int(-1),
    m_temp_frac(-1),
    m_logger(LOGGER_ID)
{
    m_logger.StartLogging();
}

STM32DHT11Reader::~STM32DHT11Reader()
{
    m_logger.LogStr("STM32DHT11Reader dtor CALLED");
    CloseHandle(m_hCom);
}

EC STM32DHT11Reader::Init()
{
    m_logger.LogStr("[Init()] CALLED");

    // Get STLink COM port
    std::wstring port;
    if (!QueryWMI(L"Win32_SerialPort WHERE Caption LIKE '%STLink Virtual COM Port%'", L"DeviceID", port) || port.empty())
    {
        m_logger.LogStr("[Init()] Error: STLink COM port not found");
        return EC::STLinkPortNotFound;
    }
    m_logger.LogWstr(L"[Init()] STLink COM port detected: " + port);

    port = L"\\\\.\\" + port;
    m_hCom = CreateFileW(
        port.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (m_hCom == INVALID_HANDLE_VALUE)
    {
        m_logger.LogStr("[Init()] Error: Opening COM port failed (" + std::to_string(GetLastError()) + ")");
        return EC::OpenComPortFailed;
    }

    DCB dcb;
    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(m_hCom, &dcb))
    {
        m_logger.LogStr("[Init()] Error: Getting comm state failed (" + std::to_string(GetLastError()) + ")");
        return EC::GetCommStateFailed;
    }

    dcb.BaudRate = BAUD_RATE;  // baud rate
    dcb.ByteSize = 8;          // data size, xmit and rcv
    dcb.Parity = NOPARITY;     // parity bit
    dcb.StopBits = ONESTOPBIT; // stop bit

    if (!SetCommState(m_hCom, &dcb))
    {
        m_logger.LogStr("[Init()] Error: Set Comm state failed (" + std::to_string(GetLastError()) + ")");
        return EC::SetCommStateFailed;
    }

    m_logger.LogStr("[Init()] Init successfully");

    return EC::Ok;
}

EC STM32DHT11Reader::ReInit()
{
    m_logger.LogStr("[ReInit()] CALLED");
    if (m_hCom != INVALID_HANDLE_VALUE)
        CloseHandle(m_hCom);
    return Init();
}

bool STM32DHT11Reader::IsInit() const
{
    return m_hCom != INVALID_HANDLE_VALUE;
}

EC STM32DHT11Reader::UpdateData()
{
    m_logger.LogStr("[UpdateData()] CALLED");

    if (!IsInit())
    {
        m_logger.LogStr("[UpdateData()] Error: IsInit() return false");
        return EC::ComPortNotInitialized;
    }

    m_logger.LogStr("[UpdateData()] Writing starting signal to STM32 ...");

    DWORD dwWrittenByte = 0;
    if (!WriteFile(m_hCom, &REQ_SIGNAL, 1, &dwWrittenByte, NULL))
    {
        m_logger.LogStr("[UpdateData()] Error: WriteFile failed (" + std::to_string(GetLastError()) + ")");
        return EC::ReadFromStm32Failed;
    }

    m_logger.LogStr("[UpdateData()] Writing starting signal successfully");

    unsigned char buffer[RECV_DATA_SIZE];
    for (int i = 0; i < RECV_DATA_SIZE; i++)
    {
        buffer[i] = 0;
    }

    m_logger.LogStr("[UpdateData()] Waiting for response from STM32 ...");

    DWORD dwByteRead = 0;
    if (!ReadFile(m_hCom, &buffer[0], RECV_DATA_SIZE, &dwByteRead, NULL))
    {
        m_logger.LogStr("[UpdateData()] Error: ReadFile failed (" + std::to_string(GetLastError()) + ")");
        return EC::ReadFromStm32Failed;
    }

    m_logger.LogStr("[UpdateData()] buffer[0](rh_int) = " + std::to_string(static_cast<int>(buffer[0])));
    m_logger.LogStr("[UpdateData()] buffer[1](rh_frac) = " + std::to_string(static_cast<int>(buffer[1])));
    m_logger.LogStr("[UpdateData()] buffer[2](temp_int) = " + std::to_string(static_cast<int>(buffer[2])));
    m_logger.LogStr("[UpdateData()] buffer[3](temp_frac) = " + std::to_string(static_cast<int>(buffer[3])));
    m_logger.LogStr("[UpdateData()] buffer[4](checksum) = " + std::to_string(static_cast<int>(buffer[4])));


    if (buffer[4] != buffer[0] + buffer[1] + buffer[2] + buffer[3])
    {
        m_logger.LogStr("[UpdateData()] Error: The checksum value is incorrect");
        return EC::ChecksumFailed;
    }

    m_rh_int = buffer[0];
    m_rh_frac = buffer[1];
    m_temp_int = buffer[2];
    m_temp_frac = buffer[3];

    m_logger.LogStr("[UpdateData()] Temperature=" + std::to_string(m_temp_int));
    m_logger.LogStr("[UpdateData()] Humidity=" + std::to_string(m_rh_int));

    return EC::Ok;
}

int STM32DHT11Reader::GetTemperature() const
{
    return m_temp_int;
}

int STM32DHT11Reader::GetHumidity() const
{
    return m_rh_int;
}

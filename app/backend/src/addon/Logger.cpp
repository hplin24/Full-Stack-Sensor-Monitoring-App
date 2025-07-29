#include "Logger.h"
#include <codecvt>
#include <shlobj_core.h>

namespace
{
    std::string ConvertWStringToString(const std::wstring& ws)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(ws);
    }

    std::wstring GetLocalAppDataPath()
    {
        LPWSTR lpwPath;
        std::wstring path;
        if (SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &lpwPath) == S_OK)
            path = lpwPath;
        CoTaskMemFree(lpwPath);
        return path;
    }
}

Logger::Logger(const std::wstring& id) : m_hFile(INVALID_HANDLE_VALUE), m_id(id), m_wsLogPath(L"")
{

}

Logger::~Logger()
{
    if (m_hFile)
    {
        LogFooter();
        CloseHandle(m_hFile);
    }
}

bool Logger::StartLogging()
{
    if (m_hFile != INVALID_HANDLE_VALUE)
        return true;

    m_wsLogPath = GetLocalAppDataPath() + L"\\MyCustomLogs\\";

    if (!CreateDirectoryW(m_wsLogPath.c_str(), NULL))
    {
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            // Create directory error
            return false;
        }
    }

    m_wsLogPath += m_id + L"_log.txt";

    m_hFile = CreateFileW(
        m_wsLogPath.c_str(),
        FILE_ALL_ACCESS,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
        NULL);

    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        LogHeader();
    }

    return m_hFile != INVALID_HANDLE_VALUE;
}

bool Logger::IsLoggerOk() const
{
    return m_hFile != INVALID_HANDLE_VALUE;
}

void Logger::LogWstr(const std::wstring& ws)
{
    WriteLog(ConvertWStringToString(ws));
}

void Logger::LogStr(const std::string& s)
{
    WriteLog(s);
}

std::wstring Logger::GetLogPath() const
{
    return m_wsLogPath;
}

std::string Logger::GetTimestamp() const
{
    SYSTEMTIME SysTime;
    GetLocalTime(&SysTime);

    int Year = SysTime.wYear;
    int Month = SysTime.wMonth;
    int Day = SysTime.wDay;
    int Hour = SysTime.wHour;
    int Minute = SysTime.wMinute;
    int Second = SysTime.wSecond;
    //int Milliseconds = SysTime.wMilliseconds;

    std::wstring timestamp;
    timestamp.resize(50, 0);

    int len = swprintf_s(
        &timestamp.front(),
        timestamp.size(),
        L"%d-%02d-%02d-%02d:%02d:%02d",
        Year, Month, Day, Hour, Minute, Second);

    if (len == -1)
    {
        timestamp = L"0000-00-00-000000";
    }
    else
    {
        timestamp.resize(len);
    }
    return ConvertWStringToString(timestamp);
}

void Logger::WriteLog(const std::string& s)
{
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        std::string sLog = "[" + GetTimestamp() + "] ";
        sLog += s + "\n";
        DWORD bytesWritten = 0;
        WriteFile(m_hFile, sLog.c_str(), sLog.size(), &bytesWritten, NULL);
    }
}

void Logger::LogHeader()
{
    LogWstr(L">>> Logging Started");
    LogWstr(L"Log path: " + m_wsLogPath);
}

void Logger::LogFooter()
{
    LogWstr(L"<<< Logging Ended");
}

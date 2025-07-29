/**
*
* Logger.h
*
* This header defines the Logger class for logging string messages to a file
*
* */

#pragma once

#include <windows.h>
#include <string>

class Logger
{
public:
    Logger(const std::wstring& id);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    bool StartLogging();
    bool IsLoggerOk() const;

    void LogWstr(const std::wstring& ws);
    void LogStr(const std::string& s);

    std::wstring GetLogPath() const;

private:
    HANDLE m_hFile;
    const std::wstring m_id;
    std::wstring m_wsLogPath;

    std::string GetTimestamp() const;
    void WriteLog(const std::string& s);
    void LogHeader();
    void LogFooter();  
};
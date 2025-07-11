#include "Logger.h"
#include <codecvt>

namespace utils
{
	Logger::Logger(const std::wstring& id) : m_wsLogPath(GetAppPath() + id + L"_" + GetTimestamp() + L".txt")
	{
		m_hFile = CreateFileW(
			m_wsLogPath.c_str(),
			FILE_ALL_ACCESS,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
			NULL);

		if (m_hFile)
			LogHeader();
	}

	Logger::~Logger()
	{
		if (m_hFile)
		{
			LogFooter();
			CloseHandle(m_hFile);
		}
	}

	bool Logger::IsLoggerOk() const
	{
		return m_hFile != NULL;
	}

	std::wstring Logger::GetTimestamp() const
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
			L"%d-%02d-%02d_%02d-%02d-%02d",
			Year, Month, Day, Hour, Minute, Second);

		if (len == -1)
		{
			timestamp = L"0000-00-00-000000";
		}
		else
		{
			timestamp.resize(len);
		}
		return timestamp;
	}

	void Logger::LogHeader()
	{
		Log(L">>> Logging started");
		Log(L"Log path: " + m_wsLogPath);
	}

	void Logger::LogFooter()
	{
		Log(L"<<< Logging ended");
	}

	void Logger::Log(const std::wstring& wsMsg)
	{
		WriteLog(wsMsg);
	}

	void Logger::WriteLog(const std::wstring& wsMsg)
	{
		if (m_hFile)
		{
			std::wstring wsLog = L"[" + GetTimestamp() + L"] ";
			wsLog += wsMsg + L"\n";
			// Wide UTF16 string to narrow UTF8 string
			std::string strLog = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(wsLog);
			DWORD bytesWritten{};
			WriteFile(m_hFile, strLog.c_str(), strLog.size(), &bytesWritten, NULL);
		}
	}

	std::wstring Logger::GetLogPath() const
	{
		return m_wsLogPath;
	}

	std::wstring GetAppExePath()
	{
		std::wstring path(MAX_PATH, 0);
		DWORD pathLength = GetModuleFileNameW(NULL, &path.front(), MAX_PATH);
		path.resize(pathLength);
		return path;
	}

	std::wstring GetAppPath()
	{
		std::wstring path = GetAppExePath();
		path = path.substr(0, path.find_last_of(L"\\"));
		return path.empty() ? path : (path + (path.back() == L'\\' ? L"" : L"\\"));
	}

	std::wstring GetAppName()
	{
		std::wstring path = GetAppExePath();
		size_t slashPos = path.find_last_of(L"\\");
		path = path.substr(slashPos + 1, path.size() - slashPos - 1);
		path = path.substr(0, path.find_last_of(L"."));
		return path;
	}
}
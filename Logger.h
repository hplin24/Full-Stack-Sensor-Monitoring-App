#pragma once

#include <windows.h>
#include <string>

namespace utils
{
	class Logger
	{
	public:
		Logger(const std::wstring& id);
		~Logger();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		bool IsLoggerOk() const;

		void Log(const std::wstring& ws);

		std::wstring GetLogPath() const;

	private:
		const std::wstring m_wsLogPath;
		HANDLE m_hFile;

		std::wstring GetTimestamp() const;
		void LogHeader();
		void LogFooter();
		void WriteLog(const std::wstring& wsMsg);
	};


	std::wstring GetAppExePath();
	std::wstring GetAppPath();
	std::wstring GetAppName();
}
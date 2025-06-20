#pragma once

//#include "stdafx.h"
//#include "C_Variables.h"

#include <string>
#include <vector>
#include <mutex>

#define LOG_POS(loc) (LOG_LOC + "::" + loc)

class Logger {
public:
	enum class Level {
		Fatal = 4,
		Error = 3,
		Warning = 2,
		Info = 1,
		Debug = 0
	};

	struct LogEntry {
	public:
		Level log_level;
		std::string source;
		std::string message;
	};


	static void Log(Level level, std::string source, const std::string message);
	static void Log(std::string source, const std::string format, ...); // LogInfo

	static void LogFatal(std::string source, const std::string format, ...);
	static void LogError(std::string source, const std::string format, ...);
	static void LogWarning(std::string source, const std::string format, ...);
	static void LogInfo(std::string source, const std::string format, ...);
	static void LogDebug(std::string source, const std::string format, ...);

	static unsigned int glCheckError_(const char* file, int line);

	static std::vector<LogEntry> GetLogEntries();

	static void Set_Direct(bool value) { m_direct = value; }

	static void Update();

private:

	static Logger m_logger;
	inline static bool m_direct{false};
	std::vector<LogEntry> m_entries;

	static std::mutex m_lock;

	//std::vector<LogEntry> m_entries_processing;
};

#define glCheckError() Logger::glCheckError_(__FILE__, __LINE__) 

#include "Logger.h"

#include <iterator>
#include <sstream>
#include <stdarg.h>
#include <cstdio>
#include <iostream>

#define MESSAGE_BUFFER_SIZE 10 * 1024

#include "opengl.h"

#include "Engine.h"

Logger Logger::m_logger;

void Logger::Log(Logger::Level level, std::string source, std::string message)
{
	LogEntry entry;
	entry.log_level = level;
	entry.source = source;
	entry.message = message;

	m_logger.m_entries.push_back(entry);

	if (m_direct) {
		Update();
	}
}

void Logger::Log(std::string source, const std::string format, ...)
{
	char buffer[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string message = std::string(buffer);
	Log(Level::Info, source, message);
}

void Logger::LogFatal(std::string source, const std::string format, ...)
{
	char buffer[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string message = std::string(buffer);
	Log(Level::Fatal, source, message);
}

void Logger::LogError(std::string source, const std::string format, ...)
{
	char buffer[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string message = std::string(buffer);
	Log(Level::Error, source, message);
}

void Logger::LogWarning(std::string source, const std::string format, ...)
{
	char buffer[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string message = std::string(buffer);
	Log(Level::Warning, source, message);
}

void Logger::LogInfo(std::string source, const std::string format, ...)
{
	char buffer[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string message = std::string(buffer);
	Log(Level::Info, source, message);
}

void Logger::LogDebug(std::string source, const std::string format, ...)
{
	char buffer[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string message = std::string(buffer);
	Log(Level::Debug, source, message);
}

unsigned int Logger::glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	int num_runs = 0;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
		//LogError("GL_ERROR", "%s (%i): %s", file, line, error.c_str());
		num_runs++;

		if (num_runs > 10) {
			std::cout << "glCheckError errors exceeded 10; breaking..." << std::endl;
			break;
		}
	}
	return errorCode;
}

std::vector<Logger::LogEntry> Logger::GetLogEntries()
{
	// Declaring new vector
	std::vector<Logger::LogEntry> res;

	// Copying vector by copy function
	std::copy(m_logger.m_entries.begin(), m_logger.m_entries.end(), std::back_inserter(res));

	m_logger.m_entries.clear();

	return res;
}

void Logger::Update()
{
	bool halt = false;
	std::vector<Logger::LogEntry> entries = GetLogEntries();
	for (const auto& entry : entries) {
		std::string type_str;
		switch (entry.log_level)
		{
			case Logger::Level::Debug:
				type_str = "DEBUG";
				break;
			case Logger::Level::Info:
				type_str = "INFO";
				break;
			case Logger::Level::Warning:
				type_str = "WARNING";
				break;
			case Logger::Level::Error:
				type_str = "ERROR";
				break;
			case Logger::Level::Fatal:
				type_str = "FATAL_ERROR";
				halt = true;
				break;
		}

		printf("%s::%s: %s\n", type_str.c_str(), entry.source.c_str(), entry.message.c_str());
		if (halt) {
			printf("HALT TRIGGERED: Stopping engine...\n");
			Engine::Instance()->Stop();
		}
	}
}



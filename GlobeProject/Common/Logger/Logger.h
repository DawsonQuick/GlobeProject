#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "./../../Common/Utilities/CappedDeque/CappedDeque.h"

enum LogLevel {
	DEBUG,
	INFO,
	WARN,
	ERROR 
};

// Macro to get the file name from the full path
// Macro to get just the file name from the full path
#ifdef _WIN32
#define FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define FILE_NAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define LOG_MESSAGE(LogLevel, message) Logger::log(LogLevel, message, FILE_NAME, __LINE__)

namespace {
	std::string formatTimePoint(const std::chrono::system_clock::time_point& timePoint) {
		// Convert time_point to time_t for formatting
		std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);

		// Convert time_t to a tm structure using localtime_s
		std::tm tm{};
		localtime_s(&tm, &timeT);

		// Format the tm structure to a string
		std::stringstream ss;
		ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");  // Example format: "2024-08-30 14:55:02"

		// Add the milliseconds part
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()) % 1000;
		ss << '.' << std::setw(3) << std::setfill('0') << milliseconds.count();  // Add ".123" for milliseconds

		return ss.str();
	}

	std::string enumToString(LogLevel e) {
		switch (e) {
		case DEBUG: return "DEBUG";
		case INFO: return "INFO";
		case WARN: return "WARN";
		case ERROR: return "ERROR";
		default: return "Unknown";
		}
	}
}


namespace Logger {
	inline std::mutex loggerMutex;
	inline static int currentCapacity = 500;
	inline CappedDeque<std::string> logList(currentCapacity);

	inline LogLevel activeLoggingLevel = LogLevel::DEBUG;

	inline std::deque<std::string> getLogger() {
		std::lock_guard<std::mutex> lock(loggerMutex);
		return logList.get();
	}

	inline void setActiveLogLevel(LogLevel level) {
		std::lock_guard<std::mutex> lock(loggerMutex);
		activeLoggingLevel = level;
	}
	inline void updateLogCapacity() {
		std::lock_guard<std::mutex> lock(loggerMutex);
		logList.updateCapacity(currentCapacity);
	}

	inline std::string getStringLogLevel(int i) {
		std::lock_guard<std::mutex> lock(loggerMutex);
		LogLevel e = static_cast<LogLevel>(i);
		return enumToString(e);
	}

	inline void log(LogLevel level, const std::string& message, const char* fileName, int lineNumber) {
		std::lock_guard<std::mutex> lock(loggerMutex);

		//If the log messages is at a lower level than what the currently set logging level is set to , dont even continue further
		if (!(level >= activeLoggingLevel)) {
			return;
		}

		std::string tempString = formatTimePoint(std::chrono::system_clock::now()) + " - " + fileName + ":" + std::to_string(lineNumber) + " : " + message;
		switch (level) {
		case 0: //Debug
			tempString = "D" + tempString;
			break;
		case 1: //Info
			tempString = "I" + tempString;
			break;
		case 2: //Warn
			tempString = "W" + tempString;
			break;
		case 3: //Error
			tempString = "E" + tempString;
			break;
		default:

			break;
		}
		logList.push_front(tempString);
	}


}
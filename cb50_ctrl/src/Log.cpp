#include "Log.h"
#include <iostream>

void Log::log(std::string message, crow::LogLevel level) {
    switch (level) {
        case crow::LogLevel::Error:
            _errors.push_back(message);
        case crow::LogLevel::Warning:
            _errors.push_back(message);
    }
    std::cerr << "Hello" << std::endl;
    std::cerr << message << std::endl;
}

#ifndef LOGH
#define LOGH
#include <crow/logging.h>
#include <vector>
#include <string>

class Log : public crow::ILogHandler {
   public:
    void log(std::string message, crow::LogLevel level) override;
   private:
    std::vector<std::string> _infos;
    std::vector<std::string> _warnings;
    std::vector<std::string> _errors;
};

#endif

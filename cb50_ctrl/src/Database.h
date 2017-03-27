#ifndef DATABASEH
#define DATABASEH
#include <SQLiteCpp/SQLiteCpp.h>
#include <crow/logging.h>
#include <crow/json.h>
#include <vector>
#include <mutex>

#include "PIRegulator.h"

class Database : public crow::ILogHandler {
    enum LogTypes { Info = 0, Error = 1, Warning = 2, Debug = 3 };

    typedef struct {
        std::string msg;
        int type;
        std::string ts;
    } log_entry_t;

   public:
    Database();
    virtual ~Database();
    void log(std::string message, crow::LogLevel level) override;
    std::vector<log_entry_t> getLog();
    crow::json::wvalue getParameters();
    void updateParameters(const crow::json::wvalue p);
    PIRegulator::parameters_t getRegulatorParameters();

   private:
    void insert_log(const std::string msg, const int type_id);
    SQLite::Database _db;
    std::mutex _m;
};

#endif

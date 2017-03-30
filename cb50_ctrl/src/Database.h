/* 
 * This file is part of the cb50_ctrl distribution (https://github.com/johnerlandsson/cb50_ctrl).
 * Copyright (c) 2017 John Erlandsson
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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
    crow::json::wvalue getLog(const int level = -1);
    crow::json::wvalue getParameters();
    void updateParameters(const crow::json::wvalue p);
    PIRegulator::parameters_t getRegulatorParameters();

   private:
    void insert_log(const std::string msg, const int type_id);
    SQLite::Database _db;
    std::mutex _m;
};

#endif

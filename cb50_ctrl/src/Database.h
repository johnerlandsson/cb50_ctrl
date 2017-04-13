/*
 * This file is part of the cb50_ctrl distribution
 * (https://github.com/johnerlandsson/cb50_ctrl).
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
#include <crow/json.h>
#include <crow/logging.h>
#include <exception>
#include <mutex>
#include <vector>

#include "PIRegulator.h"
#include "Recipe.h"

class NoSuchRecipe : public std::exception {
    virtual const char* what() const throw() {
        return "No such recipe in database.";
    }
};

class AddRecipeError : public std::exception {
    virtual const char* what() const throw() {
        return "Error when adding recipe to database.";
    }
};

class UpdateRecipeError : public std::exception {
    virtual const char* what() const throw() {
        return "Error when updating recipe in database.";
    }
};

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
    void syncRecipe(Recipe r);
    Recipe getRecipe(const std::string name);
    std::vector<std::string> getRecipeNames();

   private:
    void insert_log(const std::string msg, const int type_id);
    void add_recipe(const std::string name, const std::string entries);
    void update_recipe(const int id, const std::string name, const std::string entries);
    SQLite::Database _db;
    std::mutex _m;
};

#endif

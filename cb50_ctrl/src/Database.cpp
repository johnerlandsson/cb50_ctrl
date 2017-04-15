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

#include "Database.h"
#include <iostream>
#include <sstream>

Database::Database()
    : _db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE} {
    _db.exec(
        "CREATE TABLE IF NOT EXISTS 'main'.'log_types' ("
        "'id' INTEGER PRIMARY KEY NOT NULL,"
        "'name' TEXT NOT NULL);");
    _db.exec(
        "INSERT OR IGNORE INTO 'main'.'log_types' (id, name)"
        "VALUES (0, 'INFO');");
    _db.exec(
        "INSERT OR IGNORE INTO 'main'.'log_types' (id, name)"
        "VALUES (1, 'ERROR');");
    _db.exec(
        "INSERT OR IGNORE INTO 'main'.'log_types' (id, name)"
        "VALUES (2, 'WARNING');");
    _db.exec(
        "INSERT OR IGNORE INTO 'main'.'log_types' (id, name)"
        "VALUES (3, 'DEBUG');");
    _db.exec(
        "CREATE TABLE IF NOT EXISTS 'main'.'log' ("
        "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
        "'log_types_id' INTEGER NOT NULL,"
        "'ts' DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "'msg' TEXT NOT NULL);");
    _db.exec(
        "CREATE TABLE IF NOT EXISTS 'main'.'parameters' ("
        "'id' INTEGER PRIMARY KEY NOT NULL,"
        "'json' TEXT NOT NULL);");
    _db.exec(
        "INSERT OR IGNORE INTO 'main'.'parameters' (id, json)"
        "VALUES (0, '{\"regulator\": { \"ki\": 0, \"kp\": 10, \"max_istate\": "
        "200, \"min_istate\": -200 }}');");
    _db.exec(
        "CREATE TABLE IF NOT EXISTS 'main'.'recipes' ("
        "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
        "'recipe' TEXT NOT NULL,"
        "'name' TEXT NOT NULL);");
    _db.exec(
        "INSERT OR IGNORE INTO 'main'.'recipes' (id, recipe, name)"
        "VALUES (0, '[{\"sv\": 65, \"time\": 60, \"confirm\": false},"
        "{\"sv\": 76, \"time\": 10, \"confirm\": true},"
        "{\"sv\": 101, \"time\": 90, \"confirm\": false}]', 'default');");
}

Database::~Database() {}

void Database::log(std::string message, crow::LogLevel level) {
    // Remove trailing newline
    if (message.back() == '\n') message = message.substr(0, message.size() - 1);

    // Remove date and level part of message
    auto rb = message.find_first_of(']');
    if (rb != std::string::npos)
        message = message.substr(rb + 1, message.size());

    switch (level) {
        case crow::LogLevel::Error:
            insert_log(message, 1);
            break;
        case crow::LogLevel::Warning:
            insert_log(message, 2);
            break;
        case crow::LogLevel::Debug:
            insert_log(message, 3);
            break;
        default:
            insert_log(message, 0);
            break;
    }
}

void Database::insert_log(const std::string msg, const int type_id) {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db,
                        "INSERT INTO 'main'.'log' (log_types_id, msg)"
                        "VALUES (?, ?);");
    q.bind(1, type_id);
    q.bind(2, msg);

    q.exec();
}

crow::json::wvalue Database::getLog(const int level) {
    std::lock_guard<std::mutex> lock(_m);
    crow::json::wvalue ret;

    std::stringstream ss;
    ss << "SELECT ts, log_types_id, msg FROM log ";

    if (level >= 0 && level <= 3) ss << "WHERE log_types_id=" << level << ' ';

    ss << "ORDER BY ts DESC LIMIT 1000;";

    SQLite::Statement q(_db, ss.str());

    size_t i = 0;
    while (q.executeStep()) {
        ret["data"][i]["ts"] = q.getColumn(0).getString();
        ret["data"][i]["level"] = q.getColumn(1).getInt();
        ret["data"][i]["msg"] = q.getColumn(2).getString();
        ++i;
    }

    return crow::json::load(crow::json::dump(ret));
}

crow::json::wvalue Database::getParameters() {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db, "SELECT json FROM parameters WHERE id='0';");
    if (q.executeStep()) return crow::json::load(q.getColumn(0).getString());

    return crow::json::wvalue();
}

void Database::updateParameters(const crow::json::wvalue p) {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db, "UPDATE parameters SET json=? WHERE id='0';");
    q.bind(1, crow::json::dump(p));
    q.exec();
}

PIRegulator::parameters_t Database::getRegulatorParameters() {
    PIRegulator::parameters_t ret;
    auto p = getParameters();

    ret.kp = std::stod(crow::json::dump(p["regulator"]["kp"]));
    ret.ki = std::stod(crow::json::dump(p["regulator"]["ki"]));
    ret.max_istate = std::stod(crow::json::dump(p["regulator"]["max_istate"]));
    ret.min_istate = std::stod(crow::json::dump(p["regulator"]["min_istate"]));

    return ret;
}

void Database::syncRecipe(Recipe r) {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db,
                        "SELECT DISTINCT id FROM recipes WHERE name=?;");
    q.bind(1, r.getName());

    if (!q.executeStep())
        add_recipe(r.getName(), r.entries2Str());
    else
        update_recipe(q.getColumn(0).getInt(), r.getName(), r.entries2Str());
}

Recipe Database::getRecipe(const std::string name) {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db,
                        "SELECT DISTINCT recipe FROM recipes WHERE name=?;");
    q.bind(1, name);

    if (!q.executeStep()) throw NoSuchRecipe();

    crow::json::wvalue jr; 
    jr["name"] = name;
    jr["entries"] = crow::json::load(q.getColumn(0).getString());

    return Recipe::fromWvalue(jr);
}

std::vector<std::string> Database::getRecipeNames() {
    std::vector<std::string> ret;
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db, "SELECT name FROM recipes;");
    while (q.executeStep()) ret.push_back(q.getColumn(0).getString());

    return ret;
}

void Database::add_recipe(const std::string name, const std::string entries) {
    SQLite::Statement q(_db, "INSERT INTO recipes (name, recipe) VALUES(?, ?);");
    q.bind(1, name);
    q.bind(2, entries);
    if(!q.exec()) throw AddRecipeError();
}

void Database::update_recipe(const int id, const std::string name,
                             const std::string entries) {
    SQLite::Statement q(_db, "UPDATE recipes SET name=?, recipe=? WHERE id=?;");
    q.bind(1, name);
    q.bind(2, entries);
    q.bind(3, id);
    if(!q.exec()) throw UpdateRecipeError();
}

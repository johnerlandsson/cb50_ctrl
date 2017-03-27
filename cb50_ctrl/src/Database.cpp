#include "Database.h"
#include <iostream>

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

std::vector<Database::log_entry_t> Database::getLog() {
    std::vector<log_entry_t> ret;

    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db,
                        "SELECT ts, log_types_id, msg FROM log ORDER BY ts;");

    while (q.executeStep()) {
        log_entry_t row;
        row.ts = q.getColumn(0).getString();
        row.type = q.getColumn(1).getInt();
        row.msg = q.getColumn(2).getString();

        ret.push_back(row);
    }

    return ret;
}
crow::json::wvalue Database::getParameters() {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db, 
            "SELECT json FROM parameters WHERE id='0';");
    if (q.executeStep())
        return crow::json::load(q.getColumn(0).getString());

   return crow::json::wvalue();
}

void Database::updateParameters(const crow::json::wvalue p) {
    std::lock_guard<std::mutex> lock(_m);
    SQLite::Statement q(_db,
            "UPDATE parameters SET json=? WHERE id='0';");
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

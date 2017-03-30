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

#include <crow.h>
#include <iostream>
#include <memory>
#include <thread>

#include "Database.h"
#include "PIRegulator.h"
#include "ProcessValue.h"
#include "TrendData.h"

#define MACHINE_CYCLE_TIME_MS 100
#define PI_CYCLE_TIME_MS 10000

#ifdef NDEBUG
#define WWW_PREFIX "/var/www/"
#else
#define WWW_PREFIX "../../www/"
#endif

using namespace std;

enum class MachineState { Stopped, Warming, AtTemp };

// Global variables
bool g_run_machine_cycle = true;
TrendData g_trendData;
Database g_db;
crow::json::wvalue g_process_data;
std::mutex g_process_data_mutex;

string machinestate2string(const MachineState& s) {
    switch (s) {
        case MachineState::Warming:
            return "warming";
        case MachineState::AtTemp:
            return "at temp";
        default:
            break;
    }

    return "stopped";
}

/* file2response
 * Try to open the given filename and convert it to a crow::response
 */
crow::response file2response(const string filename) {
// Prevent directory traversal
#ifdef NDEBUG
    if (filename.find("../") != string::npos) {
        CROW_LOG_WARNING << "Traversal attempt: " << filename;
        return crow::response(403);
    }
#endif

    ifstream f(filename);
    if (f.is_open()) {
        string body{(istreambuf_iterator<char>(f)),
                    (istreambuf_iterator<char>())};
        f.close();
        if (body.length() <= 0) return crow::response(500);

        return crow::response(body);
    }

    return crow::response(404);
}

/* file_exists
 * Returns true if file at given path exists
 */
inline bool file_exists(const std::string& p) {
    struct stat buffer;
    return (stat(p.c_str(), &buffer) == 0);
}

/* machine_cycle
 * Handle I/O and temperature regulator
 */
void machine_cycle() {
    PIRegulator reg{g_db.getRegulatorParameters()};
    int pi_cycle_counter{0};
    int trend_cycle_counter{0};
    double sv{20.0f};
    double output{0.0f};
    constexpr auto cycle_duration = chrono::milliseconds(MACHINE_CYCLE_TIME_MS);
    auto state = MachineState::Stopped;
    ProcessValue temp_sensor;

    while (g_run_machine_cycle) {
// Read temperature
#ifdef DRY_RUN
        sensor.calculate();
#endif
        double pv = temp_sensor.getValue();

        auto cycle_started = chrono::system_clock::now();

        // Calculate new output
        if (state == MachineState::Warming || state == MachineState::AtTemp) {
            if (pi_cycle_counter ==
                PI_CYCLE_TIME_MS / MACHINE_CYCLE_TIME_MS - 1) {
                output = reg.regulate(sv, pv);
                pi_cycle_counter = 0;
            }
        } else {
            output = 0.0f;
        }

        if (trend_cycle_counter >= 100) {
            g_trendData.append(sv, pv, output);
            trend_cycle_counter = 0;
        }

        // Update process data
        g_process_data_mutex.lock();
        g_process_data["sv"] = sv;
        g_process_data["pv"] = pv;
        g_process_data["output"] = output;
        g_process_data["state"] = machinestate2string(state);
        g_process_data_mutex.unlock();

        this_thread::sleep_until(cycle_started + cycle_duration);

        ++pi_cycle_counter;
        ++trend_cycle_counter;
    }
}

int main(void) {
    // if (!g_parameters.load_file()) {
    // CROW_LOG_ERROR << "Failed to load parameter file.";
    // return 1;
    //}

    // Log log;
    crow::logger::setHandler(&g_db);

    crow::SimpleApp app;
    crow::mustache::set_base(WWW_PREFIX);
    thread machine_thread(machine_cycle);

    // Static routing of assets
    CROW_ROUTE(app, "/assets/<str>/<str>")
    ([](string folder, string file) {
        return file2response(string(WWW_PREFIX) + "assets/" + folder + '/' +
                             file);
    });

    // Send process data
    CROW_ROUTE(app, "/get_pd")
    ([]() {
        std::lock_guard<mutex> lock(g_process_data_mutex);
        return crow::response(g_process_data);
    });

    // Send log
    CROW_ROUTE(app, "/get_log")
    ([]() { return crow::response(g_db.getLog()); });

    CROW_ROUTE(app, "/get_log/<int>")
    ([](int level) { return crow::response(g_db.getLog(level)); });

    // Route trend data
    CROW_ROUTE(app, "/get_trend")
    ([]() { return crow::response(g_trendData.getData()); });

    // Send parameters
    CROW_ROUTE(app, "/get_parameters")
    ([]() { return crow::response(g_db.getParameters()); });

    // Receive parameters
    CROW_ROUTE(app, "/put_parameters")
        .methods("GET"_method, "POST"_method)([](const crow::request& req) {
            try {
                auto c = crow::json::load(req.body);
                g_db.updateParameters(c["message"]);
                return crow::response(200);
            } catch (...) {
                CROW_LOG_WARNING
                    << "Failed to convert received parameter data to wvalue.";
                return crow::response(417);
            }
            return crow::response(500);
        });

    // Serve pages
    CROW_ROUTE(app, "/<str>")
    ([](string page) {
        crow::mustache::context ctx;
        return file2response(string(WWW_PREFIX) + page + ".html");
    });

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        // return crow::response(crow::mustache::load("index.html").render());
        return file2response(string(WWW_PREFIX) + "index.html");
    });

    CROW_LOG_INFO << "Application started";

    // Start server
    app.port(80).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    CROW_LOG_INFO << "Clean exit";

    return 0;
}

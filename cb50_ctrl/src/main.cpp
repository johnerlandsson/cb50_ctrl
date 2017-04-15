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
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include "Database.h"
#include "PIRegulator.h"
#include "ProcessData.h"
#include "ProcessValue.h"
#include "Recipe.h"
#include "TrendData.h"

#define MACHINE_CYCLE_TIME_MS 100
#define PI_CYCLE_TIME_MS 10000

#ifdef NDEBUG
#define WWW_PREFIX "/var/www/"
#else
#define WWW_PREFIX "../../www/"
#endif

using namespace std;

// Global variables
bool g_run_machine_cycle = true;
TrendData g_trendData;
Database g_db;
ProcessData g_process_data;

bool str2bool(string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    istringstream is(s);
    bool ret;
    is >> std::boolalpha >> ret;
    return ret;
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
    constexpr auto cycle_duration = chrono::milliseconds(MACHINE_CYCLE_TIME_MS);
    ProcessValue temp_sensor;

    while (g_run_machine_cycle) {
// Read process data
#ifdef DRY_RUN
        sensor.calculate();
#endif
        g_process_data.setPv(temp_sensor.getValue());

        auto cycle_started = chrono::system_clock::now();

        // Calculate new output
        if (g_process_data.getRunRecipe()) {
            if (pi_cycle_counter ==
                PI_CYCLE_TIME_MS / MACHINE_CYCLE_TIME_MS - 1) {
                g_process_data.setOutput(reg.regulate(g_process_data.getSv(),
                                                      g_process_data.getPv()));
                pi_cycle_counter = 0;
            }
        } else {
            g_process_data.setOutput(0.0f);
        }

        if (trend_cycle_counter >= 100) {
            g_trendData.append(g_process_data.getSv(), g_process_data.getPv(),
                               g_process_data.getOutput());
            trend_cycle_counter = 0;
        }

        this_thread::sleep_until(cycle_started + cycle_duration);

        ++pi_cycle_counter;
        ++trend_cycle_counter;
    }
}

inline void setup_routing(crow::SimpleApp& app) {
    // Send recipe names
    CROW_ROUTE(app, "/get_recipe_names")
    ([]() {
        auto names = g_db.getRecipeNames();
        crow::json::wvalue ret;
        int i = 0;
        for (auto n : names) ret[i++] = n;

        return crow::response(ret);
    });

    // Send specific recipe
    CROW_ROUTE(app, "/get_recipe/<str>")
    ([](string name) {
        try {
            auto r = g_db.getRecipe(name).toWvalue();
            //cout << crow::json::dump(r) << endl;
            return crow::response(r);
        } catch (NoSuchRecipe&) {
            return crow::response(404);
        }
    });

    // Receive recipe to be added or updated
    CROW_ROUTE(app, "/sync_recipe")
        .methods("GET"_method, "POST"_method)([](const crow::request& req) {
            crow::response ret;
            try {
                crow::json::wvalue c = crow::json::load(req.body)["message"];
                g_db.syncRecipe(Recipe::fromWvalue(c));
                ret.body = "ok";
                ret.code = 200;
                CROW_LOG_INFO << "Synced recipe: "
                              << crow::json::dump(c["name"]);
            } catch (const exception& e) {
                ret.body = e.what();
                ret.code = 500;
                CROW_LOG_ERROR << "Failed to sync recipe: " << req.body
                               << " Message: " << e.what();
            }

            return ret;
        });

    // Static routing of assets
    CROW_ROUTE(app, "/assets/<str>/<str>")
    ([](string folder, string file) {
        return file2response(string(WWW_PREFIX) + "assets/" + folder + '/' +
                             file);
    });

    // Send process data
    CROW_ROUTE(app, "/get_pd")
    ([]() { return crow::response(g_process_data.toWvalue()); });

    // Receive process data
    CROW_ROUTE(app, "/put_pd")
        .methods("GET"_method, "POST"_method)([](const crow::request& req) {
            try {
                auto c = crow::json::load(req.body)["message"];
                g_process_data.setPump(str2bool(crow::json::dump(c["pump"])));
                g_process_data.setMixer(str2bool(crow::json::dump(c["mixer"])));
                g_process_data.setRunRecipe(
                    str2bool(crow::json::dump(c["run_recipe"])));

                return crow::response(g_process_data.toWvalue());
            } catch (const exception& e) {
                CROW_LOG_WARNING
                    << "Caught exception when receiving process data. "
                    << e.what();
                return crow::response(417);
            }
            return crow::response(500);
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
}

int main(void) {
    crow::logger::setHandler(&g_db);

    crow::SimpleApp app;
    crow::mustache::set_base(WWW_PREFIX);
    setup_routing(app);

    thread machine_thread(machine_cycle);

    CROW_LOG_INFO << "Application started";

    // Start server
    app.port(80).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    CROW_LOG_INFO << "Clean exit";

    return 0;
}

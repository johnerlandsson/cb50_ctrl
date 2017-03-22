#include <crow.h>
#include <sys/stat.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Parameters.h"
#include "TrendData.h"

#define MACHINE_CYCLE_TIME_MS 100
#define PI_CYCLE_TIME_MS 10000

using namespace std;

// Global variables
bool g_run_machine_cycle = true;
Parameters g_parameters;
TrendData g_trendData;

/* file2response
 * Try to open the given filename and convert it to a crow::response
 */
crow::response file2response(const string filename) {
    // Prevent directory traversal
    if (filename.find("../") != string::npos) return crow::response(403);

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

string render_file(const std::string file) {
    crow::mustache::context ctx;
    if (!file_exists("www/" + file)) return string();
    return crow::mustache::load(file).render();
}

void machine_cycle() {
    PIRegulator reg{g_parameters.getRegulatorParameters()};
    int pi_cycle_counter{0};
    int trend_cycle_counter{0};
    double sv{20.0f};
    double pv{4.0f};
    double output{0.0f};
    constexpr auto cycle_duration = chrono::milliseconds(MACHINE_CYCLE_TIME_MS);

    while (g_run_machine_cycle) {
        auto cycle_started = chrono::system_clock::now();

        if (pi_cycle_counter == PI_CYCLE_TIME_MS / MACHINE_CYCLE_TIME_MS - 1) {
            output = reg.regulate(0.0f, 0.0f);
            pi_cycle_counter = 0;
            cout << "Regulating" << endl;
        }

        if (trend_cycle_counter >= 10000) {
            pv += 1.0f;
            g_trendData.append(sv, pv, output);
            trend_cycle_counter = 0;
        }

        this_thread::sleep_until(cycle_started + cycle_duration);

        ++pi_cycle_counter;
        ++trend_cycle_counter;
    }
}

int main(int argc, const char* argv[]) {
    if (!g_parameters.load_file()) {
        cerr << "Failed to load parameter file." << endl;
        return 1;
    }

    crow::SimpleApp app;
    crow::mustache::set_base("./www");
    thread machine_thread(machine_cycle);

    // Static routing of assets
    CROW_ROUTE(app, "/assets/<str>/<str>")
    ([](string folder, string file) {
        return file2response("www/assets/" + folder + '/' + file);
    });

    CROW_ROUTE(app, "/get_trend")
    ([]() {
        crow::json::wvalue data;
        data["labels"][0] = 0;
        data["labels"][1] = 1;
        data["labels"][2] = 2;
        data["labels"][3] = 3;
        data["labels"][4] = 4;
        data["labels"][5] = 5;
        data["labels"][6] = 6;
        data["labels"][7] = 7;
        data["labels"][8] = 8;
        data["labels"][9] = 9;
        data["data"]["sv"][0] = 20;
        data["data"]["sv"][1] = 21;
        data["data"]["sv"][2] = 21;
        data["data"]["sv"][3] = 22;
        data["data"]["sv"][4] = 22;
        data["data"]["sv"][5] = 24;
        data["data"]["sv"][6] = 25;
        data["data"]["sv"][7] = 26;
        data["data"]["sv"][8] = 27;
        data["data"]["sv"][9] = 28;
        return crow::response(data);
    });

    // Render pages
    CROW_ROUTE(app, "/<str>")
    ([](string page) {
        crow::mustache::context ctx;
        return file2response("www/" + page + ".html");
    });

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::response(crow::mustache::load("index.html").render());
    });

    // Start server
    app.port(80).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    return 0;
}

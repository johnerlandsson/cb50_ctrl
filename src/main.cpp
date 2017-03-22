#include <crow.h>
#include <sys/stat.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Parameters.h"

#define MACHINE_CYCLE_TIME_MS 100
#define PI_CYCLE_TIME_MS 10000

using namespace std;

// Global variables
bool g_run_machine_cycle = true;
Parameters g_parameters;

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

string file2string(const string file) {
    ifstream f(file);
    if (f.is_open()) {
        string ret{(istreambuf_iterator<char>(f)),
                   (istreambuf_iterator<char>())};
        f.close();
        return ret;
    }

    return string();
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
    int cycle_counter = 0;
    constexpr auto cycle_duration = chrono::milliseconds(MACHINE_CYCLE_TIME_MS);

    while (g_run_machine_cycle) {
        auto cycle_started = chrono::system_clock::now();

        if (cycle_counter == PI_CYCLE_TIME_MS / MACHINE_CYCLE_TIME_MS - 1) {
            auto outp = reg.regulate(0.0f, 0.0f);
            cycle_counter = 0;
            cout << "Regulating" << endl;
        }

        this_thread::sleep_until(cycle_started + cycle_duration);

        ++cycle_counter;
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

    // Render pages
    CROW_ROUTE(app, "/<str>")
    ([](string page) {
        crow::mustache::context ctx;
        return crow::response(crow::mustache::load(page + ".html").render());
    });

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::response(crow::mustache::load("index.html").render());
    });

    app.port(80).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    return 0;
}

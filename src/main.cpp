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

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load("index.html").render();
    });

    CROW_ROUTE(app, "/parameters")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load("parameters.html").render();
    });

    CROW_ROUTE(app, "/log")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load("log.html").render();
    });

    CROW_ROUTE(app, "/trend")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load("trend.html").render();
    });

    CROW_ROUTE(app, "/assets/js/<str>")
    ([](string path) {
        auto body = file2string("www/assets/js/" + path);
        if (body.length() <= 0) return crow::response(404);

        return crow::response(body);
    });

    CROW_ROUTE(app, "/assets/js/chart/chart.js")
    ([]() {
        auto body = render_file("assets/js/chart/chart.js");
        if (body.length() <= 0) return crow::response(404);

        return crow::response(body);
    });

    CROW_ROUTE(app, "/assets/js/chart/<str>/<str>")
    ([](string subfolder, string file) {
        auto body = render_file("assets/js/chart/" + subfolder + '/' + file);
        if (body.length() <= 0) return crow::response(404);

        return crow::response(body);
    });

    CROW_ROUTE(app, "/assets/css/<str>")
    ([](string path) {
        auto f = "assets/css/" + path;
        if (file_exists("www/" + f)) {
            crow::mustache::context ctx;
            return crow::response(crow::mustache::load(f).render());
        }
        return crow::response(404);
    });

    CROW_ROUTE(app, "/assets/img/<str>")
    ([](string path) {
        auto f = "assets/img/" + path;
        if (file_exists("www/" + f)) {
            crow::mustache::context ctx;
            return crow::response(crow::mustache::load(f).render());
        }
        return crow::response(404);
    });

    CROW_ROUTE(app, "/assets/fonts/<str>")
    ([](string path) {
        auto body = file2string("www/assets/fonts/" + path);
        if (body.length() <= 0) return crow::response(404);
        return crow::response(body);
    });

    app.port(80).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    return 0;
}

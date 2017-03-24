#include <crow.h>
#include <iostream>
#include <thread>

#include "Parameters.h"
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
Parameters g_parameters;
TrendData g_trendData;

/* file2response
 * Try to open the given filename and convert it to a crow::response
 */
crow::response file2response(const string filename) {
    // Prevent directory traversal
#ifdef NDEBUG
    if (filename.find("../") != string::npos) return crow::response(403);
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
        }

        if (trend_cycle_counter >= 100) {
            pv += 1.0f;
            static int tmp = 0;
            pv = 100.0 * cos(tmp * 0.1);
            tmp++;
            g_trendData.append(sv, pv, output);
            trend_cycle_counter = 0;
        }

        this_thread::sleep_until(cycle_started + cycle_duration);

        ++pi_cycle_counter;
        ++trend_cycle_counter;
    }
}

int main(void) {
    if (!g_parameters.load_file()) {
        cerr << "Failed to load parameter file." << endl;
        return 1;
    }

    crow::SimpleApp app;
    crow::mustache::set_base(WWW_PREFIX);
    thread machine_thread(machine_cycle);

    // Static routing of assets
    CROW_ROUTE(app, "/assets/<str>/<str>")
    ([](string folder, string file) {
        return file2response(string(WWW_PREFIX) + "assets/" + folder + '/' + file);
    });

    // Route trend data
    CROW_ROUTE(app, "/get_trend")
    ([]() { return crow::response(g_trendData.getData()); });

    // Send parameters
    CROW_ROUTE(app, "/get_parameters")
    ([]() { return crow::response(g_parameters.to_wvalue()); });

    //Receive parameters
    CROW_ROUTE(app, "/put_parameters")
        .methods("GET"_method, "POST"_method)([](const crow::request& req) {
            try {
                g_parameters.from_wvalue(crow::json::load(req.body)["message"]);
                return crow::response(200);
            } catch (...) {
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
        //return crow::response(crow::mustache::load("index.html").render());
        return file2response(string(WWW_PREFIX) + "index.html");
    });

    // Start server
    app.port(80).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    return 0;
}

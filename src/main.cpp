#include <crow.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "Parameters.h"

#define MACHINE_CYCLE_TIME_MS 100
#define PI_CYCLE_TIME_MS 10000

using namespace std;

// Global variables
bool g_run_machine_cycle = true;
Parameters g_parameters;

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

int main(int argc, const char *argv[]) {
    if(!g_parameters.load_file())
    {
        cerr << "Failed to load parameter file." << endl;
        return 1;
    }

    crow::SimpleApp app;
    thread machine_thread(machine_cycle);

    CROW_ROUTE(app, "/")
    ([]() { return "Hello"; });

    app.port(18080).run();

    // Stop machine thread
    g_run_machine_cycle = false;
    machine_thread.join();

    return 0;
}

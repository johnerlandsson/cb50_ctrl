#include "ProcessData.h"

ProcessData::ProcessData()
    : _sv{0.0f}, _pv{0.0f}, _output{0.0f}, _pump{false}, _mixer{false} {}
ProcessData::~ProcessData() {}

double ProcessData::getSv() const {
    std::lock_guard<std::mutex> lock(_m);
    return _sv;
}

double ProcessData::getPv() const {
    std::lock_guard<std::mutex> lock(_m);
    return _pv;
}

double ProcessData::getOutput() const {
    std::lock_guard<std::mutex> lock(_m);
    return _output;
}

bool ProcessData::getPump() const {
    std::lock_guard<std::mutex> lock(_m);
    return _pump;
}

bool ProcessData::getMixer() const {
    std::lock_guard<std::mutex> lock(_m);
    return _mixer;
}

bool ProcessData::getRunRecipe() const {
    std::lock_guard<std::mutex> lock(_m);
    return _run_recipe;
}

crow::json::wvalue ProcessData::toWvalue() const {
    crow::json::wvalue ret;
    std::lock_guard<std::mutex> lock(_m);
    ret["pv"] = _pv;
    ret["sv"] = _sv;
    ret["output"] = _output;
    ret["pump"] = _pump;
    ret["mixer"] = _mixer;
    ret["run_recipe"] = _run_recipe;

    return ret;
}

void ProcessData::setSv(const double v) {
    std::lock_guard<std::mutex> lock(_m);
    _sv = v;
}

void ProcessData::setPv(const double v) {
    std::lock_guard<std::mutex> lock(_m);
    _pv = v;
}

void ProcessData::setOutput(const double v) {
    std::lock_guard<std::mutex> lock(_m);
    _output = v;
}

void ProcessData::setPump(const bool v) {
    std::lock_guard<std::mutex> lock(_m);
    _pump = v;
}

void ProcessData::setMixer(const bool v) {
    std::lock_guard<std::mutex> lock(_m);
    _mixer = v;
}

void ProcessData::setRunRecipe(const bool v) {
    std::lock_guard<std::mutex> lock(_m);
    _run_recipe = v;
}

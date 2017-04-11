#include "Recipe.h"

Recipe::Recipe() {}

Recipe::Recipe(std::string name, std::vector<Recipe::recipe_entry_t> entries)
    : _name{name}, _entries{entries} {}

Recipe::~Recipe() {}

Recipe Recipe::fromWvalue(crow::json::wvalue& r) {
    Recipe ret;
    ret._entries.clear();
    ret._name = crow::json::dump(r["name"]);

    int i = 0;
    while (crow::json::dump(r["entries"][i]["sv"]) != "null") {
        recipe_entry_t et;
        et.confirm = crow::json::dump(r["entries"][i]["confirm"]) == "true"
                         ? true
                         : false;
        et.dur = std::chrono::minutes(
            std::stoi(crow::json::dump(r["entries"][i]["time"])));
        et.sv = std::stod(crow::json::dump(r["entries"][i]["sv"]));

        ret._entries.push_back(et);
        ++i;
    }

    return ret;
}

crow::json::wvalue Recipe::toWvalue() const {
    crow::json::wvalue ret;
    ret["name"] = _name;
    int i = 0;
    for (auto e : _entries) {
        ret["entries"][i]["time"] = e.dur.count();
        ret["entries"][i]["sv"] = e.sv;
        ret["entries"][i]["confirm"] = e.confirm;
        ++i;
    }

    return ret;
}

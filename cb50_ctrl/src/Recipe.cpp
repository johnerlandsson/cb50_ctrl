#include "Recipe.h"

Recipe::Recipe() {}

Recipe::Recipe(std::string name, std::vector<Recipe::recipe_entry_t> entries)
    : _name{name}, _entries{entries} {}

Recipe::~Recipe() {}

Recipe Recipe::fromWvalue(crow::json::wvalue r) {
    Recipe ret;
    ret._entries.clear();

    int i = 0;
    while (r[i].count("name") > 0) {
        recipe_entry_t et;
        et.name = crow::json::dump(r[i]["name"]);
        et.dur =
            std::chrono::minutes(std::stoi(crow::json::dump(r[i]["duration"])));
        et.sv = std::stod(crow::json::dump(r[i]["sv"]));

        ret._entries.push_back(et);
        ++i;
    }

    return ret;
}

crow::json::wvalue Recipe::toWvalue() const {
    crow::json::wvalue ret;
    int i = 0;
    for (auto e : _entries) {
        ret[i]["name"] = e.name;
        ret[i]["duration"] = e.dur.count();
        ret[i]["sv"] = e.sv;
        ++i;
    }

    return ret;
}

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

std::string Recipe::getName() const {
    return _name;
}

std::string Recipe::entries2Str() const {
    crow::json::wvalue ret;
    int i = 0;
    for(auto e : _entries) {
        ret[i]["sv"] = e.sv;
        ret[i]["time"] = e.dur.count();
        ret[i]["confirm"] = e.confirm;
        i++;
    }

    return crow::json::dump(ret);
}

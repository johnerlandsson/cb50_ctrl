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

#ifndef RECIPEH
#define RECIPEH

#include <chrono>
#include <string>
#include <vector>
#include <crow/json.h>

class Recipe {
   public:
    typedef struct {
        std::chrono::minutes dur;
        float sv;
        bool confirm;
    } recipe_entry_t;

    Recipe();
    Recipe(std::string name, std::vector<recipe_entry_t> entries);
    virtual ~Recipe();

    static Recipe fromWvalue(crow::json::wvalue& r);
    crow::json::wvalue toWvalue() const;

   private:
    std::string _name;
    std::vector<recipe_entry_t> _entries;
};

#endif

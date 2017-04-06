#ifndef RECIPEH
#define RECIPEH

#include <chrono>
#include <string>
#include <vector>
#include <crow/json.h>

class Recipe {
   public:
    typedef struct {
        std::string name;
        std::chrono::minutes dur;
        float sv;
    } recipe_entry_t;

    Recipe();
    Recipe(std::string name, std::vector<recipe_entry_t> entries);
    virtual ~Recipe();

    void fromWvalue(crow::json::wvalue r);
    crow::json::wvalue toWvalue() const;

   private:
    std::string _name;
    std::vector<recipe_entry_t> _entries;
};

#endif

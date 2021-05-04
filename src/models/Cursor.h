#ifndef SPOTIFY_PLUSPLUS_CURSOR_H
#define SPOTIFY_PLUSPLUS_CURSOR_H

#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Cursor
{
public:
    Cursor(nlohmann::json cursorJson);
    std::string GetAfter() const;

private:
    std::string after;
};


#endif

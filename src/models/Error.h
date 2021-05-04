#ifndef SPOTIFY_PLUSPLUS_ERROR_H
#define SPOTIFY_PLUSPLUS_ERROR_H

#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Error
{
public:
    Error(nlohmann::json pagerJson);

    int GetStatus() const;
    std::string GetMessage() const;

private:
    int status;
    std::string message;
};


#endif

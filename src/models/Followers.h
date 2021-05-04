#ifndef SPOTIFY_PLUSPLUS_FOLLOWERS_H
#define SPOTIFY_PLUSPLUS_FOLLOWERS_H

#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Followers
{
public:
    Followers(nlohmann::json followersJson);

    std::string GetHref() const;
    int GetTotal() const;

private:
    std::string href;
    int total;
};


#endif

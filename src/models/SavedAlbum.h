#ifndef SPOTIFY_PLUSPLUS_SAVEDALBUM_H
#define SPOTIFY_PLUSPLUS_SAVEDALBUM_H

#include <string>
#include "Album.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class SavedAlbum
{
public:
    SavedAlbum(nlohmann::json albumJson);

    std::string GetAddedAt() const;
    std::shared_ptr<Album> GetAlbum() const;

private:
    std::string addedAt;
    std::shared_ptr<Album> album;
};


#endif

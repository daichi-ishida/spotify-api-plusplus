#ifndef SPOTIFY_PLUSPLUS_CURLUTILS_H
#define SPOTIFY_PLUSPLUS_CURLUTILS_H

#include <curl/curl.h>
#include "../models/Error.h"
#include <nlohmann/json.hpp>
#include "CurlException.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <sstream>
#include <cstdlib>

using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

const std::string encodeClientCode(const std::string &client_id, const std::string &client_secret)
{
    using namespace boost::archive::iterators;

    const std::string client_code = client_id + ":" + client_secret;

    std::stringstream os;
    typedef base64_from_binary<    // convert binary values to base64 characters
                transform_width<   // retrieve 6 bit integers from a sequence of 8 bit bytes
                    const char *,
                    6,
                    8
                >
            > 
            base64_text; // compose all the above operations in to a new iterator

    std::copy(
        base64_text(client_code.c_str()),
        base64_text(client_code.c_str() + client_code.size()),
        ostream_iterator<char>(os)
    );

    return "Authorization: Basic " + os.str() + "=";
}

nlohmann::json RequestToken(const std::string &client_id, const std::string &client_secret)
{
    CURL * curl;

    curl = curl_easy_init ( ) ;
    if(!curl)
    {
        std::cerr << "Could not initiate cURL" << std::endl;
        std::exit(1);
    }

    std::string url = "https://accounts.spotify.com/api/token";

    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // Can't authenticate the certificate, so disable authentication.
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    std::string header = encodeClientCode(client_id, client_secret);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "grant_type=client_credentials");

    int rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        throw CurlException(rc);

    curl_easy_cleanup(curl);
    if(readBuffer.empty())
        return nlohmann::json();
    return nlohmann::json::parse(readBuffer);
}

nlohmann::json SpotifyCurlInternal(const std::string &request, const std::string &endpoint, const std::map<std::string, std::string> &options, const std::string &authToken, const std::string &body)
{
    CURL * curl;

    curl = curl_easy_init ( ) ;
    if(!curl)
    {
        std::cerr << "Could not initiate cURL" << std::endl;
        std::exit(1);
    }

    std::string url = "https://api.spotify.com" + endpoint;
    if(!options.empty())
    {
        url += "?";
        for(auto option : options)
        {
            url += option.first + "=" + option.second + '&';
        }
    }
    url = ReplaceAll(url, " ", "%20");
    url = ReplaceAll(url, ",", "%2C");

    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // Can't authenticate the certificate, so disable authentication.
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());

    if(!authToken.empty())
    {
        std::string header = "Authorization: Bearer " + authToken;
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    if(!body.empty())
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    int rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        throw CurlException(rc);

    curl_easy_cleanup(curl);
    if(readBuffer.empty())
        return nlohmann::json();
    return nlohmann::json::parse(readBuffer);
}

nlohmann::json SpotifyGET(const std::string &endpoint, const std::map<std::string, std::string> &options, const std::string &authToken, std::string body = "")
{
    return SpotifyCurlInternal("GET", endpoint, options, authToken, body);
}

nlohmann::json SpotifyPUT(const std::string &endpoint, const std::map<std::string, std::string> &options, const std::string &authToken, std::string body = "")
{
   return SpotifyCurlInternal("PUT", endpoint, options, authToken, body);
}

nlohmann::json SpotifyDELETE(const std::string &endpoint, const std::map<std::string, std::string> &options, const std::string &authToken, std::string body = "")
{
    return SpotifyCurlInternal("DELETE", endpoint, options, authToken, body);
}

nlohmann::json SpotifyPOST(const std::string &endpoint, const std::map<std::string, std::string> &options, const std::string &authToken, std::string body = "")
{
    return SpotifyCurlInternal("POST", endpoint, options, authToken, body);
}

std::string VectorJoin(const std::vector<std::string> &vector)
{
    std::stringstream ss;
    for(size_t i = 0; i < vector.size(); ++i)
    {
        if(i != 0)
            ss << ",";
        ss << vector[i];
    }
    return ss.str();
}

#endif

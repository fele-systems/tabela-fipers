#include <curlpp.h>
#include <stdexcept>
#include <fmt/core.h>
#include <cassert>

namespace curlpp {
    
    Client::~Client()
    {
        curl_easy_cleanup(hCurl);
    }
    
    std::string encode(const std::string& input)
    {
        char* buffer = curl_easy_escape(nullptr, input.c_str(), static_cast<int>(input.size()));
        std::string encoded{ buffer };
        curl_free(buffer);
        return encoded;
    }

    std::string decode(const std::string& input)
    {
        int len = 0;
        char* buffer = curl_easy_unescape(nullptr, input.c_str(), static_cast<int>(input.size()), &len);
        std::string decoded{ buffer, static_cast<size_t>(len) };
        curl_free(buffer);
        return decoded;
    }

    std::unordered_map<std::string, std::string> str_to_query(std::string_view query_as_str)
    {
        std::unordered_map<std::string, std::string> query;
        
        size_t p = 0;
        size_t i = 0;
        
        if (query_as_str.empty())
            return {};
        
        if (query_as_str[0] == '?')
            i = 1;

        while ( true )
        {
            i = query_as_str.find('=', p+1);
            
            if (i == std::string::npos)
            {
                fmt::print("\n");
                query.try_emplace(std::string{query_as_str.substr(p, i-p)},
                        ""
                );
                break;
            }
            auto j = query_as_str.find('&', i+1);
            
            if (j == std::string::npos)
            {
                query.try_emplace(std::string{query_as_str.substr(p, i-p)},
                        decode(std::string{query_as_str.substr(i+1)})
                );
                break;
            }
            else
            {
                query.try_emplace(std::string{query_as_str.substr(p, i-p)},
                        decode(std::string{query_as_str.substr(i+1, j-i-1)})
                );
            }
            p = j+1;
        }
        
       
        return query;
    }

    Url::Url(const std::string& url)
        : hUrl(curl_url())
    {
        auto code = curl_url_set(hUrl, CURLUPART_URL, url.c_str(), 0);
        
        if (code != CURLUE_OK){
            throw_error_from_curl_msg(code, fmt::format("Invalid url: '{}'", url));
        }
    }

    Url& curlpp::Url::operator=(const Url& url)
    {
        hUrl = curl_url_dup(url.hUrl);
        return *this;
    }

    curlpp::Url::Url(Url && url) noexcept
    {
        std::swap(hUrl, url.hUrl);
    }

    Url& curlpp::Url::operator=(Url && url) noexcept
    {
        std::swap(hUrl, url.hUrl);
        return *this;
    }

    Url::Url(const Url& url)
        : hUrl(curl_url_dup(url.hUrl))
    {
    }

    Url::~Url()
    {
        if (hUrl) curl_url_cleanup(hUrl);
    }

    std::unordered_map<std::string,std::string> curlpp::Url::get_query() const
    {
        auto querystr = get_part(CURLUPart::CURLUPART_QUERY);
        if (querystr)
            return str_to_query(*querystr);
        else
            return {};
    }

    void curlpp::Url::set_query(const std::unordered_map<std::string,std::string>& map)
    {
        std::string querystr;
        
        for(const auto& [key, value] : map)
        {
            querystr += key;
            querystr += '=';
            querystr += value;
            querystr += '&';
        }

        if (querystr.size()>0) querystr.resize(querystr.size()-1);

        set_part(CURLUPart::CURLUPART_QUERY, querystr.c_str());
    }

    std::optional<std::string> Url::get_part(CURLUPart part) const
    {
        char* buffer;
        auto code = curl_url_get(hUrl, part, &buffer, 0);
        assert(code != CURLUE_BAD_PARTPOINTER &&
            code != CURLUE_UNKNOWN_PART);

        if (code >= CURLUE_NO_SCHEME && code <= CURLUE_NO_ZONEID)
        {
            return {};
        }
        else if (code != CURLUE_OK)
        {
            throw std::runtime_error{ fmt::format("CURLU error: {}", static_cast<int>(code)) };
        }
        std::string host_str{ buffer };
        curl_free(buffer);
        return host_str;
    }

    void curlpp::Url::set_part(CURLUPart part, const std::string& value)
    {
        auto code = curl_url_set(hUrl, part, value.c_str(), 0);
        if (code) throw_error_from_curl(code);
    }

    void Url::add_query(const std::string& param, const std::string& value)
    {
        auto query = fmt::format("{}={}", param, value);
        auto code = curl_url_set(hUrl, CURLUPART_QUERY, query.c_str(), CURLU_APPENDQUERY | CURLU_URLENCODE);
        if (code != CURLUE_OK)
        {
            throw_error_from_curl(code);
        }
    }

    Url Url::operator+(std::string_view part) const
    {
        auto newUrl = *this;
        newUrl += part;
        return newUrl;
    }

    Url Url::operator+=(std::string_view part)
    {
        auto i = part.find('?');
        auto newPath = i == std::string::npos ? "" : part.substr(0, i);
        auto newQuery = i == std::string::npos ? "" : part.substr(i+1);
        auto path = get_path().value_or("/");
        
        // merge paths
        if (!path.empty() && path[path.size()] == '/')
        {
            path.resize(path.size()-1);
        }
        if (!newPath.empty() && newPath[0] != '/')
        {
            path += '/';
        }
        path += newPath;
        set_path(path);

        // merge query
        for (const auto& [key,value] : str_to_query(newQuery))
        {
            add_query(key, value);
        }

        return *this;
    }
    
}
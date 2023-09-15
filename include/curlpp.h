#pragma once

#include <string>
#include <curl/curl.h>
#include <optional>
#include <unordered_map>

namespace curlpp
{
    class Client
    {
    public:
        ~Client();
    public:
    private:
        CURL* hCurl = curl_easy_init();
    };

    std::string encode(const std::string& input);
    std::string decode(const std::string& input);

    std::unordered_map<std::string, std::string> str_to_query(std::string_view query_as_str);

    class Url
    {
    public:
        explicit Url(const std::string& url);
        Url& operator=(const Url& url);
        Url(const Url& url);
        Url& operator=(Url&& url) noexcept;
        Url(Url&& url) noexcept;
        ~Url();
        std::optional<std::string> get_host() const { return get_part(CURLUPart::CURLUPART_HOST); }
        std::optional<std::string> get_prot() const { return get_part(CURLUPart::CURLUPART_SCHEME); }
        std::optional<std::string> get_user() const { return get_part(CURLUPart::CURLUPART_USER); }
        std::optional<std::string> get_pass() const { return get_part(CURLUPart::CURLUPART_PASSWORD); }
        std::optional<std::string> get_option() const { return get_part(CURLUPart::CURLUPART_OPTIONS); }
        std::optional<std::string> get_port() const { return get_part(CURLUPart::CURLUPART_PORT); }
        std::optional<std::string> get_path() const { return get_part(CURLUPart::CURLUPART_PATH); }
        std::unordered_map<std::string, std::string> get_query() const;
        std::optional<std::string> get_frag() const { return get_part(CURLUPart::CURLUPART_FRAGMENT); }

        void set_host(const std::string& value) { set_part(CURLUPart::CURLUPART_HOST, value); }
        void set_prot(const std::string& value) { set_part(CURLUPart::CURLUPART_SCHEME, value); }
        void set_user(const std::string& value) { set_part(CURLUPart::CURLUPART_USER, value); }
        void set_pass(const std::string& value) { set_part(CURLUPart::CURLUPART_PASSWORD, value); }
        void set_option(const std::string& value) { set_part(CURLUPart::CURLUPART_OPTIONS, value); }
        void set_port(uint16_t value) { set_part(CURLUPart::CURLUPART_PORT, std::to_string(value)); }
        void set_path(const std::string& value) { set_part(CURLUPart::CURLUPART_PATH, value); }
        void set_query(const std::unordered_map<std::string, std::string>& value);
        void set_frap(const std::string& value) { set_part(CURLUPart::CURLUPART_FRAGMENT, value); }

        void add_query(const std::string& param, const std::string& value);

        std::optional<std::string> to_string() const { return get_part(CURLUPart::CURLUPART_URL); }

        Url operator+(std::string_view part) const;
        Url operator+=(std::string_view part);
    private:
        std::optional<std::string> get_part(CURLUPart part) const;
        void set_part(CURLUPart part, const std::string& value);
    private:
        CURLU* hUrl = nullptr;
    };

    //TODO: Create a exception that can hold the error code

    #define throw_error_from_curl(rc) \
        throw std::runtime_error{ fmt::format("{}({}:{}). CURLU returned: {}",\
            __FUNCTION__, \
            __FILE__, \
            __LINE__, \
            rc) }
    
    #define throw_error_from_curl_msg(rc, msg) \
        throw std::runtime_error{ fmt::format("{}({}:{}). {}. CURLU returned: {}",\
            __FUNCTION__, \
            __FILE__, \
            __LINE__, \
            msg, \
            rc) }

}
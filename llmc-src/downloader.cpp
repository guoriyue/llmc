#include "downloader.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outfile = static_cast<std::ofstream*>(userp);
    size_t total_size = size * nmemb;
    outfile->write(static_cast<const char*>(contents), total_size);
    return total_size;
}

bool downloader::download_file(const std::string& url, const std::string& file_path) {
    CURL* curl;
    CURLcode res;
    std::ofstream out_file(file_path, std::ios::binary);

    if (!out_file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);
        
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            out_file.close();
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_cleanup(curl);
    }

    out_file.close();
    return true;  // Success
}

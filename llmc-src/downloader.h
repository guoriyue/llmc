#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

class downloader {
public:
    static bool download_file(const std::string& url, const std::string& file_path);
};

#endif // DOWNLOADER_H

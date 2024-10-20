#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

bool download_file(const std::string& url, const std::string& file_path);
void show_progress_bar(double percentage, double speed);
bool file_exists(const std::string & path);
bool file_is_empty(const std::string & path);
bool file_create(const std::string & path);
bool json_file_create(const std::string & path);
#endif // DOWNLOADER_H

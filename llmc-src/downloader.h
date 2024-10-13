#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

bool download_file(const std::string& url, const std::string& file_path);
void show_progress_bar(double percentage, double speed);


#endif // DOWNLOADER_H

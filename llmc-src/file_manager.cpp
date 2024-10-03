#include "file_manager.h"
#include <iostream>
#include <sys/stat.h>  // For mkdir, stat
#include <cstring>     // For strerror
#include <cstdlib>     // For getenv

bool file_manager::create_directory_if_not_exists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == -1) {
        if (mkdir(path.c_str(), 0755) != 0) {
            std::cerr << "Failed to create directory: " << path << " - " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

std::string file_manager::get_cache_directory(const std::string& cmd_name) {
    const char* home_dir = std::getenv("HOME");
    if (!home_dir) {
        std::cerr << "Unable to locate HOME directory." << std::endl;
        exit(1);
    }

    std::string cache_dir = std::string(home_dir) + "/.cache";
    std::string cmd_dir = cache_dir + "/" + cmd_name;

    if (!create_directory_if_not_exists(cache_dir)) {
        exit(1);
    }

    if (!create_directory_if_not_exists(cmd_dir)) {
        exit(1);
    }

    return cmd_dir;
}

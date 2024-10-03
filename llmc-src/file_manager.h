#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>

class file_manager {
public:
    static bool create_directory_if_not_exists(const std::string& path);
    static std::string get_cache_directory(const std::string& cmd_name);
};

#endif // FILE_MANAGER_H

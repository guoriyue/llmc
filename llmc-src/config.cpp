#include "config.h"
#include "file_manager.h"
#include <fstream>
#include <string>


std::string config::read_model_path() {
    std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
    std::ifstream infile(config_path);
    std::string model_path;
    if (infile.good()) {
        std::getline(infile, model_path);
    }
    return model_path;
}

void config::save_model_path(const std::string& path) {
    std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
    std::ofstream outfile(config_path);
    outfile << path;
}

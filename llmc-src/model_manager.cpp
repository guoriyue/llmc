#include "model_manager.h"
#include "config.h"
#include "file_manager.h"
#include "downloader.h"
#include "common.h"
#include "console.h"

#include "json.hpp"
#include <sys/stat.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <string>

#include <fstream>
#include <string>

#include <sys/stat.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <string>

#ifdef _WIN32
    #include <conio.h>  // Windows specific for _getch()
#else
    #include <termios.h>  // POSIX terminal control
    #include <unistd.h>   // For STDIN_FILENO
#endif
#include <cstdio>

// Function to move the cursor up by a given number of lines
static void move_cursor_up(int lines) {
    std::cout << "\033[" << lines << "A";
}

// Function to clear the current line
static void clear_line() {
    std::cout << "\033[2K\r";
}

// // Cross-platform getch function
// static int cp_getch() {
// #ifdef _WIN32
//     return _getch();  // Use _getch() on Windows
// #else
//     struct termios oldt, newt;
//     int ch;
//     tcgetattr(STDIN_FILENO, &oldt);  // Get current terminal settings
//     newt = oldt;
//     newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply new terminal settings
//     ch = getchar();  // Read one character
//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore old terminal settings
//     return ch;
// #endif
// }

// Function to manually turn echo back on
static void enable_echo() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);  // Get current terminal settings
    t.c_lflag |= ECHO;  // Enable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  // Apply the settings
}

// Function to disable echo if needed (optional helper)
static void disable_echo() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);  // Get current terminal settings
    t.c_lflag &= ~ECHO;  // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  // Apply the settings
}

void model_manager::print_models(const std::vector<std::string>& models, std::size_t current_choice) {
    // Move the cursor up by the number of model lines to overwrite them
    if (first_call_print_models) {
        first_call_print_models = false;
    } else {
        for (std::size_t i = 0; i < models.size(); ++i) {
            std::cout << "\033[F";
        }
    }
    for (std::size_t i = 0; i < models.size(); ++i) {
        if (i == current_choice) {
            // Highlight the current choice (reverse video mode)
            std::cout << "\033[7m" << models[i] << "\033[0m" << std::endl;
        } else {
            std::cout << models[i] << std::endl;
        }
    }
}


std::string model_manager::read_model_path() {
    // std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
    std::string config_path = fs_get_cache_file(CONFIG_FILE);
    std::ifstream infile(config_path);
    if (!infile.is_open()) {
        std::cout << "Error: Unable to open the config file."<< std::endl;
        return "";
    }
    nlohmann::json config_dict;
    infile >> config_dict;  // The >> operator automatically parses the JSON file content
    infile.close();  // Close the file after reading
    return config_dict["model_path"];
    // std::string model_path;
    // if (infile.good()) {
    //     std::getline(infile, model_path);
    // }
    // return model_path;
}

void model_manager::save_model_path(const std::string& path) {
    // std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
    std::string config_path = fs_get_cache_file(CONFIG_FILE);
    std::ofstream outfile(config_path);
    nlohmann::json config_dict;
    config_dict["model_path"] = path;
    if (outfile.is_open()) {
        outfile << config_dict.dump(4);  // Pretty print with 4 spaces
        outfile.close();
    } else {
        std::cout << "Error: Unable to open the config file." << std::endl;
    }
    // std::ofstream outfile(config_path);
    // outfile << path;
}

// std::string model_manager::get_cached_model() {
//     std::string model_path = read_model_path();
//     if (model_path.empty()) {
//         std::cout << "No cached model found. Please set a model first." << std::endl;
//     }
//     return model_path;
// }

std::size_t model_manager::choose_model() {
    std::size_t choice = 0;
    int key;
    disable_echo();
    // Print the initial model list
    print_models(models_to_choose, choice);
    while (true) {
        key = console::getchar32();
        if (key == 27) { // Escape sequence starts with 27 (ESC)
            key = console::getchar32(); // Skip the '[' character
            key = console::getchar32(); // Get the actual arrow key

            switch (key) {
                case 'A': // Up arrow key
                    if (choice > 0) {
                        choice--;
                    }
                    break;
                case 'B': // Down arrow key
                    if (choice < models_to_choose.size() - 1) {
                        choice++;
                    }
                    break;
            }
        } else if (key == 10) { // Enter key
            return choice;
        }

        // Reprint the model list with the updated selection
        print_models(models_to_choose, choice);
    }
    enable_echo();
}

std::string model_manager::set_model() {
    enable_echo();
    int chosen = choose_model();
    if (models_to_choose[chosen] == "custom"){
        std::string custom_model_path;
        std::string custom_prompt;
        
        // Ask the user for custom model path
        std::cout << "Please enter the local path to the custom model: \n";
        // std::getline(std::cin, custom_model_path);
        std::cin >> custom_model_path;
        std::cout << "Please enter your custom prompt: \n";
        // std::getline(std::cin, custom_prompt);
        std::cin >> custom_prompt;
        return custom_model_path;
    } else {
        std::string chosen_model = models_to_choose[chosen];
        std::string chosen_model_url = model_urls[chosen];
        size_t last_slash = chosen_model_url.find_last_of('/');
    
        // Get the substring after the last '/'
        std::string file_name = (last_slash != std::string::npos) ? chosen_model_url.substr(last_slash + 1) : chosen_model_url;
    
        std::string chosen_model_path = fs_get_cache_file(file_name);
        printf("before download\n");
        // download the model
        bool download_model = downloader::download_file(chosen_model_url, chosen_model_path);
        if (!download_model) {
            std::cout << "Error: Could not download the model." << std::endl;
            return "";
        }
        printf("download_model = %d\n", download_model);
        printf("chosen_model_path = %s\n", chosen_model_path.c_str());
        save_model_path(chosen_model_path);
        return chosen_model_path;
    }
}

// std::string model_manager::get_valid_model_path(const std::string& cache_dir, const std::string& model_url, const std::string& model_file_name) {
//     std::string model_path = read_model_path();
//     if (model_path.empty()) {
//         model_path = cache_dir + "/" + model_file_name;
//         std::cout << "Downloading model.\n";
//         if (downloader::download_file(model_url, model_path)) {
//             std::cout << "Model downloaded successfully.\n";
//         } else {
//             std::cout << "Model download failed.\n";
//             exit(1);
//         }
//     } else {
//         // check if the model file exists
//         struct stat buffer;
//         if (stat(model_path.c_str(), &buffer) != 0) {
//             std::cerr << "Model file not found: " << model_path << std::endl;
//             model_path = cache_dir + "/" + model_file_name;
//             std::cout << "Downloading model.\n";
//             if (downloader::download_file(model_url, model_path)) {
//                 std::cout << "\nModel downloaded successfully.\n";
//             } else {
//                 std::cout << "Model download failed.\n";
//                 exit(1);
//             }
//         }
//     }
//     save_model_path(model_path);
//     return model_path;
// }


// void model_manager::set_default_model() {
//     std::string config_path = file_manager::get_cache_directory(CMD_NAME) + "/" + CONFIG_FILE;
//     std::ofstream outfile(config_path);
//     outfile << "";
// }
#include "model_manager.h"
#include "config.h"
#include "file_manager.h"
#include "downloader.h"
#include <sys/stat.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
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
void move_cursor_up(int lines) {
    std::cout << "\033[" << lines << "A";
}

// Function to clear the current line
void clear_line() {
    std::cout << "\033[2K\r";
}

// Cross-platform getch function
int cp_getch() {
#ifdef _WIN32
    return _getch();  // Use _getch() on Windows
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);  // Get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply new terminal settings
    ch = getchar();  // Read one character
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore old terminal settings
    return ch;
#endif
}

// Show help command
void model_manager::show_help() {
    std::cout << "Usage: " + std::string(CMD_NAME) + " [options]\n"
              << "Options:\n"
              << "  -h           Show this help message\n"
              << "  -set-model   Select a model using arrow keys\n"
              << "  -n [tokens]  Set maximum number of tokens\n"
              << "  -s           Show explanation for the generated command\n";
}

void model_manager::print_models(const std::vector<std::string>& models, int current_choice) {
    // Move the cursor up by the number of model lines to overwrite them
    if (first_call_print_models) {
        first_call_print_models = false;
    } else {
        for (int i = 0; i < models.size(); ++i) {
            std::cout << "\033[F";
        }
    }
    for (int i = 0; i < models.size(); ++i) {
        if (i == current_choice) {
            // Highlight the current choice (reverse video mode)
            std::cout << "\033[7m" << models[i] << "\033[0m" << std::endl;
        } else {
            std::cout << models[i] << std::endl;
        }
    }
}


std::string model_manager::set_model() {
    int choice = 0;
    int key;

    // Print the initial model list
    print_models(models_to_choose, choice);

    while (true) {
        key = cp_getch(); // Cross-platform getch() for keypress
        printf("                   key: %d\n", key);
        if (key == 27) { // Escape sequence starts with 27 (ESC)
            key = cp_getch(); // Skip the '[' character
            key = cp_getch(); // Get the actual arrow key

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
            if (models_to_choose[choice] == "custom") {
                std::string custom_model_path;
                std::string custom_prompt;

                // Ask the user for custom model path
                std::cout << "Please enter the local path to the custom model: ";
                std::getline(std::cin, custom_model_path);
                std::cout << "Please enter your custom prompt: ";
                std::getline(std::cin, custom_prompt);

                return custom_model_path;
            } else {
                return models_to_choose[choice];
            }
        }

        // Reprint the model list with the updated selection
        print_models(models_to_choose, choice);
    }
}

// Show explanation or not
void model_manager::set_show_explanation(bool show) {
    show_explanation_flag = show;
}

std::string model_manager::get_valid_model_path(const std::string& cache_dir, const std::string& model_url, const std::string& model_file_name) {
    std::string model_path = config::read_model_path();
    if (model_path.empty()) {
        model_path = cache_dir + "/" + model_file_name;
        std::cout << "Downloading model.\n";
        if (downloader::download_file(model_url, model_path)) {
            std::cout << "Model downloaded successfully.\n";
        } else {
            std::cout << "Model download failed.\n";
            exit(1);
        }
    } else {
        // check if the model file exists
        struct stat buffer;
        if (stat(model_path.c_str(), &buffer) != 0) {
            std::cerr << "Model file not found: " << model_path << std::endl;
            model_path = cache_dir + "/" + model_file_name;
            std::cout << "Downloading model.\n";
            if (downloader::download_file(model_url, model_path)) {
                std::cout << "\nModel downloaded successfully.\n";
            } else {
                std::cout << "Model download failed.\n";
                exit(1);
            }
        }
    }
    config::save_model_path(model_path);
    return model_path;
}

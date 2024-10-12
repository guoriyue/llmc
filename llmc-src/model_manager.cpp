#include "model_manager.h"
#include "config.h"
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


// Enable raw mode to capture key presses without echoing them automatically
void enable_raw_mode(termios &orig_termios) {
    termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);  // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Disable raw mode and restore original terminal settings
void disable_raw_mode(termios &orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Handle special keys like arrow keys
int handle_escape_sequence() {
    int ch = getchar(); // This should be '[' for arrow keys, or 'b'/'f' for Alt+B/Alt+F
    if (ch == '[') {
        ch = getchar(); // Now read the actual direction
        switch (ch) {
            case 'A': return 'A'; // Up arrow (not used in this case)
            case 'B': return 'B'; // Down arrow (not used in this case)
            case 'C': return 'C'; // Right arrow
            case 'D': return 'D'; // Left arrow
        }
    } else if (ch == 'b') {  // Alt+B (move back one word)
        return 'b';
    } else if (ch == 'f') {  // Alt+F (move forward one word)
        return 'f';
    }
    return 0;
}

// Function to reprint the string and adjust cursor position
void refresh_line(const std::string &input, int pos) {
    std::cout << "\r\033[K" << input;  // Clear line and reprint the updated string
    // Move cursor back to the correct position
    std::cout << "\033[" << (input.length() - pos) << "D";
}

// Move back by one word (for Alt+B)
void move_back_one_word(const std::string &input, int &pos) {
    while (pos > 0 && std::isspace(input[pos - 1])) {
        pos--;  // Skip whitespace
    }
    while (pos > 0 && std::isalnum(input[pos - 1])) {
        pos--;  // Skip the word
    }
    refresh_line(input, pos);
}

// Move forward by one word (for Alt+F)
void move_forward_one_word(const std::string &input, int &pos) {
    while (pos < input.length() && std::isspace(input[pos])) {
        pos++;  // Skip whitespace
    }
    while (pos < input.length() && std::isalnum(input[pos])) {
        pos++;  // Skip the word
    }
    refresh_line(input, pos);
}

// Function to allow inline editing of prefilled input with cursor movement
std::string edit_prefilled_input(const std::string &prefilled_text) {
    std::string input = prefilled_text;
    int pos = input.length();  // Cursor position starts at the end of the prefilled text
    int ch;

    // Save the original terminal settings
    termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);

    // Enable raw mode for capturing character-by-character input
    enable_raw_mode(orig_termios);

    // Print the prefilled text
    std::cout << prefilled_text;
    std::cout.flush();

    // Read input character by character
    while ((ch = getchar()) != '\n') {
        if (ch == 127 || ch == '\b') {  // Handle backspace
            if (pos > 0) {
                input.erase(--pos, 1);  // Remove character at cursor
                refresh_line(input, pos);
            }
        } else if (ch == 27) {  // Escape character (arrow keys or Alt+B/F)
            int key = handle_escape_sequence();
            if (key == 'C' && pos < input.length()) {  // Right arrow
                std::cout << "\033[C";  // Move cursor right
                pos++;
            } else if (key == 'D' && pos > 0) {  // Left arrow
                std::cout << "\033[D";  // Move cursor left
                pos--;
            } else if (key == 'b' && pos > 0) {  // Alt+B (move back one word)
                move_back_one_word(input, pos);
            } else if (key == 'f' && pos < input.length()) {  // Alt+F (move forward one word)
                move_forward_one_word(input, pos);
            }
        } else if (ch == 1) {  // CTRL+A (move to start of line)
            pos = 0;
            refresh_line(input, pos);
        } else if (ch == 5) {  // CTRL+E (move to end of line)
            pos = input.length();
            refresh_line(input, pos);
        } else if (ch >= 32 && ch <= 126) {  // Handle printable characters
            input.insert(pos, 1, ch);  // Insert character at cursor position
            pos++;  // Move cursor forward
            refresh_line(input, pos);  // Reprint the string and move the cursor to the correct position
        }
        std::cout.flush();
    }

    // Restore the original terminal settings
    disable_raw_mode(orig_termios);
    
    std::cout << std::endl;  // Move to the next line after user presses Enter
    return input;
}

std::string trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

void model_manager::show_config() {
    std::string config_path = fs_get_cache_file(CONFIG_FILE);
    std::ifstream
        infile(config_path);
    if (!infile.is_open()) {
        std::cout << "Error: Unable to open the config file." << std::endl;
        return;
    }
    nlohmann::json config_dict;
    infile >> config_dict;  // The >> operator automatically parses the JSON file content
    infile.close();  // Close the file
    for (auto it = config_dict.begin(); it != config_dict.end(); ++it) {
        std::cout << it.key() << ": " << it.value() << std::endl;
    }
    // std::string model_path = config_dict["model_path"];
    // std::cout << "Current model path: " << model_path << std::endl;
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
}

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
            enable_echo();
            return choice;
        }

        // Reprint the model list with the updated selection
        print_models(models_to_choose, choice);
    }
    enable_echo();
}



std::string model_manager::set_model() {
    int chosen = choose_model();
    // enable_echo();
    if (models_to_choose[chosen] == "custom"){
        std::string custom_model_path;
        // std::string custom_prompt;
        
        // Ask the user for custom model path
        std::cout << "Please enter the local path to the custom model: \n";
        // std::getline(std::cin, custom_model_path);
        // std::cin >> custom_model_path;
        custom_model_path = edit_prefilled_input("");
        custom_model_path = trim(custom_model_path);
        // bool another_line = console::readline(custom_model_path, false);
        // std::cout << "Please enter your custom prompt: \n";
        // // std::getline(std::cin, custom_prompt);
        // std::cin >> custom_prompt;
        printf("custom_model_path: %s\n", custom_model_path.c_str());
        save_model_path(custom_model_path);
        return custom_model_path;
    } else {
        std::string chosen_model = models_to_choose[chosen];
        std::string chosen_model_url = model_urls[chosen];
        size_t last_slash = chosen_model_url.find_last_of('/');
    
        // Get the substring after the last '/'
        std::string file_name = (last_slash != std::string::npos) ? chosen_model_url.substr(last_slash + 1) : chosen_model_url;
    
        std::string chosen_model_path = fs_get_cache_file(file_name);
        // download the model
        bool download_model = downloader::download_file(chosen_model_url, chosen_model_path);
        if (!download_model) {
            std::cout << "Error: Could not download the model." << std::endl;
            return "";
        }
        save_model_path(chosen_model_path);
        return chosen_model_path;
    }
}

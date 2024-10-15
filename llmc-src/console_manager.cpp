// #include "config.h"
// #include "downloader.h"
#include "console_manager.h"
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
void move_cursor_up(int lines) {
    std::cout << "\033[" << lines << "A";
}

// Function to clear the current line
void clear_line() {
    std::cout << "\033[2K\r";
}

// Function to manually turn echo back on
void enable_echo() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);  // Get current terminal settings
    t.c_lflag |= ECHO;  // Enable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);  // Apply the settings
}

// Function to disable echo if needed (optional helper)
void disable_echo() {
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
void refresh_line(const std::string &input, size_t pos) {
    std::cout << "\r\033[K" << input;  // Clear line and reprint the updated string
    // Move cursor back to the correct position
    std::cout << "\033[" << (input.length() - pos) << "D";
}

// Move back by one word (for Alt+B)
void move_back_one_word(const std::string &input, size_t &pos) {
    while (pos > 0 && std::isspace(input[pos - 1])) {
        pos--;  // Skip whitespace
    }
    while (pos > 0 && std::isalnum(input[pos - 1])) {
        pos--;  // Skip the word
    }
    refresh_line(input, pos);
}

// Move forward by one word (for Alt+F)
void move_forward_one_word(const std::string &input, size_t &pos) {
    while (pos < input.length() && std::isspace(input[pos])) {
        pos++;  // Skip whitespace
    }
    while (pos < input.length() && std::isalnum(input[pos])) {
        pos++;  // Skip the word
    }
    refresh_line(input, pos);
}

// Function to allow inline editing of prefilled input with cursor movement
std::string edit_prefilled_input_single_line(const std::string &prefilled_text) {
    std::string input = prefilled_text;
    size_t pos = input.length();  // Cursor position starts at the end of the prefilled text
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


void print_centered_message(const char* message, int total_length) {
    int message_length = strlen(message);
    
    // Ensure the message is not longer than the total length
    if (message_length >= total_length) {
        printf("%s\n", message); // Print the message directly if it's too long
        return;
    }

    // Calculate how many '=' signs will go on each side
    int padding = (total_length - message_length - 2); // 2 accounts for the spaces around the message
    int half_padding = padding / 2;
    
    // Print the left side '=' signs
    for (int i = 0; i < half_padding; i++) {
        printf("=");
    }

    // Print the message with spaces
    printf(" %s ", message);

    // Print the right side '=' signs (adjusting for odd number of total_length)
    for (int i = 0; i < (padding - half_padding); i++) {
        printf("=");
    }

    // End the line with a newline character
    printf("\n");
}


// void print_vector(const std::vector<std::string>& to_choose, std::size_t current_choice, bool first_call_print) {
//     // Move the cursor up by the number of model lines to overwrite them
//     if (!first_call_print) {
//         for (std::size_t i = 0; i < to_choose.size(); ++i) {
//             std::cout << "    " << "\033[F";
//         }
//     }
//     for (std::size_t i = 0; i < to_choose.size(); ++i) {
//         if (i == current_choice) {
//             // Highlight the current choice (reverse video mode)
//             std::cout << "[>] " << "\033[7m" << to_choose[i] << "\033[0m" << std::endl;
//         } else {
//             std::cout << "    " << to_choose[i] << std::endl;
//         }
//     }
// }

// size_t choose_from_vector(const std::vector<std::string> &to_choose) {
//     std::size_t choice = 0;
//     int key;
//     disable_echo();
//     // Print the initial model list
//     print_vector(to_choose, choice, true);
//     while (true) {
//         key = console::getchar32();
//         if (key == 27) { // Escape sequence starts with 27 (ESC)
//             key = console::getchar32(); // Skip the '[' character
//             key = console::getchar32(); // Get the actual arrow key

//             switch (key) {
//                 case 'A': // Up arrow key
//                     if (choice > 0) {
//                         choice--;
//                     }
//                     break;
//                 case 'B': // Down arrow key
//                     if (choice < to_choose.size() - 1) {
//                         choice++;
//                     }
//                     break;
//             }
//         } else if (key == 10) { // Enter key
//             enable_echo();
//             return choice;
//         }

//         // Reprint the model list with the updated selection
//         print_vector(to_choose, choice, false);
//     }
//     enable_echo();
// }


// Helper function to count the number of lines in a string
std::size_t count_lines(const std::string& str) {
    std::size_t count = 0;
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        ++count;
    }
    return count;
}

// Function to print the vector with the current choice highlighted
void print_vector(const std::vector<std::string>& to_choose, std::size_t current_choice, bool first_call_print) {
    // Move the cursor up by the number of lines to overwrite them
    if (!first_call_print) {
        // Calculate total lines that were printed in the previous call
        std::size_t total_lines = 0;
        for (const auto& item : to_choose) {
            total_lines += count_lines(item);  // Each item is followed by a newline
        }

        // Move up by the total number of lines and clear each line
        for (std::size_t i = 0; i < total_lines; ++i) {
            std::cout << "\033[F\033[K";  // Move up and clear the line
        }
    }

    // Print the vector items
    for (std::size_t i = 0; i < to_choose.size(); ++i) {
        if (i == current_choice) {
            // Highlight the current choice (reverse video mode)
            std::cout << "[>] \033[7m" << to_choose[i] << "\033[0m" << std::endl;
        } else {
            std::cout << "    " << to_choose[i] << std::endl;
        }
    }
}

size_t choose_from_vector(const std::vector<std::string> &to_choose) {
    std::size_t choice = 0;
    int key;
    disable_echo();  // Assuming this function disables input echo in terminal

    // Print the initial model list
    print_vector(to_choose, choice, true);
    
    while (true) {
        key = console::getchar32();  // Assuming this reads a character input
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
                    if (choice < to_choose.size() - 1) {
                        choice++;
                    }
                    break;
            }
        } else if (key == 10) { // Enter key
            enable_echo();  // Re-enable echo before returning
            return choice;
        }

        // Reprint the model list with the updated selection
        print_vector(to_choose, choice, false);
    }
    
    enable_echo();  // Just in case
}
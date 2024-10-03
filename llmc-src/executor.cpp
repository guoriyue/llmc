#include "executor.h"
#include "config.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <cctype>  // For checking if characters are alphanumeric

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

void executor::execute_embedded_executable(const std::string& model_path, int argc, char* argv[]) {
    std::string exec_path = "./llama_cpp/llama-cli";
    std::string command = exec_path;
    bool print_mode = false;

    if (argc == 2) {
        std::string prompt = argv[1];
        prompt = std::string(PROMPT_CMD_GEN) + '\n' + prompt;
        printf("prompt: %s\n", prompt.c_str());
        command += " -m " + model_path + " -p \"" + prompt + "\"" ;
    } else if (argc == 3) {
        std::string prompt = argv[1];
        std::string num_tokens = argv[2];
        prompt = std::string(PROMPT_CMD_GEN) + '\n' + prompt;
        printf("prompt: %s\n", prompt.c_str());
        command += " -m " + model_path + " -p \"" + prompt + "\"";
    } 
    else {
        std::cerr << "Please provide a prompt. For example: llmc \"sort files by size\"" << std::endl;
        std::cerr << "Usage: llmc <prompt>" << std::endl;
        exit(1);
    }


    
    
    command += " 2>&1"; // redirect stderr to stdout

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command: " << strerror(errno) << std::endl;
        exit(1);
    }

    char buffer[1024];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        if (line.find("llama_perf_sampler_print:") != std::string::npos) {
            print_mode = false;
        }

        // Always check for "error"
        if (line.find("error") != std::string::npos || print_mode) {
            output += line;
        }

        if (line.find("generate:") != std::string::npos) {
            print_mode = true;
        }
    }
    // std::string prefilled_text = "git add -A && git commit -m \"Initial commit\" && git push origin master";
    
    std::string result = edit_prefilled_input(output);
    
    std::cout << "You entered: " << result << std::endl;

    pclose(pipe);
}

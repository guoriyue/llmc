#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <string>
#include <termios.h>

// Function declarations

// Function to move the cursor up by a given number of lines
void move_cursor_up(int lines);

// Function to clear the current line
void clear_line();

// Function to enable echo in the terminal
void enable_echo();

// Function to disable echo in the terminal
void disable_echo();

// Function to enable raw mode for capturing key presses without echoing
void enable_raw_mode(termios &orig_termios);

// Function to disable raw mode and restore original terminal settings
void disable_raw_mode(termios &orig_termios);

// Handle special keys like arrow keys and Alt+B/F
int handle_escape_sequence();

// Function to reprint the string and adjust the cursor position
void refresh_line(const std::string &input, size_t pos);

// Move back by one word (for Alt+B)
void move_back_one_word(const std::string &input, size_t &pos);

// Move forward by one word (for Alt+F)
void move_forward_one_word(const std::string &input, size_t &pos);

// Function to allow inline editing of prefilled input with cursor movement
std::string edit_prefilled_input(const std::string &prefilled_text);

// Function to trim leading and trailing whitespace from a string
std::string trim(const std::string &str);

size_t choose_from_vector(const std::vector<std::string> &to_choose);

void print_vector(const std::vector<std::string>& to_choose, std::size_t current_choice, bool first_call_print);

void print_centered_message(const char* message, int total_length);
#endif // INPUT_MANAGER_H

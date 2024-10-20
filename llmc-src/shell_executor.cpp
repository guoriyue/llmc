#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include "shell_executor.h"
#include "console_manager.h"
#include "config.h"

std::string exec_command(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;

    // Use popen() to open the pipe for reading command output
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // Read the output from the command, chunk by chunk
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        // Append to the result
        result += buffer.data();
        
        // Print the chunk immediately to ensure real-time output
        std::cout << buffer.data() << std::flush;
    }

    return result;
}

void edit_exec(std::string output_line, bool edit) {
    if (edit) {
        // print_centered_message("Edit & Execute", PRINT_LENGTH);
        // std::string edited_cmd =  edit_prefilled_input(output_line);
        std::string edited_cmd = edit_prefilled_input_multiline(output_line);
        exec_command(edited_cmd);
    } else {
        exec_command(output_line);
    }
}
void choose_edit_exec(std::vector<std::string>& output_lines) {
    print_centered_message("Enter: execute | e: edit | q: quit", PRINT_LENGTH);
    std::pair<size_t, int> chosen_cmd = choose_from_vector_with_eq(output_lines);
    if (chosen_cmd.second == 10) {
        edit_exec(output_lines[chosen_cmd.first], false);
    } else if (chosen_cmd.second == static_cast<int>('e')) {
        edit_exec(output_lines[chosen_cmd.first], true);
    } else if (chosen_cmd.second == static_cast<int>('q')) {
        return;
    }
}
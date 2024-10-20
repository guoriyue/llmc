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
    // Read the output from the command, line by line
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    printf("%s\n", result.c_str());
    return result;
}

void edit_exec(std::string output_line, bool no_edit) {
    if (!no_edit) {
        print_centered_message("Edit & Execute", PRINT_LENGTH);
        std::string edited_cmd =  edit_prefilled_input(output_line);
        exec_command(edited_cmd);
    } else {
        exec_command(output_line);
    }
}
void choose_edit_exec(std::vector<std::string>& output_lines, bool no_edit) {
    print_centered_message("Choose a Command", PRINT_LENGTH);
    size_t chosen_cmd = choose_from_vector(output_lines);
    edit_exec(output_lines[chosen_cmd], no_edit);
}
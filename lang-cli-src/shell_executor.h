#ifndef SHELL_EXECUTOR_H
#define SHELL_EXECUTOR_H

#include <string>

// Declare the function prototype
std::string exec_command(const std::string& cmd);
void choose_edit_exec(std::vector<std::string>& output_lines);
#endif // SHELL_EXECUTOR_H

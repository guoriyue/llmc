#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "shell_executor.h"

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
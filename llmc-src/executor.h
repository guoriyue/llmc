#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>

class executor {
public:
    // static void execute_embedded_executable(const std::string& model_path, int argc, char* argv[]);
    static std::string edit_prefilled_input(const std::string &prefilled_text);
};

#endif // EXECUTOR_H

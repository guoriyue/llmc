#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>

class executor {
public:
    static void execute_embedded_executable(const std::string& model_path, int argc, char* argv[]);
};

#endif // EXECUTOR_H

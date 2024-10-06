#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <string>
#include <vector>

class model_manager {
public:
    std::string model_path;
    std::string prompt;
    std::vector<std::string> models_to_choose;
    bool first_call_print_models = true;

    // Constructor
    model_manager() {
        init_models();
    }

    // Function to initialize the models vector
    void init_models() {
        models_to_choose.push_back("llama3.2-1b");
        models_to_choose.push_back("custom");
    }

    std::string get_valid_model_path(const std::string& cache_dir, const std::string& model_url, const std::string& model_file_name);
    std::string set_model();
    int choose_model();
    std::string get_cached_model();
    std::string read_model_path();
    void save_model_path(const std::string& path);
    void print_models(const std::vector<std::string>& models, int current_choice);
};

#endif // MODEL_MANAGER_H

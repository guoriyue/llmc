#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <string>
#include <vector>

class model_manager {
public:
    std::string model_path;
    std::string prompt;
    int max_tokens;
    bool show_explanation_flag;
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
    void show_help();
    std::string set_model();
    void handle_max_tokens(int max_tokens);
    void set_show_explanation(bool show);
    void print_models(const std::vector<std::string>& models, int current_choice);
};

#endif // MODEL_MANAGER_H

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <string>
#include <vector>

class model_manager {
public:
    std::string prompt;
    std::vector<std::string> models_to_choose;
    std::vector<std::string> model_urls;
    bool first_call_print_models = true;

    // Constructor
    model_manager() {
        init_models();
    }

    // Function to initialize the models vector
    void init_models() {
        models_to_choose.push_back("llama3.2-3b");
        models_to_choose.push_back("custom");
        // model_urls.push_back("https://llmc-models.s3.amazonaws.com/Llama-3.2-1B-Instruct-Q8_0.gguf");
        model_urls.push_back("https://llmc-models.s3.amazonaws.com/Llama-3.2-3B-Instruct-Q8_0.gguf");
    }

    // std::string get_valid_model_path(const std::string& cache_dir, const std::string& model_url, const std::string& model_file_name);
    std::string set_model();
    // std::size_t choose_model();
    // std::string get_cached_model();
    // std::string read_model_path();
    // void save_model_path(const std::string& path);
    // void print_models(const std::vector<std::string>& models, std::size_t current_choice);
    // std::string editable_input();
    void show_args();

    bool save_args(const std::string& args_key, const std::string& args_value);
    std::string get_args(const std::string& args_key);
};

#endif // MODEL_MANAGER_H

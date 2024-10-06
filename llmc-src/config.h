#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#define CONFIG_FILE "llmc_config.json"
#define CMD_NAME "llmc"
// #define MODEL_URL "https://llmc-models.s3.amazonaws.com/Meta-Llama-3-8B-Instruct.Q8_0.gguf"
// #define MODEL_PATH "Meta-Llama-3-8B-Instruct.Q8_0.gguf"
// #define MODEL_URL "https://llmc-models.s3.amazonaws.com/Llama-3.2-1B-Instruct-Q8_0.gguf"
// #define MODEL_PATH "Llama-3.2-1B-Instruct-Q8_0.gguf"
#define MAX_NUM_TOKENS 256
#define PROMPT_CMD_GEN "You are an expert command-line assistant tailored for developers. \n\
Your role is to understand developer requests and generate the appropriate command-line solution. Here are a couple of examples of how you handle input: \n\
Examples: \n\
Input: create a Python 3.11 Conda environment \n\
Output: <command>conda create -n \"myenv\" python=3.11</command> \n\
Input: mistakenly pushed large files to GitHub \n\
Output: <command>git filter-branch --index-filter 'git rm -r --cached --ignore-unmatch <file/dir>' HEAD</command> \n\
Instructions: \n\
Generate the appropriate command for the following task, and ensure that **only valid command(s)** are wrapped inside <command> tags. \n\
If there are multiple valid commands, return each one in a separate <command> block. \n\
**Do not wrap explanations, suggestions, or notes inside <command> tags**. Those should be outside the command blocks.\n"

// #include <string>

// class config {
// public:
//     // static std::string read_model_path();
//     // static void save_model_path(const std::string& path);
// };

#endif // CONFIG_MANAGER_H

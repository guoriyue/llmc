#include "bash_executor.h"
#include "console_manager.h"
#include <regex>
#include <string>
#include <vector>


std::vector<std::string> extract_strs(const std::string& input, const std::string& regex_str) {
    std::vector<std::string> bash_blocks;

    // Regular expression to find bash code blocks
    // No `dotall` in C++: `.` matches everything except newlines by default
    // So we manually adjust the regex to allow matching newlines
    std::regex bash_regex(regex_str);
    std::sregex_iterator iter(input.begin(), input.end(), bash_regex);
    std::sregex_iterator end;

    // Extract each bash block and store it in the vector
    while (iter != end) {
        std::smatch match = *iter;

        // Only process reasonable-sized blocks to avoid memory issues with untrusted input
        if (match[1].length() <= 10000) {
            bash_blocks.push_back(match[1].str()); // Store the extracted content
        }

        ++iter;
    }

    return bash_blocks;
}
// R"(```bash([\s\S]*?)```)"

std::string extract_str(const std::string& input, const std::string& regex_str) {
    std::string example;
    // Regular expression to find the first example in a bash block
    std::regex example_regex(regex_str);
    std::smatch match;

    // Extract the first example from the input string
    if (std::regex_search(input, match, example_regex)) {
        example = match.str();
    }

    return example;
} 
// R"(### Example.*?(?=### ))"
// R"(### Instruction.*?(?=### ))"

std::vector<std::string> extract_suggestions(const std::string& input) {
    std::vector<std::string> suggestions;

    // the block before ### Instruction or ### Example
    size_t pos_instruction = input.find("### Instruction");
    size_t pos_example = input.find("### Example");
    std::string first_output = "";
    if (pos_instruction != std::string::npos) {
        first_output = input.substr(0, pos_instruction);
    } else if (pos_example != std::string::npos) {
        first_output = input.substr(0, pos_example);
    }
    
    std::string regex_str_example = R"(### Example.*?(?=### ))";
    std::string example = extract_str(input, regex_str_example);
    std::string regex_str_instruction = R"(### Instruction.*?(?=### ))";
    std::string instruction = extract_str(input, regex_str_instruction);

    std::string potential_suggestion = first_output + example + instruction;

    std::vector<std::string> blocks = extract_strs(potential_suggestion, R"(```([\s\S]*?)```)");

    for (const std::string& block : blocks) {
        std::string clean_bash_command = trim(block);
        if (!clean_bash_command.empty() && std::find(suggestions.begin(), suggestions.end(), clean_bash_command) == suggestions.end()) {
            suggestions.push_back(clean_bash_command);
        }
    }
    return suggestions;
}
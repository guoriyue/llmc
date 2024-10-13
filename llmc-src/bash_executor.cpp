#include "bash_executor.h"
#include "console_manager.h"
#include <regex>
#include <string>
#include <vector>
#include <queue>


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

bool has_two_instructions(const std::string& input, const std::string& delimiter) {

    size_t first_pos = input.find(delimiter);

    // Check if the first "### Instruction" exists
    if (first_pos != std::string::npos) {
        // Find the second "### Instruction" after the first one
        size_t second_pos = input.find(delimiter, first_pos + delimiter.length());

        // Check if the second one exists
        if (second_pos != std::string::npos) {
            return true; // Two occurrences found
        }
    }

    return false; // Less than two occurrences found
}

size_t get_nth_delimiters(const std::string& input, const std::string& delimiter, size_t n) {
    size_t n_delimiters = 0;
    size_t pos = 0;

    while ((pos = input.find(delimiter, pos)) != std::string::npos) {
        n_delimiters++;
        pos += delimiter.length();
        if (n_delimiters == n) {
            return pos;
        }
    }

    return pos;
}


bool check_early_stop(const std::string& output_buffer, size_t unlogged_output_size) {
    // if 2 Instruction or Example blocks are found, stop early
    if (has_two_instructions(output_buffer, "###")) {
        size_t pos = get_nth_delimiters(output_buffer, "###", 2);
        std::string last_buffer = output_buffer.substr(output_buffer.length() - unlogged_output_size, pos);
        printf("%s", last_buffer.c_str());
        return true;
    }
    // if (has_two_instructions(output_buffer, "### Instruction")) {
    //     size_t pos = get_nth_delimiters(output_buffer, "### Instruction", 2);
    //     std::string last_buffer = output_buffer.substr(output_buffer.length() - unlogged_output_size, pos);
    //     printf("%s", last_buffer.c_str());
    //     return true;
    // }
    // if (has_two_instructions(output_buffer, "### Example")) {
    //     size_t pos = get_nth_delimiters(output_buffer, "### Example", 2);
    //     std::string last_buffer = output_buffer.substr(output_buffer.length() - unlogged_output_size, pos);
    //     printf("%s", last_buffer.c_str());
    //     return true;
    // }
    // if (has_two_instructions(output_buffer, "Instruction:")) {
    //     size_t pos = get_nth_delimiters(output_buffer, "Instruction:", 2);
    //     std::string last_buffer = output_buffer.substr(output_buffer.length() - unlogged_output_size, pos);
    //     printf("%s", last_buffer.c_str());
    //     return true;
    // }
    // if has ### References
    if (output_buffer.find("### References") != std::string::npos) {
        size_t pos = output_buffer.find("### References");
        std::string last_buffer = output_buffer.substr(output_buffer.length() - unlogged_output_size, pos);
        printf("%s", last_buffer.c_str());
        return true;
    }
    if (output_buffer.find("####") != std::string::npos) {
        size_t pos = output_buffer.find("####");
        std::string last_buffer = output_buffer.substr(output_buffer.length() - unlogged_output_size, pos);
        printf("%s", last_buffer.c_str());
        return true;
    }
    return false;
}
#include "output_parser.h"
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



// Function to split a string by a delimiter character and return a vector of strings
std::vector<std::string> split_str(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string temp;
    
    for (char ch : str) {
        if (ch == delimiter) {
            if (!temp.empty()) {
                result.push_back(temp);
                temp.clear();  // Reset temp for the next part
            }
        } else {
            temp += ch;  // Append the current character to the temp string
        }
    }
    
    // Add the last part after the final delimiter
    if (!temp.empty()) {
        result.push_back(temp);
    }
    
    return result;
}


std::vector<std::string> extract_suggestions(const std::string& input) {
    // may need to move this to handle by llm again, but now we do manual processing
    // many manual work to fix the generated suggestions
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

    if (potential_suggestion == "") {
        potential_suggestion = input;
    }

    std::vector<std::string> blocks = extract_strs(potential_suggestion, R"(```bash([\s\S]*?)```)");

    if (blocks.size() == 0) {
        blocks = extract_strs(potential_suggestion, R"(```shell([\s\S]*?)```)"); 
    }
    if (blocks.size() == 0) {
        blocks = extract_strs(potential_suggestion, R"(```sh([\s\S]*?)```)"); 
    }
    if (blocks.size() == 0) {
        // blocks = extract_strs(potential_suggestion, R"(```([\s\S]*?)```)"); 
        std::string regex_str = R"(```(?![a-zA-Z]+)([\s\S]*?)```)";
    }

    for (const std::string& block : blocks) {
        std::string clean_bash_command = trim(block);
        if (!clean_bash_command.empty() && std::find(suggestions.begin(), suggestions.end(), clean_bash_command) == suggestions.end()) {
            suggestions.push_back(clean_bash_command);
            // if (clean_bash_command.length() && clean_bash_command[0] == '#') {
            //     continue;
            // }
            // if (clean_bash_command.find("\n") != std::string::npos) {
            //     std::vector<std::string> lines = split_str(clean_bash_command, '\n');
            //     printf("lines.size(): %lu\n", lines.size());
            //     if (lines.size() == 0) {
            //         continue;
            //     } else if (lines.size() == 1) {
            //         // clean_bash_command = lines[0];
            //         suggestions.push_back(lines[0]);
            //     } else {
            //         // clean_bash_command = lines[1];
            //         // if (lines[0].find("#") != std::string::npos && lines[1].find("#") == std::string::npos) {
            //         //     clean_bash_command = lines[1];
            //         // } else {
            //         //     clean_bash_command = lines[0];
            //         // }
            //         for (const std::string& line : lines) {
            //             if (line.find("#") != std::string::npos && line.length() && line[0] == '#') {
            //                 // comments
            //             } else {
            //                 // clean_bash_command = line;
            //                 suggestions.push_back(line);
            //                 // break;
            //             }
            //         }
            //     }
            // }
            
        }
    }
    return suggestions;
}

size_t get_num_delimiters(const std::string& input, const std::string& delimiter) {
    size_t pos = 0;
    size_t n_delimiters = 0;

    while ((pos = input.find(delimiter, pos)) != std::string::npos) {
        n_delimiters++;
        pos += delimiter.length();
    }

    return n_delimiters;
}

size_t get_nth_delimiters(const std::string& input, const std::string& delimiter, size_t n) {
    size_t n_delimiters = 0;
    size_t pos = 0;

    while ((pos = input.find(delimiter, pos)) != std::string::npos) {
        n_delimiters++;
        if (n_delimiters == n) {
            return pos;
        }
        pos += delimiter.length();
    }

    return pos;
}


size_t check_early_stop(const std::string& output_buffer) {
    // if 2 Instruction or Example blocks are found, stop early
    if (get_num_delimiters(output_buffer, "###") >= 3) {
        return get_nth_delimiters(output_buffer, "###", 3);
    }
    if (get_num_delimiters(output_buffer, "### Instruction") >= 2) {
        return get_nth_delimiters(output_buffer, "### Instruction", 2);
    }
    if (get_num_delimiters(output_buffer, "### Example") >= 2) {
        return get_nth_delimiters(output_buffer, "### Example", 2);
    }
    // if has ### References
    if (output_buffer.find("### References") != std::string::npos) {
        return output_buffer.find("### References");
    }
    if (output_buffer.find("####") != std::string::npos) {
        return output_buffer.find("####");
    }
    if (get_num_delimiters(output_buffer, "**") >= 7) {
        return get_nth_delimiters(output_buffer, "**", 7);
    }
    // return false;
    return -1;
}
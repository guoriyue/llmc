#include "str_parser.h"
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


std::string to_lower_case(const std::string& input) {
    std::string result = input;  // Create a copy of the input string
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}
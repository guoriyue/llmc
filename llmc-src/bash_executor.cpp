#include <regex>
#include <string>
#include <vector>

std::vector<std::string> extract_bash_blocks(const std::string& input) {
    std::vector<std::string> bash_blocks;

    // Regular expression to find bash code blocks
    // No `dotall` in C++: `.` matches everything except newlines by default
    // So we manually adjust the regex to allow matching newlines
    std::regex bash_regex(R"(```bash([\s\S]*?)```)");
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

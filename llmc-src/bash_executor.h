#ifndef EXTRACT_BASH_BLOCKS_H
#define EXTRACT_BASH_BLOCKS_H

#include <string>
#include <vector>

// Function declaration for extracting bash blocks from input string
std::vector<std::string> extract_strs(const std::string& input, const std::string& regex_str);

std::string extract_str(const std::string& input, const std::string& regex_str);

std::vector<std::string> extract_suggestions(const std::string& input);
#endif // EXTRACT_BASH_BLOCKS_H

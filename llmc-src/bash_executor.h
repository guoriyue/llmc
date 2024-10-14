#ifndef EXTRACT_BASH_BLOCKS_H
#define EXTRACT_BASH_BLOCKS_H

#include <string>
#include <vector>

// Function declaration for extracting bash blocks from input string
std::vector<std::string> extract_strs(const std::string& input, const std::string& regex_str);

std::string extract_str(const std::string& input, const std::string& regex_str);

std::vector<std::string> extract_suggestions(const std::string& input);

size_t check_early_stop(const std::string& output_buffer);

size_t get_nth_delimiters(const std::string& input, const std::string& delimiter, size_t n);

size_t get_num_delimiters(const std::string& input, const std::string& delimiter);
#endif // EXTRACT_BASH_BLOCKS_H


#ifndef STR_PARSER_H
#define STR_PARSER_H

#include <string>
#include <vector>
// Function declaration for extracting bash blocks from input string
std::vector<std::string> extract_strs(const std::string& input, const std::string& regex_str);

std::string extract_str(const std::string& input, const std::string& regex_str);


size_t get_nth_delimiters(const std::string& input, const std::string& delimiter, size_t n);

size_t get_num_delimiters(const std::string& input, const std::string& delimiter);

std::vector<std::string> split_str(const std::string& str, char delimiter);


std::string to_lower_case(const std::string& input);

#endif // STR_PARSER_H
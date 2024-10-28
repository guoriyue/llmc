#ifndef OUTPUT_PARSER_H
#define OUTPUT_PARSER_H

#include <string>
#include <vector>

std::vector<std::string> extract_suggestions(const std::string& input);

size_t check_early_stop(const std::string& output_buffer);

#endif // OUTPUT_PARSER_H

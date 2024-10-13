#ifndef EXTRACT_BASH_BLOCKS_H
#define EXTRACT_BASH_BLOCKS_H

#include <string>
#include <vector>

// Function declaration for extracting bash blocks from input string
std::vector<std::string> extract_bash_blocks(const std::string& input);

std::string extract_instruction(const std::string& input);
#endif // EXTRACT_BASH_BLOCKS_H

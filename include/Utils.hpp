#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

std::string decode(std::string encoded_string);
std::string GetCurrentDate(void);
std::string to_string(ssize_t num);

// directory list
std::string generate_directory_list(const std::string& path);

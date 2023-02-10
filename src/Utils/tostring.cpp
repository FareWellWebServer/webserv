#include "../../include/Utils.hpp"

std::string to_string(ssize_t num) {
	std::stringstream ss;
	std::string str;
	ss << num;
	ss >> str;
	return (str);
}

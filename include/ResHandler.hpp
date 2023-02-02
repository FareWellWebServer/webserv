#ifndef RESHANDLER_HPP
#define RESHANDLER_HPP

//#include <iostream>
//#include "unistd.h"
#include "./WebServ.hpp"

class ResHandler {
private:
	const char* response_;
	std::size_t response_length_;

public:
	ResHandler(const char* response, std::size_t response_length);
    ~ResHandler(void);
	void SendToClient(int client_fd);
};

#endif

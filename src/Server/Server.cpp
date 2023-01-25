#include "../../include/Server.hpp"

Server::Server(const std::string& host) { Server::Init(); }

Server::~Server(void) {}

void Server::Run() {
  std::cout << "server run" << std::endl;
  // TODO: accept, bind
  // TODO: client응답을 받아서 '처리' 해서 응답을 보냄
}

void Server::Init() {
  std::cout << "server init" << std::endl;
  // TODO: init server using config
}
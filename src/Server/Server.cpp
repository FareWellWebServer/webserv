#include "../../include/Server.hpp"

Server::Server(const std::string& host_port) : host_port_(host_port) {
  Server::Init();
}

Server::~Server(void) { std::cout << "destructor" << std::endl; }

void Server::Run() {
  std::cout << host_port_ << " server run" << std::endl;
  // TODO: accept, bind
  // TODO: client응답을 받아서 '처리' 해서 응답을 보냄
}

void Server::Init() {
  std::cout << host_port_ << " server init" << std::endl;
  // TODO: init server using config
}
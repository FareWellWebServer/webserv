#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

class Server {
 public:
  Server(const std::string& host_port);
  ~Server(void);
  void Run();

 private:
  std::string host_port_;
  void Init();
};

#endif
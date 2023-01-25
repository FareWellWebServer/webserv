#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

class Server {
 public:
  Server(const std::string& host);
  ~Server(void);
  void Run();

 private:
  void Init();
};

#endif
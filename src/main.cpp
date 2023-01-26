#include "../include/WebServ.hpp"

std::vector<ServerConfigInfo> config_process(int ac, char** av);
void main_process(int ac, char** av);
int main(int ac, char** av);

// 누수 잡는 용
void a(void) { system("leaks farewell_webserv | grep leaked"); }

int main(int ac, char** av) {
  atexit(a);
  try {
    main_process(ac, av);
  } catch (const WebServException& custom_error) {
    // error handler
    ErrorHandler error_handler(custom_error);
  } catch (const std::exception& error) {
    // 미쳐 처리하지 못한 에러 잡기
    std::cerr << RED << error.what() << std::endl;
  };
}

void main_process(int ac, char** av) {
  std::vector<ServerConfigInfo> server_infos = config_process(ac, av);

  for (int i = 0; i < server_infos.size(); ++i) {
    Server server(server_infos[i].host + std::to_string(server_infos[i].port));
    server.Run();
  }
}

std::vector<ServerConfigInfo> config_process(int ac, char** av) {
  if (ac > 2) {
    throw std::runtime_error("[Config Error] few argument");
  }
  const char* file_path = (ac == 1) ? "config/default.config" : av[1];

  Config config(file_path);
  config.Parse();  // 파싱 과정 출력 X
  config.PrintConfigInfos();
  config.CheckValidation();
  return config.GetServerConfigInfos();
}

#include "../include/WebServ.hpp"

std::vector<ServerConfigInfo> config_process(int ac, char** av);
void main_process(int ac, char** av);
int main(int ac, char** av);

extern char** environ;
// 누수 잡는 용
void CheckLeaks(void) { system("leaks farewell_webserv | grep leaked"); }

int main(int ac, char** av) {
#if DG
  atexit(CheckLeaks);
#endif
  try {
    main_process(ac, av);
  } catch (const WebServException& custom_error) {
    // error handler
    ErrorHandler error_handler(custom_error);
  } catch (const std::exception& error) {
    // 미처 처리하지 못한 에러 잡기
    std::cerr << RED << error.what() << std::endl;
  };
}

void main_process(int ac, char** av) {
  const std::vector<ServerConfigInfo> server_infos = config_process(ac, av);
  Server server;

  server.Init(server_infos);
  server.Run();
}

std::vector<ServerConfigInfo> config_process(int ac, char** av) {
  if (ac > 2) {
    throw std::runtime_error("[Config Error] few argument");
  }
  const char* file_path = (ac == 1) ? "config/default.config" : av[1];

  Config config(file_path);
  config.Parse();  // 파싱 과정 출력 X
#if DG
  config.PrintConfigInfos();
#endif
  config.CheckValidation();
  return config.GetServerConfigInfos();
}

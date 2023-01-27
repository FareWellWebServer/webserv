#include "../include/WebServ.hpp"

/*
 * 에러 처리 부분은 바로 구현하기가 쉽지 않아 보여서 일단 runtime_error로
 * 처리했습니다.
 */

void config_process(int ac, char** av);
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
  // server run
  std::cout << "-----  main process running  -----" << std::endl;

  config_process(ac, av);

  // throw WebServException("example error message");
}

void config_process(int ac, char** av) {
  std::cout << "-----  config process running -----" << std::endl;
  if (ac > 2) throw std::runtime_error("[Config Error] few argument");
  const char* file_path = (ac == 1) ? "config/default.config" : av[1];

  Config config(file_path);
  config.Parse(NOT_VISIBLE);  // 파싱 과정 출력 X
  // config.Parse(VISIBLE);  // 파싱 과정 출력 O
  config.PrintConfigInfos();
  config.CheckValidation();

  std::vector<ServerConfigInfo> server_infos = config.GetServerConfigInfos();

  t_location* loc1 = server_infos[0].get_cur_location("test/path");
  t_location* loc2 = server_infos[0].get_cur_location("none");
  std::cout << "-----  Location Find Test -----" << std::endl;
  config.PrintLocation(*loc1);
  if (loc2) {
    config.PrintLocation(*loc2);
  } else
    std::cout << "Location Not Found..." << std::endl;
  std::cout << "-----  Location Find Test Finish -----" << std::endl;

  std::cout << "current date: " << GetCurrentDate() << std::endl;

  std::cout << "-----  config process finish -----" << std::endl;
}

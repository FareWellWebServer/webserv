#include "../include/WebServ.hpp"

/*
 * 에러 처리 부분은 바로 구현하기가 쉽지 않아 보여서 일단 runtime_error로 처리했습니다.
 */

void config_process(int ac, char** av);
void main_process(int ac, char** av);
int main(int ac, char** av);

int main(int ac, char** av) {
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
  std::cout << GREEN << "-----  main process running  -----" << std::endl;

  config_process(ac, av);

  // throw WebServException("example error message");
}

void config_process(int ac, char** av) {
  std::cout << BLUE << "-----  config process running -----" << std::endl;
  if (ac == 1)
    ServerConfigInfo config("config/default.config");
  else if (ac == 2)
    ServerConfigInfo config(av[1]);
  else
    throw std::runtime_error("[Config Error] few argument");
}

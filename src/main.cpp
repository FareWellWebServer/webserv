#include "../include/WebServ.hpp"

void main_process(void) {
  Server server_1("temp_domain/4242");

  server_1.Run();
}

int main(void) {
  try {
    main_process();
  } catch (const WebServException& custom_error) {
    // error handler
    ErrorHandler error_handler(custom_error);
  } catch (const std::exception& error) {
    // 미쳐 처리하지 못한 에러 잡기
    std::cerr << error.what() << std::endl;
  };
}

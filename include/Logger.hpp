#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fcntl.h>
#include <sys/event.h>
#include <unistd.h>

#include "Color.hpp"
#include "Utils.hpp"
class Logger {
  typedef int fd;

 public:
  /// @brief logger.info("message") 같은 방식으로 로깅에 사용.
  /// @param msg
  Logger(void);
  virtual ~Logger(void);
  void info(std::string msg);
  void warn(std::string msg);
  void error(std::string msg);

 private:
  fd logger_file_fd_;
  static Logger* instance_;
};

#endif

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fcntl.h>
#include <sys/event.h>
#include <unistd.h>

#include "Data.hpp"
#include "Color.hpp"
#include "Utils.hpp"

typedef int fd;
class Logger {
 public:
  /// @brief logger.info("message") 같은 방식으로 로깅에 사용.
  /// @param msg
  Logger(int kq, const std::string& log_dir_path);
  virtual ~Logger(void);
  void info(std::string msg, Data* data = NULL) const;
  void error(std::string msg, Data* data = NULL) const;
  fd GetLogFileFD(void) const;

 private:
  fd logger_file_fd_;
  int server_kq_;
};

#endif

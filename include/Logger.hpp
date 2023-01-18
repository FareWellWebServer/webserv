#ifndef LOGGER_HPP
#define LOGGER_HPP

class Logger {
 public:
  Logger(void);
  virtual ~Logger(void);
  /// @brief logger.info("message") 같은 방식으로 로깅에 사용.
  /// @param msg
  void info(std::string msg);
  void warn(std::string msg);
  void error(std::string msg);
};

#endif
#include "../../include/Logger.hpp"

Logger::Logger(int kq) : server_kq_(kq) {
  const char* file_name = "./log/log.txt";

  logger_file_fd_ = open(file_name, O_CREAT | O_NONBLOCK | O_APPEND | O_WRONLY);
  if (logger_file_fd_ == -1) {
    throw std::runtime_error("logger error: can't open log file");
  }
}

Logger::~Logger(void) { close(logger_file_fd_); }

void Logger::info(std::string msg) const {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg = current_time + " " + msg + "\n";
  struct kevent event;

  write(logger_file_fd_, log_msg.c_str(), log_msg.length());
  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ENABLE, 0, 0,
         static_cast<void*>(const_cast<char*>(log_msg.c_str())));
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

void Logger::warn(std::string msg) const {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg = YELLOW + current_time + " " + msg + "\n" + RESET;
  struct kevent event;

  write(logger_file_fd_, log_msg.c_str(), log_msg.length());
  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ENABLE, 0, 0,
         static_cast<void*>(const_cast<char*>(log_msg.c_str())));
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

void Logger::error(std::string msg) const {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg = RED + current_time + " " + msg + "\n" + RESET;
  struct kevent event;

  write(logger_file_fd_, log_msg.c_str(), log_msg.length());
  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ENABLE, 0, 0,
         static_cast<void*>(const_cast<char*>(log_msg.c_str())));
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

fd Logger::GetLogFileFD(void) const { return logger_file_fd_; }

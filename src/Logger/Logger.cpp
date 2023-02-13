#include "../../include/Logger.hpp"

Logger::Logger(int kq, const std::string& log_dir_path) : server_kq_(kq) {
  const std::string file_name = log_dir_path + "log.txt";

  logger_file_fd_ = open(file_name.c_str(), O_CREAT | O_NONBLOCK | O_APPEND | O_RDWR, S_IRWXU);
  if (logger_file_fd_ == -1) {
    throw std::runtime_error("logger error: can't open log file");
  }
}

Logger::~Logger(void) { close(logger_file_fd_); }

void Logger::info(std::string msg, Data* data) const {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg = data != NULL ?
     current_time + "\t" + msg + "\n" : 
     current_time + "\t" + msg + "\t" + data->GetMethodEntityType() + "\tclient fd:\t" + to_string(data->GetClientFd()) + "\n";
  struct kevent event;

  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ENABLE | EV_ADD, 0, 0,
         static_cast<void*>(const_cast<char*>(log_msg.c_str())));
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

void Logger::warn(std::string msg, Data* data) const {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg = data != NULL ?
     current_time + "\t" + msg + "\n" : 
     current_time + "\t" + msg + "\t" + data->GetMethodEntityType() + "\tclient fd:\t" + to_string(data->GetClientFd()) + "\n";
  const std::string warn_msg = YELLOW + log_msg + RESET;
  struct kevent event;

  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ENABLE | EV_ADD, 0, 0,
         static_cast<void*>(const_cast<char*>(warn_msg.c_str())));
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

void Logger::error(std::string msg, Data* data) const {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg = data != NULL ?
     current_time + "\t" + msg + "\n" : 
     current_time + "\t" + msg + "\t" + data->GetMethodEntityType() + "\tclient fd:\t" + to_string(data->GetClientFd()) + "\n";
  const std::string error_msg = RED + log_msg + RESET;
  struct kevent event;

  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ENABLE | EV_ADD, 0, 0,
         static_cast<void*>(const_cast<char*>(error_msg.c_str())));
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

fd Logger::GetLogFileFD(void) const { return logger_file_fd_; }

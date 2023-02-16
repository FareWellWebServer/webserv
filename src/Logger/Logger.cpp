#include "../../include/Logger.hpp"

Logger::Logger(int kq, const std::string& log_dir_path) : server_kq_(kq) {
  const std::string file_name = log_dir_path + "log.txt";

  logger_file_fd_ = open(file_name.c_str(),
                         O_CREAT | O_NONBLOCK | O_APPEND | O_RDWR, S_IRWXU);
  if (logger_file_fd_ == -1) {
    throw std::runtime_error("logger error: can't open log file");
  }
}

Logger::~Logger(void) { close(logger_file_fd_); }

void Logger::info(std::string msg, Data* data) {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg =
      data == NULL
          ? current_time + "\t" + msg + "\n"
          : current_time + "\t" + msg +
                "\tserver port: " + to_string(data->GetListenPort()) +
                "\tclient name: " + data->GetClientName() +
                "\tclient port: " + data->GetClientPort() +
                "\tmethod: " + data->GetReqMethod() +
                "\tstatus code: " + to_string(data->GetStatusCode()) + "\n";
  struct kevent event;

  log_msg_buffer_.push_back(log_msg);
  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

void Logger::error(std::string msg, Data* data) {
  const std::string current_time = GetCurrentDate();
  const std::string log_msg =
      data == NULL
          ? current_time + "\t" + msg + "\n"
          : current_time + "\t" + msg +
                "\tserver port: " + to_string(data->GetListenPort()) +
                "\tclient name: " + data->GetClientName() +
                "\tclient port: " + data->GetClientPort() +
                "\tmethod: " + data->GetReqMethod() +
                "\tstatus code:" + to_string(data->GetStatusCode()) + "\n";
  const std::string error_msg =
      RED + std::string("[ERROR]\t") + log_msg + RESET;
  struct kevent event;

  log_msg_buffer_.push_back(error_msg);
  EV_SET(&event, logger_file_fd_, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
  kevent(server_kq_, &event, 1, NULL, 0, NULL);
}

fd Logger::GetLogFileFD(void) const { return logger_file_fd_; }

void Logger::PrintAllLogMsg(void) {
  while (!log_msg_buffer_.empty()) {
    std::vector<std::string>::iterator it = log_msg_buffer_.begin();
    if (write(logger_file_fd_, it->c_str(), it->size()) < 0) {
      std::cerr << "Log File error" << std::endl;
    }
    log_msg_buffer_.erase(it);
  }
  log_msg_buffer_.clear();
}

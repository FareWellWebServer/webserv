#include "../../include/Server.hpp"

void Server::ExecuteLogEvent(void) {
  logger_.PrintAllLogMsg();
  struct kevent event;
  EV_SET(&event, logger_.GetLogFileFD(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(kq_, &event, 1, 0, 0, NULL);
}

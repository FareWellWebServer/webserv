#include "../../include/Server.hpp"

void Server::ExecuteProcessEvent(const int& idx) {
  int stat;
  waitpid(events_[idx].ident, &stat, 0);
  struct kevent event;
  EV_SET(&event, (int)events_[idx].ident, EVFILT_PROC, EV_DELETE, NOTE_EXIT, 0,
         NULL);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}

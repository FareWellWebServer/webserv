#include "../../include/Server.hpp"

void Server::ExecuteTimerEvent(const int& idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  int event_fd = events_[idx].ident;

  if (client->is_remain == true) {
    DisConnect(event_fd);
    return;
  }

  if (client->is_working == true) {
    client->timeout_ = true;
    return;
  }

  DisConnect(event_fd);
}

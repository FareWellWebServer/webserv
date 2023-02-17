#include "../../include/ClientMetaData.hpp"

ClientMetaData::ClientMetaData() : current_fd_(-1) {}

ClientMetaData::~ClientMetaData() {}

const char* ClientMetaData::WrongFd::what() const throw() {
  return "Wrong File Descriptor";
}

void ClientMetaData::ValidCheckToAccessData() {
  if (datas_.find(current_fd_) == datas_.end()) {
    throw WrongFd();
  }
}

void ClientMetaData::ValidCheckToAccessDataByFd(int fd) {
  if (datas_.find(fd) == datas_.end()) {
    throw WrongFd();
  }
}

void ClientMetaData::InitializeData(Data* data) {
  data->litsen_fd_ = -1;
  data->listen_port_ = -1;
  data->client_fd_ = -1;
  data->event_ = NULL;
  data->status_code_ = 200;
}

void ClientMetaData::SetCurrentFd(const fd& client_fd) {
  current_fd_ = client_fd;
}

void ClientMetaData::AddData(const int& listen_fd, const int& client_fd,
                             const int& host_port, char* client_name,
                             char* client_port) {
  if (datas_.find(client_fd) != datas_.end()) {
    DeleteByFd(client_fd);
  }

  Data* new_data = new Data;
  InitializeData(new_data);
  new_data->litsen_fd_ = listen_fd;
  new_data->client_fd_ = client_fd;
  new_data->listen_port_ = host_port;
  new_data->client_name_ = strdup(client_name);
  new_data->client_port_ = strdup(client_port);
  datas_.insert(std::pair<int, Data*>(client_fd, new_data));
  current_fd_ = client_fd;
}

void ClientMetaData::SetEvent(struct kevent* event) {
  datas_[current_fd_]->event_ = event;
}

void ClientMetaData::SetEventByFd(struct kevent* event, int fd) {
  datas_[fd]->event_ = event;
}

void ClientMetaData::SetConfig() {
  ValidCheckToAccessData();
  if (datas_[current_fd_]->event_ == NULL) return;
  datas_[current_fd_]->config_ =
      reinterpret_cast<ServerConfigInfo*>(datas_[current_fd_]->event_->udata);
}

void ClientMetaData::SetFileFd(int file_fd) {
  ValidCheckToAccessData();
  datas_[current_fd_]->client_fd_ = file_fd;
}

void ClientMetaData::SetPipeFd(int pipe[2]) {
  ValidCheckToAccessData();
  datas_[current_fd_]->pipe_[READ] = pipe[READ];
  datas_[current_fd_]->pipe_[WRITE] = pipe[WRITE];
}

void ClientMetaData::DeleteByFd(const int& client_fd) {
  ValidCheckToAccessDataByFd(client_fd);
  datas_[client_fd]->Clear();
  delete datas_[client_fd];
  datas_.erase(client_fd);
  struct kevent event[3];
  EV_SET(&event[0], client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&event[1], client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  EV_SET(&event[2], client_fd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
  kevent(3, event, 3, NULL, 0, NULL);
}

void ClientMetaData::SetReqMessage(t_req_msg* req_message) {
  ValidCheckToAccessData();
  datas_[current_fd_]->req_message_ = req_message;
}

void ClientMetaData::SetReqMessageByFd(t_req_msg* req_message, int fd) {
  ValidCheckToAccessDataByFd(fd);
  datas_[fd]->req_message_ = req_message;
}

Data* ClientMetaData::GetData() {
  ValidCheckToAccessData();
  return datas_[current_fd_];
}

Data* ClientMetaData::GetDataByFd(int fd) {
  ValidCheckToAccessDataByFd(fd);
  return datas_[fd];
}

t_req_msg* ClientMetaData::GetReqMsgByFd(int fd) {
  ValidCheckToAccessDataByFd(fd);
  return datas_[fd]->req_message_;
}

ServerConfigInfo* ClientMetaData::GetConfig() {
  ValidCheckToAccessData();
  return reinterpret_cast<ServerConfigInfo*>(
      datas_[current_fd_]->event_->udata);
}

int ClientMetaData::GetPort() {
  ValidCheckToAccessData();
  return datas_[current_fd_]->listen_port_;
}

int ClientMetaData::GetDataCount(void) {
  ValidCheckToAccessData();
  return datas_.size();
}

void ClientMetaData::SetStatusCode(int status_code) {
  ValidCheckToAccessData();
  datas_[current_fd_]->status_code_ = status_code;
}

int ClientMetaData::GetStatusCode() {
  ValidCheckToAccessData();
  return datas_[current_fd_]->status_code_;
}

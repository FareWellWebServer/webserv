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
  data->port_ = -1;
  // data->client_fd_ = -1;
  data->event_ = NULL;
  // data->config_ = NULL;
  // data->req_message_ = NULL;
  // data->res_message_ = NULL;
  data->status_code_ = 200;
}

void ClientMetaData::SetCurrentFd(const fd& client_fd) {
  current_fd_ = client_fd;
}

void ClientMetaData::AddData(const int& listen_fd, const int& client_fd,
                             const int& port) {
  Data new_data;

  InitializeData(&new_data);
  new_data.litsen_fd_ = listen_fd;
  new_data.client_fd_ = client_fd;
  new_data.port_ = port;
  datas_.insert(std::pair<int, Data>(client_fd, new_data));
  current_fd_ = client_fd;
}

void ClientMetaData::SetEvent(struct kevent* event) {
  datas_[current_fd_].event_ = event;
}

void ClientMetaData::SetEventByFd(struct kevent* event, int fd) {
  datas_[fd].event_ = event;
}

void ClientMetaData::SetConfig() {
  ValidCheckToAccessData();
  if (datas_[current_fd_].event_ == NULL)
    return ;
  datas_[current_fd_].config_ =
      reinterpret_cast<ServerConfigInfo*>(datas_[current_fd_].event_->udata);
}

void ClientMetaData::SetFileFd(int file_fd) {
  ValidCheckToAccessData();
  datas_[current_fd_].client_fd_ = file_fd;
}

void ClientMetaData::SetPipeFd(int pipe[2]) {
  ValidCheckToAccessData();
  datas_[current_fd_].pipe_[READ] = pipe[READ];
  datas_[current_fd_].pipe_[WRITE] = pipe[WRITE];
}


void ClientMetaData::DeleteByFd(const int& client_fd) {
  ValidCheckToAccessData();
  datas_[client_fd].Clear();
  datas_.erase(client_fd);
}

void ClientMetaData::SetReqMessage(t_req_msg* req_message)
{
  ValidCheckToAccessData();
  datas_[current_fd_].req_message_ = req_message;
}

void ClientMetaData::SetResEntity(t_entity* res_enetity)
{
  ValidCheckToAccessData();
  datas_[current_fd_].res_entity_ = res_enetity;
}

Data& ClientMetaData::GetData() {
  ValidCheckToAccessData();
  return datas_[current_fd_];
}

Data& ClientMetaData::GetDataByFd(int fd) {
	ValidCheckToAccessDataByFd(fd);
	return datas_[fd];
}

// struct HTTPMessage* ClientMetaData::GetReqHeader() {
//   ValidCheckToAccessData();
//   return datas_[current_fd_].req_message_;
// }

// struct HTTPMessage* ClientMetaData::GetResHeader() {
//   ValidCheckToAccessData();
//   return datas_[current_fd_].res_message_;
// }

ServerConfigInfo* ClientMetaData::GetConfig() {
  ValidCheckToAccessData();
  return reinterpret_cast<ServerConfigInfo*>(datas_[current_fd_].event_->udata);
}

int ClientMetaData::GetPort() {
  ValidCheckToAccessData();
  return datas_[current_fd_].port_;
}

int ClientMetaData::GetDataCount(void) {
  ValidCheckToAccessData();
  return datas_.size();
}

void ClientMetaData::SetStatusCode(int status_code) {
  ValidCheckToAccessData();
  datas_[current_fd_].status_code_ = status_code;
}

int ClientMetaData::GetStatusCode() {
  ValidCheckToAccessData();
  return datas_[current_fd_].status_code_;
}

// std::vector<std::string> ClientMetaData::GetMethods() {
//   ValidCheckToAccessData();
//   return datas_[current_fd_].req_message_.getMethod();
// }

// bool ClientMetaData::FindMethods(std::string method) {
//   ValidCheckToAccessData();
//   if (std::find(GetMethods().begin(), GetMethods().end(), method) ==
//       GetMethods().end())
//     return false;
//   else
//     return true;
// }

// char* ClientMetaData::GetURL() {
//   ValidCheckToAccessData();
//   return datas_[current_fd_].req_message_.getURL();
// }

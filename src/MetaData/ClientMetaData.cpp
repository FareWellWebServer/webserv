#include "../../include/ClientMetaData.hpp"

ClientMetaData::ClientMetaData() : current_fd_(-1)
{}

ClientMetaData::~ClientMetaData() {}

void ClientMetaData::validCheckToAccessData()
{
  if (data_.find(current_fd_) == data_.end())
    throw WrongFd();
}

void ClientMetaData::AddData(const int& listen_fd, const int& client_fd, const int& port)
{
  Data new_data;

  new_data.litsen_fd_ = listen_fd;
  // new_data.client_fd_ = client_fd;
  new_data.port_ = port;
  data_.insert({client_fd, new_data});
  current_fd_ = client_fd;
}

void ClientMetaData::setConfig(struct kevent& event)
{
  validCheckToAccessData();
  data_[current_fd_].event_ = &event;
  // data_[current_fd_].config_ = reinterpret_cast<ServerConfigInfo*>(event.udata);
}

void ClientMetaData::DeleteByFd(const int& client_fd)
{
  validCheckToAccessData();
  data_.erase(current_fd_);
}

void ClientMetaData::SetReqMessage(struct HTTPMessage* header)
{
  validCheckToAccessData();
  data_[current_fd_].req_message_ = header;
}

void ClientMetaData::SetResMessage(struct HTTPMessage* header)
{
  validCheckToAccessData();
  data_[current_fd_].res_message_ = header;
}

void ClientMetaData::SetEntity(char* entitiy)
{
  validCheckToAccessData();
  data_[current_fd_].entity_ = entitiy;
}

Data& ClientMetaData::GetData()
{
  validCheckToAccessData();
  return data_[current_fd_];
}
struct HTTPMessage* ClientMetaData::GetReqHeader()
{
  validCheckToAccessData();
  return data_[current_fd_].req_message_;
}

struct HTTPMessage* ClientMetaData::GetResHeader()
{
  validCheckToAccessData();
  return data_[current_fd_].res_message_;
}

ServerConfigInfo* ClientMetaData::GetConfig()
{
  validCheckToAccessData();
  return reinterpret_cast<ServerConfigInfo*>(data_[current_fd_].event_.udata);
}

int ClientMetaData::GetPort()
{
  validCheckToAccessData();
  return data_[current_fd_].port_;
}

int ClientMetaData::GetDataCount(void)
{
  validCheckToAccessData();
  return data_.size();
}

int ClientMetaData::GetStatusCode()
{
  validCheckToAccessData();
  data_[current_fd_].status_code_;
}

std::vector<std::string> ClientMetaData::GetMethods()
{
  validCheckToAccessData();
  return data_[current_fd_].req_message.getMethod();
}

char* ClientMetaData::GetURL()
{
  validCheckToAccessData();
  return data_[current_fd_].req_message.getURL();
}
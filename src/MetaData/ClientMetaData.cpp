#include "../../include/ClientMetaData.hpp"

ClientMetaData::ClientMetaData() : current_fd_(-1)
{}

ClientMetaData::~ClientMetaData() {}

void ClientMetaData::ValidCheckToAccessData()
{
  if (datas_.find(current_fd_) == datas_.end())
    throw WrongFd();
}

void ClientMetaData::InitializeData(Data *data)
{
  data->litsen_fd_ = -1;
  data->port_ = -1;
  // data->client_fd_ = -1;
  data->event_ = NULL;
  // data->config_ = NULL;
  data->req_message_ = NULL;
  data->res_message_ = NULL;
  data->status_code_ = 200;
  data->entity_ = NULL;
}

void ClientMetaData::SetCurrentFd(const fd& client_fd)
{
  current_fd_ = client_fd;
}

void ClientMetaData::AddData(const int& listen_fd, const int& client_fd, const int& port)
{
  Data new_data;

  InitializeData(&new_data);
  new_data.litsen_fd_ = listen_fd;
  // new_data.client_fd_ = client_fd;
  new_data.port_ = port;
  datas_.insert({client_fd, new_data});
  current_fd_ = client_fd;
}

void ClientMetaData::SetConfig(struct kevent& event)
{
  ValidCheckToAccessData();
  datas_[current_fd_].event_ = &event;
  datas_[current_fd_].config_ = reinterpret_cast<ServerConfigInfo*>(event.udata);
}

void ClientMetaData::DeleteByFd(const int& client_fd)
{
  ValidCheckToAccessData();
  datas_.erase(client_fd);
}

void ClientMetaData::SetReqMessage(struct HTTPMessage* header)
{
  ValidCheckToAccessData();
  datas_[current_fd_].req_message_ = header;
}

void ClientMetaData::SetResMessage(struct HTTPMessage* header)
{
  ValidCheckToAccessData();
  datas_[current_fd_].res_message_ = header;
}

void ClientMetaData::SetEntity(char* entitiy)
{
  ValidCheckToAccessData();
  datas_[current_fd_].entity_ = entitiy;
}

Data& ClientMetaData::GetData()
{
  ValidCheckToAccessData();
  return datas_[current_fd_];
}
struct HTTPMessage* ClientMetaData::GetReqHeader()
{
  ValidCheckToAccessData();
  return datas_[current_fd_].req_message_;
}

struct HTTPMessage* ClientMetaData::GetResHeader()
{
  ValidCheckToAccessData();
  return datas_[current_fd_].res_message_;
}

ServerConfigInfo* ClientMetaData::GetConfig()
{
  ValidCheckToAccessData();
  return reinterpret_cast<ServerConfigInfo*>(datas_[current_fd_].event_->udata);
}

int ClientMetaData::GetPort()
{
  ValidCheckToAccessData();
  return datas_[current_fd_].port_;
}

int ClientMetaData::GetDataCount(void)
{
  ValidCheckToAccessData();
  return datas_.size();
}

int ClientMetaData::GetStatusCode()
{
  ValidCheckToAccessData();
  datas_[current_fd_].status_code_;
}

std::vector<std::string> ClientMetaData::GetMethods()
{
  ValidCheckToAccessData();
  return datas_[current_fd_].req_message_.getMethod();
}

bool ClientMetaData::FindMethods(std::string method)
{
  ValidCheckToAccessData();
  if (std::find(GetMethods().begin(), GetMethods().end(), method) == GetMethods().end())
    return false;
  else
    return true;
}

char* ClientMetaData::GetURL()
{
  ValidCheckToAccessData();
  return datas_[current_fd_].req_message_.getURL();
}
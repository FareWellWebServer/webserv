#include "../include/ClientMetaData.hpp"
#include "../include/Data.hpp"

ClientMetaData::ClientMetaData() {}

ClientMetaData::~ClientMetaData() {}

// bool ClientMetaData::AddServer(const int& server_fd, const int& port)
// {
//   server_fd_.insert({server_fd, port});
// }

bool ClientMetaData::AddData(const int& listen_fd) 
{
  current_fd = listen_fd;
  Data data(listen_fd);
  data_.insert(std::pair<int, Data>(current_fd, data));
}

bool ClientMetaData::setConfig(struct kevent& event, const struct config* config) 
{
  data_[current_fd].setEvent(event);
  data_[current_fd].setConfig(config);
}

bool ClientMetaData::DeleteByFd(const int& client_fd)
{
  data_.erase(current_fd);
}

bool ClientMetaData::SetReqMessage(const int& client_fd,
                                   HTTPMessage* header)
{
  data_[client_fd].setReqMessage(header);
}

bool ClientMetaData::SetResMessage(const int& client_fd,
                                   HTTPMessage* header)
{
  data_[client_fd].setResMessage(header);
}

void SetEntity(const char* entitiy)
{
  data_[current_fd].entity_ = entity;
}

data* ClientMetaData::GetDataByFd() {}

char* ClientMetaData::GetPort() {}

int ClientMetaData::GetDataCount(void) {}

int ClientMetaData::GetStatusCode() {}

char* ClientMetaData::GetMethod() {}

char* ClientMetaData::GetURL() {}

struct HTTPMessage* ClientMetaData::GetReqHeader() {}

struct HTTPMessage* ClientMetaData::GetResHeader() {}

struct config* ClientMetaData::GetConfig() {}

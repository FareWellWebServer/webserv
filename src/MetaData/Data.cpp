#include "../../include/Data.hpp"

/**
 * @brief 세팅 후 연결 해제까지 바뀌지 않아야하는 값은 생성자에서 초기화
 * 
 */
Data::Data() : \
litsen_fd_(-1), port_(-1), client_fd_(-1), timeout_(false), event_(NULL), config_(NULL) {
  req_message_ = NULL;
  res_entity_ = NULL;
  Clear();
}

Data::~Data() {
  Clear();
}

/**
 * @brief 새로운 요청 들어올때마다 초기화할 값
 * delete : ReqMessage, res_entity_
 */
void Data::Clear() {
  status_code_ = 200;
  if (req_message_ != NULL) {
    delete req_message_;
    req_message_ = NULL;
  }
  if (res_entity_ != NULL) {
    delete res_entity_;
    res_entity_ = NULL;
  }
  if (res_message_)
  cgi = false;
  file_fd_ = -1;
  pipe_[READ] = -1;
  pipe_[WRITE] = -1;
}

int Data::GetListenFd() const {
  return litsen_fd_;
}

int Data::GetListenPort() const {
  return port_;
}

int Data::GetClientFd() const {
  return client_fd_;
}

int Data::GetStatusCode() const {
  return status_code_;
}

void Data::SetStatusCode(int status_code)
{
  status_code_ = status_code;
}

bool Data::IsTimeout() const {
  return timeout_;
}

int Data::GetFileFd() const {
  return file_fd_;
}

int Data::GetPipeWrite() const {
  return pipe_[WRITE];
}

int Data::GetPipeRead() const {
return pipe_[READ];
}

t_req_msg* Data::GetReqMessage() const {
  return req_message_;
}

std::string Data::GetReqMethod() const {
  return req_message_->method_;
}

std::string Data::GetReqURL() const {
  return req_message_->req_url_;
}

std::string Data::GetReqProtocol() const {
  return req_message_->protocol_;
}

/**
 * @brief Request Header의 키값 넣고, 그에 해당하는 value 반환
 * 
 * @param key 
 * @return std::string, 못찾으면 빈문자열
 */
std::string Data::GetReqHeaderByKey(std::string &key) const {
  return req_message_->headers_[key];
}

char* Data::GetReqBodyData() const {
  return req_message_->body_data_.data_;
}

char* Data::GetReqBodyType() const {
  return req_message_->body_data_.type_;
}

size_t Data::GetReqBodyLength() const {
  return req_message_->body_data_.length_;
}


/* Get about Response Message */
t_res_msg* Data::GetResMessage() const {
  return res_message_;
}

std::string Data::GetResVersion() const {
  return res_message_->http_version_;
}

int Data::GetResStatusCode() const {
  return res_message_->status_code_;
}

std::string Data::GetResStatusText() const {
  return res_message_->status_text_;
}

/**
 * @brief Request Header의 키값 넣고, 그에 해당하는 value 반환
 * 
 * @param key 
 * @return std::string, 못찾으면 빈문자열
 */
std::string Data::GetResHeaderByKey(std::string &key) const {
  return res_message_->headers_[key];
}

char* Data::GetResBodyData() const {
  return res_message_->body_data_->data_;
}

char* Data::GetResBodyType() const {
  return res_message_->body_data_->type_;
}

size_t Data::GetResBodyLength() const {
  return res_message_->body_data_->length_;
}

void Data::SetResMessage(t_res_msg* res_msg) {
  res_message_ = res_msg;
}

void Data::SetResEntity(t_entity* entity) {
  res_entity_ = entity;
}

t_entity* Data::GetResEntity() const {
  return res_entity_;
}

char* Data::GetResEntitData() const {
  return res_entity_->data_;
}

size_t Data::GetResEntityLength() const {
  return res_entity_->length_;
}

char* Data::GetResEntityType() const {
  return res_entity_->type_;
}


void Data::SetResEntityData(char* data) {
  res_entity_->data_ = data;
}

void Data::SetResEntityLength(size_t length) {
  res_entity_->length_ = length;
}

void Data::SetResEntityType(char *type) {
  res_entity_->type_ = type;
}

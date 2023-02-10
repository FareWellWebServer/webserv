#include "../../include/Data.hpp"

/**
 * @brief 세팅 후 연결 해제까지 바뀌지 않아야하는 값은 생성자에서 초기화
 *
 */
Data::Data(void)
    : litsen_fd_(-1),
      listen_port_(-1),
      client_fd_(-1),
      client_name_(NULL),
      client_port_(NULL),
      timeout_(false),
      cgi_(false),
      is_directory_list_(false),
      is_download(false),
      file_fd_(-1),
      log_file_fd_(-1),
      event_(NULL),
      config_(NULL),
      req_message_(NULL),
      res_message_(NULL),
      method_entity_(NULL) {
  pipe_[0] = (-1);
  pipe_[1] = (-1);
}

Data::~Data(void) {
  Clear();
  if (client_name_ != NULL) {
    delete client_name_;
    client_name_ = NULL;
  }
  if (client_port_ != NULL) {
    delete client_port_;
    client_port_ = NULL;
  }
}

/**
 * @brief HTTP 요청 시작할 때 동적할당 해주기. 이후에 외부 객체들이 이 자리에
 * 값만 넣어주기
 *
 */
void Data::Init(void) {
  // Clear();
  req_message_ = NULL;
  res_message_ = new t_res_msg;
  memset(res_message_, 0, sizeof(t_res_msg));
  method_entity_ = new t_entity;
  memset(method_entity_, 0, sizeof(t_entity));
}

/**
 * @brief HTTP 요청 시작 or HTTP 응답 직후 남은 정보 지워주기
 *
 */
void Data::Clear(void) {
  status_code_ = 200;
  cgi_ = false;
  is_directory_list_ = false;
  file_fd_ = -1;
  pipe_[READ] = -1;
  pipe_[WRITE] = -1;
  if (req_message_ != NULL) {
    if (req_message_->body_data_.data_ != NULL) {
      delete req_message_->body_data_.data_;
      req_message_->body_data_.data_ = NULL;
    }
    if (req_message_->body_data_.type_ != NULL) {
      delete req_message_->body_data_.type_;
      req_message_->body_data_.type_ = NULL;
    }
    delete req_message_;
    req_message_ = NULL;
  }
  if (res_message_ != NULL) {
    if (res_message_->body_data_.data_ != NULL) {
      delete res_message_->body_data_.data_;
      res_message_->body_data_.data_ = NULL;
    }
    if (res_message_->body_data_.type_ != NULL) {
      delete res_message_->body_data_.type_;
      res_message_->body_data_.type_ = NULL;
    }
    delete res_message_;
    res_message_ = NULL;
  }
  if (method_entity_ != NULL) {
    if (method_entity_->data_ != NULL) {
      delete method_entity_->data_;
      method_entity_->data_ = NULL;
    }
    if (method_entity_->type_ != NULL) {
      delete method_entity_->type_;
      method_entity_->type_ = NULL;
    }
    delete method_entity_;
    method_entity_ = NULL;
  }
}

///////////  * 멤버변수 Getter() *  ///////////

int Data::GetListenFd(void) const { return litsen_fd_; }

int Data::GetListenPort(void) const { return listen_port_; }

int Data::GetClientFd(void) const { return client_fd_; }

char* Data::GetClientName(void) const { return client_name_; }

char* Data::GetClientPort(void) const { return client_port_; }

int Data::GetStatusCode(void) const { return status_code_; }

bool Data::IsTimeout(void) const { return timeout_; }

bool Data::IsCGI(void) const { return cgi_; }

int Data::GetFileFd(void) const { return file_fd_; }

int Data::GetLogFileFd(void) const { return log_file_fd_; }

int Data::GetPipeWrite(void) const { return pipe_[WRITE]; }

int Data::GetPipeRead(void) const { return pipe_[READ]; }

const ServerConfigInfo* Data::GetConfig() const { return config_; }

///////////  * 멤버변수 Setter() *  ///////////

void Data::SetListenFd(int listen_fd) { litsen_fd_ = listen_fd; }

void Data::SetListenPort(int listen_port) { listen_port_ = listen_port; }

void Data::SetClientFd(int client_fd) { client_fd_ = client_fd; }

void Data::SetClientName(char* client_name) { client_name_ = client_name; }

void Data::SetClientPort(char* client_port) { client_port_ = client_port; }

void Data::SetStatusCode(int status_code) { status_code_ = status_code; }

void Data::SetTimeout(bool is_timeout) { timeout_ = is_timeout; }

void Data::SetCGi(bool is_CGI) { cgi_ = is_CGI; }

void Data::SetDirectoryList(bool is_directory_list) {
  is_directory_list_ = is_directory_list;
}

void Data::SetFileFd(int file_fd) { file_fd_ = file_fd; }

void Data::SetLogFileFd(int log_file_fd) { log_file_fd_ = log_file_fd; }

void Data::SetPipeWrite(int pipe_write_fd) { pipe_[WRITE] = pipe_write_fd; }

void Data::SetPipeRead(int pipe_read_fd) { pipe_[READ] = pipe_read_fd; }

//////////* Request Message Getter() *//////////

t_req_msg* Data::GetReqMessage(void) const { return req_message_; }

std::string Data::GetReqMethod(void) const { return req_message_->method_; }

std::string Data::GetReqURL(void) const { return req_message_->req_url_; }

std::string Data::GetReqProtocol(void) const { return req_message_->protocol_; }

/**
 * @brief Request Header의 키값 넣고, 그에 해당하는 value 반환
 *
 * @param key
 * @return std::string, 못찾으면 빈문자열
 */
std::string Data::GetReqHeaderByKey(std::string& key) const {
  return req_message_->headers_[key];
}

t_entity Data::GetReqBody(void) const { return req_message_->body_data_; }

char* Data::GetReqBodyData(void) const {
  return req_message_->body_data_.data_;
}

char* Data::GetReqBodyType(void) const {
  return req_message_->body_data_.type_;
}

size_t Data::GetReqBodyLength(void) const {
  return req_message_->body_data_.length_;
}
//////////* Request Message Setter() *//////////

void Data::SetReqMessage(t_req_msg* req_message) {
  req_message_ = req_message;
  // SetReqMethod(req_message_->method_);  //string
  // SetReqURL(req_message_->req_url_); //string
  // SetReqProtocol(req_message_->protocol_); //string
  // SetReqHeaders(req_message_->headers_); //map<string, string>
  // SetReqBody(&req_message_->body_data_);
}

void Data::SetReqMethod(std::string req_message_method) {
  if (req_message_ != NULL) req_message_->method_ = req_message_method;
}

void Data::SetReqURL(std::string req_message_URL) {
  if (req_message_ != NULL) req_message_->req_url_ = req_message_URL;
}

void Data::SetReqProtocol(std::string req_message_protocol) {
  if (req_message_ != NULL) req_message_->protocol_ = req_message_protocol;
}

void Data::SetReqHeaders(std::map<std::string, std::string>& headers) {
  if (req_message_ != NULL) req_message_->headers_ = headers;
}

void Data::SetReqBody(t_entity* body_entity) {
  req_message_->body_data_ = *body_entity;
}

void Data::SetReqBodyData(char* req_body_data) {
  if (req_message_ != NULL) req_message_->body_data_.data_ = req_body_data;
}

void Data::SetReqBodyType(char* req_body_type) {
  if (req_message_ != NULL) req_message_->body_data_.type_ = req_body_type;
}

void Data::SetReqBodyLength(size_t req_body_length) {
  if (req_message_ != NULL) req_message_->body_data_.length_ = req_body_length;
}

//////////* Response Message Getter() *//////////

t_res_msg* Data::GetResMessage(void) const { return res_message_; }

std::string Data::GetResVersion(void) const {
  return res_message_->http_version_;
}

int Data::GetResStatusCode(void) const { return res_message_->status_code_; }

std::string Data::GetResStatusText(void) const {
  return res_message_->status_text_;
}

/**
 * @brief Request Header의 키값 넣고, 그에 해당하는 value 반환
 *
 * @param key
 * @return std::string, 못찾으면 빈문자열
 */
std::string Data::GetResHeaderByKey(std::string& key) const {
  return res_message_->headers_[key];
}

t_entity Data::GetResBody(void) const { return res_message_->body_data_; }

char* Data::GetResBodyData(void) const {
  return res_message_->body_data_.data_;
}

char* Data::GetResBodyType(void) const {
  return res_message_->body_data_.type_;
}

size_t Data::GetResBodyLength(void) const {
  return res_message_->body_data_.length_;
}

//////////* Response Message Setter() *//////////

void Data::SetResMessage(t_res_msg* res_msg) {
  SetResVersion(res_msg->http_version_);
  SetResStatusCode(res_msg->status_code_);
  SetResStatusText(res_msg->status_text_);
  SetResHeaders(res_msg->headers_);
  SetResBody(&res_msg->body_data_);
}

void Data::SetResVersion(std::string version) {
  res_message_->http_version_ = version;
}

void Data::SetResStatusCode(int status_code) {
  res_message_->status_code_ = status_code;
}

void Data::SetResStatusText(std::string status_text) {
  res_message_->status_text_ = status_text;
}

void Data::SetResHeaders(std::map<std::string, std::string>& headers) {
  res_message_->headers_ = headers;
}

void Data::SetResBody(t_entity* entity) { res_message_->body_data_ = *entity; }

void Data::SetResBodyData(char* res_body_data) {
  res_message_->body_data_.data_ = res_body_data;
}

void Data::SetResBodyType(char* res_body_type) {
  res_message_->body_data_.type_ = res_body_type;
}

void Data::SetResBodyLength(size_t res_body_length) {
  res_message_->body_data_.length_ = res_body_length;
}

//////////* Method Entity Getter() *//////////

t_entity* Data::GetMethodEntity(void) const { return method_entity_; }

char* Data::GetMethodEntityData(void) const { return method_entity_->data_; }

size_t Data::GetMethodEntityLength(void) const {
  return method_entity_->length_;
}

char* Data::GetMethodEntityType(void) const { return method_entity_->type_; }

//////////* Method Entity Setter() *//////////

void Data::SetMethodEntity(t_entity* entity) {
  SetMethodEntityData(entity->data_);
  SetMethodEntityLength(entity->length_);
  SetMethodEntityType(entity->type_);
}

void Data::SetMethodEntityData(char* data) { method_entity_->data_ = data; }

void Data::SetMethodEntityLength(size_t length) {
  method_entity_->length_ = length;
}

void Data::SetMethodEntityType(char* type) { method_entity_->type_ = type; }

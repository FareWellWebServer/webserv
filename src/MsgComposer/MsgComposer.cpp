#include "../../include/MsgComposer.hpp"

MsgComposer::MsgComposer() : client_(NULL) {
  Clear();
  status_infos_.insert(std::make_pair(100, "Continue"));
  status_infos_.insert(std::make_pair(101, "Switching Protocols"));
  status_infos_.insert(std::make_pair(200, "OK"));
  status_infos_.insert(std::make_pair(201, "Created"));
  status_infos_.insert(std::make_pair(202, "Accepted"));
  status_infos_.insert(std::make_pair(203, "Non-Authoritative Information"));
  status_infos_.insert(std::make_pair(204, "No Content"));
  status_infos_.insert(std::make_pair(205, "Content"));
  status_infos_.insert(std::make_pair(206, "Partial Content"));
  status_infos_.insert(std::make_pair(300, "Multiple Choices"));
  status_infos_.insert(std::make_pair(301, "Moved Permanently"));
  status_infos_.insert(std::make_pair(302, "Found"));
  status_infos_.insert(std::make_pair(303, "See Other"));
  status_infos_.insert(std::make_pair(304, "Not Modified"));
  status_infos_.insert(std::make_pair(305, "Use Proxy"));
  status_infos_.insert(std::make_pair(307, "Temporary Redirect"));
  status_infos_.insert(std::make_pair(400, "Bad Request"));
  status_infos_.insert(std::make_pair(401, "Unauthorized"));
  status_infos_.insert(std::make_pair(402, "Payment Required"));
  status_infos_.insert(std::make_pair(403, "Payment Required"));
  status_infos_.insert(std::make_pair(404, "Not Found"));
  status_infos_.insert(std::make_pair(405, "Method Not Allowed"));
  status_infos_.insert(std::make_pair(406, "Not Acceptable"));
  status_infos_.insert(std::make_pair(407, "Proxy Authentication Required"));
  status_infos_.insert(std::make_pair(408, "Request Timeout"));
  status_infos_.insert(std::make_pair(409, "Conflict"));
  status_infos_.insert(std::make_pair(410, "Gone"));
  status_infos_.insert(std::make_pair(411, "Length Required"));
  status_infos_.insert(std::make_pair(412, "Precondition Failed"));
  status_infos_.insert(std::make_pair(413, "Request Entity Too Large"));
  status_infos_.insert(std::make_pair(414, "Request-URI Too Long"));
  status_infos_.insert(std::make_pair(415, "Unsupported Media Type"));
  status_infos_.insert(std::make_pair(416, "Requested Range Not Satisfiable"));
  status_infos_.insert(std::make_pair(417, "Expectation Failed"));
  status_infos_.insert(std::make_pair(500, "Internal Server Error"));
  status_infos_.insert(std::make_pair(501, "Not Implemented"));
  status_infos_.insert(std::make_pair(502, "Bad Gateway"));
  status_infos_.insert(std::make_pair(503, "Service Unavailable"));
  status_infos_.insert(std::make_pair(504, "Gateway Timeout"));
  status_infos_.insert(std::make_pair(505, "HTTP Version Not Supported"));
}

// MsgComposer::MsgComposer(Data* client) : client_(client) {
//   Clear();
//   status_infos_.insert(std::make_pair(100, "Continue"));
//   status_infos_.insert(std::make_pair(101, "Switching Protocols"));
//   status_infos_.insert(std::make_pair(200, "OK"));
//   status_infos_.insert(std::make_pair(201, "Created"));
//   status_infos_.insert(std::make_pair(202, "Accepted"));
//   status_infos_.insert(std::make_pair(203, "Non-Authoritative Information"));
//   status_infos_.insert(std::make_pair(204, "No Content"));
//   status_infos_.insert(std::make_pair(205, "Content"));
//   status_infos_.insert(std::make_pair(206, "Partial Content"));
//   status_infos_.insert(std::make_pair(300, "Multiple Choices"));
//   status_infos_.insert(std::make_pair(301, "Moved Permanently"));
//   status_infos_.insert(std::make_pair(302, "Found"));
//   status_infos_.insert(std::make_pair(303, "See Other"));
//   status_infos_.insert(std::make_pair(304, "Not Modified"));
//   status_infos_.insert(std::make_pair(305, "Use Proxy"));
//   status_infos_.insert(std::make_pair(307, "Temporary Redirect"));
//   status_infos_.insert(std::make_pair(400, "Bad Request"));
//   status_infos_.insert(std::make_pair(401, "Unauthorized"));
//   status_infos_.insert(std::make_pair(402, "Payment Required"));
//   status_infos_.insert(std::make_pair(403, "Payment Required"));
//   status_infos_.insert(std::make_pair(404, "Not Found"));
//   status_infos_.insert(std::make_pair(405, "Method Not Allowed"));
//   status_infos_.insert(std::make_pair(406, "Not Acceptable"));
//   status_infos_.insert(std::make_pair(407, "Proxy Authentication Required"));
//   status_infos_.insert(std::make_pair(408, "Request Timeout"));
//   status_infos_.insert(std::make_pair(409, "Conflict"));
//   status_infos_.insert(std::make_pair(410, "Gone"));
//   status_infos_.insert(std::make_pair(411, "Length Required"));
//   status_infos_.insert(std::make_pair(412, "Precondition Failed"));
//   status_infos_.insert(std::make_pair(413, "Request Entity Too Large"));
//   status_infos_.insert(std::make_pair(414, "Request-URI Too Long"));
//   status_infos_.insert(std::make_pair(415, "Unsupported Media Type"));
//   status_infos_.insert(std::make_pair(416, "Requested Range Not Satisfiable"));
//   status_infos_.insert(std::make_pair(417, "Expectation Failed"));
//   status_infos_.insert(std::make_pair(500, "Internal Server Error"));
//   status_infos_.insert(std::make_pair(501, "Not Implemented"));
//   status_infos_.insert(std::make_pair(502, "Bad Gateway"));
//   status_infos_.insert(std::make_pair(503, "Service Unavailable"));
//   status_infos_.insert(std::make_pair(504, "Gateway Timeout"));
//   status_infos_.insert(std::make_pair(505, "HTTP Version Not Supported"));
// }

MsgComposer::~MsgComposer(void) {}

void MsgComposer::SetStatusText(void) {
  if (status_infos_.count(res_msg_.status_code_) == 1) {
    res_msg_.status_text_ = status_infos_[res_msg_.status_code_];
    // 없는 상태 코드의 경우가 없을 것 같긴 합니다
  } else {
    res_msg_.status_text_ = "Unknown Status Code Error";
  }
}

void MsgComposer::SetData(Data* client) {
  client_ = client;
}

void MsgComposer::SetHeaders(void) {
  // content-length
  std::stringstream ss;
  ss << client_->GetReqBodyLength();
  res_msg_.headers_["Content-length"] = ss.str();
  // content-type
  // .headers_["Content-type"] = .body_data_->type;
  res_msg_.headers_["Content-type"] = "text/html";
  // Connection -> keep-alive
  res_msg_.headers_["Connection"] = "keep-alive";
  // 헤더 필요하면 여기서 더 추가하기(set-cookie or ...)
}

void MsgComposer::InitResMsg() {
  res_msg_.http_version_ = "HTTP/1.1";
  res_msg_.status_code_ = client_->status_code_;
  SetStatusText();
  res_msg_.body_data_ = client_->GetReqBody();
  SetHeaders();
}

const char* MsgComposer::GetResponse(void) {
  std::stringstream ss;
  std::string status_code;

  // first line 채우기
  ss << res_msg_.status_code_;
  ss >> status_code;
  std::string str = res_msg_.http_version_ + " " + status_code + " " +
                    res_msg_.status_text_ + "\r\n";

  std::size_t first_len = str.size();
  std::cout << "first_line len: " << first_len << std::endl;

  // headers 채우기
  std::map<std::string, std::string>::const_iterator it =
      res_msg_.headers_.begin();
  for (; it != res_msg_.headers_.end(); ++it)
    str.append(it->first + ": " + it->second + "\r\n");
  str.append("\r\n");

  std::size_t headers_len = str.size() - first_len;
  std::cout << "headers len: " << headers_len << std::endl;

  // response 생성
  std::cout << "entity len: " << res_msg_.body_data_.length_ << std::endl;
  response_length_ = str.length() + res_msg_.body_data_.length_;
  std::cout << "response len: " << response_length_ << std::endl << std::endl;

  // response에 붙여넣기
  char* res = (char*)malloc(sizeof(char) * response_length_);
  size_t str_len = str.length();
  for (std::size_t i = 0; i < str_len; ++i) {
    res[i] = str[i];
  }
  for (std::size_t i = 0; i < res_msg_.body_data_.length_; ++i) {
    res[str_len + i] = res_msg_.body_data_.data_[i];
  }
  return res;
}

std::size_t MsgComposer::getLength(void) const { return (response_length_); }

void MsgComposer::Clear() {
  res_msg_.http_version_.clear();
  res_msg_.status_code_ = 200;
  res_msg_.status_text_.clear();
  res_msg_.headers_.clear();
  // res_msg_.body_data_ // leak 날 수 있음
}

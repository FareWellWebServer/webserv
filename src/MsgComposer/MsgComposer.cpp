#include "../../include/MsgComposer.hpp"

MsgComposer::MsgComposer() : client_(NULL) {
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
  status_infos_.insert(std::make_pair(403, "Forbidden"));
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

MsgComposer::~MsgComposer(void) {}

void MsgComposer::SetStatusText(void) {
  if (status_infos_.count(client_->res_message_->status_code_) == 1) {
    client_->res_message_->status_text_ =
        status_infos_[client_->res_message_->status_code_];
  } else {
    client_->res_message_->status_text_ = "Unknown Status Code Error";
  }
}

void MsgComposer::SetData(Data* client) { client_ = client; }

void MsgComposer::InitResMsg(Data* client) {
  SetData(client);
  client_->res_message_->http_version_ = "HTTP/1.1";
  client_->res_message_->status_code_ = client_->status_code_;
  SetStatusText();
}

const char* MsgComposer::GetResponse(Data* client) {
  SetData(client);
  std::stringstream ss;
  std::string status_code;

  // first line 채우기
  ss << client_->res_message_->status_code_;
  ss >> status_code;
  std::string str = client_->res_message_->http_version_ + " " + status_code +
                    " " + client_->res_message_->status_text_ + "\r\n";

  // headers 채우기
  if (!client_->res_message_->headers_.empty()) {
    std::map<std::string, std::string>::const_iterator it =
        client_->res_message_->headers_.begin();
    for (; it != client_->res_message_->headers_.end(); ++it) {
      str.append(it->first + ": " + it->second + "\r\n");
    }
    str.append("\r\n");
  }

  // response 생성

  response_length_ = str.size() + client_->res_message_->body_data_.length_;
  char* res = new char[response_length_];

  // response에 붙여넣기
  size_t str_len = str.size();
  for (size_t i = 0; i < str_len; ++i) {
    res[i] = str[i];
  }
  for (size_t i = 0; i < client_->res_message_->body_data_.length_; ++i) {
    res[str_len + i] = client_->res_message_->body_data_.data_[i];
  }
  return res;
}

std::size_t MsgComposer::GetLength(void) const { return (response_length_); }

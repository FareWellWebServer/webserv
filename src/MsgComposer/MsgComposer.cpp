#include <iostream>
#include "../../include/MsgComposer.hpp"

MsgComposer::MsgComposer(Data* client) : client_(client) {
    Clear();
}

MsgComposer::~MsgComposer(void) {}

void MsgComposer::SetStatusText(void) {
	switch (res_msg_.status_code_) {
    case 100:
        res_msg_.status_text_ = "Continue";
        break;
    case 101:
        res_msg_.status_text_ = "Switching Protocols";
        break;
    case 200:
        res_msg_.status_text_ = "Ok";
        break;
    case 201:
        res_msg_.status_text_ = "Created";
        break;
    case 202:
        res_msg_.status_text_ = "Accepted";
        break;
    case 203:
        res_msg_.status_text_ = "Non-Authoritative Information";
        break;
    case 204:
        res_msg_.status_text_ = "No Content";
        break;
    case 205:
        res_msg_.status_text_ = " Content";
        break;
    case 206:
        res_msg_.status_text_ = "Partial Content";
        break;
    case 300:
        res_msg_.status_text_ = "Multiple Choices";
        break;
    case 301:
        res_msg_.status_text_ = "Moved Permanently";
        break;
    case 302:
        res_msg_.status_text_ = "Found";
        break;
    case 303:
        res_msg_.status_text_ = "See Other";
        break;
    case 304:
        res_msg_.status_text_ = "Not Modified";
        break;
    case 305:
        res_msg_.status_text_ = "Use Proxy";
        break;
    case 307:
        res_msg_.status_text_ = "Temporary Redirect";
        break;
    case 400:
        res_msg_.status_text_ = "Bad Request";
        break;
    case 401:
        res_msg_.status_text_ = "Unauthorized";
        break;
    case 402:
        res_msg_.status_text_ = "Payment Required";
        break;
    case 403:
        res_msg_.status_text_ = "Forbidden";
        break;
	case 404:
		res_msg_.status_text_ = "Not Found";
		break;
	case 405:
		res_msg_.status_text_ = "Method Not Allowed";
		break;
	case 406:
		res_msg_.status_text_ = "Not Acceptable";
		break;
	case 407:
		res_msg_.status_text_ = "Proxy Authentication Required";
		break;
	case 408:
		res_msg_.status_text_ = "Request Timeout";
		break;
	case 409:
		res_msg_.status_text_ = "Conflict";
		break;
	case 410:
		res_msg_.status_text_ = "Gone";
		break;
	case 411:
		res_msg_.status_text_ = "Length Required";
		break;
	case 412:
		res_msg_.status_text_ = "Precondition Failed";
		break;
	case 413:
		res_msg_.status_text_ = "Request Entity Too Large";
		break;
	case 414:
		res_msg_.status_text_ = "Request-URI Too Long";
		break;
	case 415:
		res_msg_.status_text_ = "Unsupported Media Type";
		break;
	case 416:
		res_msg_.status_text_ = "Requested Range Not Satisfiable";
		break;
	case 417:
		res_msg_.status_text_ = "Expectation Failed";
		break;
	case 500:
		res_msg_.status_text_ = "Internal Server Error";
		break;
	case 501:
		res_msg_.status_text_ = "Not Implemented";
		break;
	case 502:
		res_msg_.status_text_ = "Bad Gateway";
		break;
	case 503:
		res_msg_.status_text_ = "Service Unavailable";
		break;
	case 504:
		res_msg_.status_text_ = "Gateway Timeout";
		break;
	case 505:
		res_msg_.status_text_ = "HTTP Version not supported";
		break;
    default:
		res_msg_.status_text_ = "Unknown Status Code Error";
        break;
    }
}

void MsgComposer::SetHeaders(void) {
    // content-length
    std::stringstream ss;
    ss << res_msg_.body_data_->entity_length_;
    res_msg_.headers_["Content-length"] = ss.str();
    // content-type
    // .headers_["Content-type"] = .body_data_->type;
    res_msg_.headers_["Content-type"] = "text/plain";
    // Connection -> keep-alive
    res_msg_.headers_["Connection"] = "keep-alive";
    // 헤더 필요하면 여기서 더 추가하기(set-cookie or ...)
}

void MsgComposer::InitResMsg() {
	res_msg_.http_version_ = "HTTP/1.1";
	res_msg_.status_code_ = client_->status_code_;
    SetStatusText();
	res_msg_.body_data_ = client_->entity_;
    SetHeaders();
}

const char* MsgComposer::GetResponse(void) {
    std::stringstream ss;
    std::string status_code;

    // first line 채우기
    ss << res_msg_.status_code_;
    ss >> status_code;
    std::string str = res_msg_.http_version_ + " " + status_code + " " \
                    + res_msg_.status_text_ + "\r\n";

    std::size_t first_len = str.size();
    std::cout << "first_line len: " << first_len << std::endl;
        
    // headers 채우기
    std::map<std::string, std::string>::const_iterator it = res_msg_.headers_.begin();
    for (; it != res_msg_.headers_.end(); ++it)
		str.append(it->first + ": " + it->second + "\r\n");
	str.append("\r\n");

    std::size_t headers_len = str.size() - first_len;
    std::cout << "headers len: " << headers_len << std::endl;

    // response 생성
    std::cout << "entity len: " << res_msg_.body_data_->entity_length_ << std::endl;
    response_length_ = str.length() + res_msg_.body_data_->entity_length_;
    std::cout << "response len: " << response_length_ << std::endl << std::endl;

    // response에 붙여넣기
    char* res = (char *)malloc(sizeof(char) * response_length_);
    size_t str_len = str.length();
    for (std::size_t i = 0; i < str_len; ++i) {
        res[i] = str[i];
    }
    for (std::size_t i = 0; i < res_msg_.body_data_->entity_length_; ++i) {
        res[str_len + i] = res_msg_.body_data_->entity_[i];
    }
    return res;
}

std::size_t MsgComposer::getLength(void) const {
    return (response_length_);
}

void MsgComposer::Clear() {
    res_msg_.http_version_.clear();
    res_msg_.status_code_ = 200;
    res_msg_.status_text_.clear();
    res_msg_.headers_.clear();
    res_msg_.body_data_ = NULL;
}

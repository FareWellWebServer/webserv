#include "../../include/Session.hpp"

void Session::SetCookie(Data* client) {
  srand(time(NULL));
  int cookie_id(rand());
  while (cookie_.find(cookie_id) != cookie_.end()) cookie_id = rand();
  cookie_[cookie_id] = GetCurrentTime();
  client->res_message_->headers_["Set-Cookie"] =
      std::string("id=") + to_string(cookie_id);
  // client->req_message_->headers_["Cookie"] = std::string("id=") +
  // to_string(cookie_id);
}

time_t Session::GetCurrentTime() {
  time_t current_time(time(NULL));
  return current_time;
}

/* 유효하면 쿠키 전용 html 페이지
    유효하지 않으면 밖에서 SetCookie 해주기
*/
bool Session::IsValidCookie(Data* client) {  // Cookie : id=12392193721
  typedef std::map<std::string, std::string> key_val_t;
  key_val_t::iterator cookid_id = client->req_message_->headers_.find("Cookie");
  if (cookid_id == client->req_message_->headers_.end()) {
    return false;
  }

  // std::map<int, time_t> cookie_;
  std::string client_cookie(cookid_id->second.substr(3));
  cookie_t::iterator time_it = cookie_.find(atoi(client_cookie.c_str()));
  if (time_it != cookie_.end()) {
    if (difftime(GetCurrentTime(), time_it->second) < TIMEOUT_SEC) {
      cookie_[time_it->first] = GetCurrentTime();
      return true;
    } else {
      cookie_.erase(time_it);
      return false;
    }
  }
  return false;
}

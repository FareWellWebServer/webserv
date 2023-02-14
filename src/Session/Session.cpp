#include "../include/Session.hpp"

void Session::SetCookie(Data* client) {
    int cookie_id(rand());
    while (cookie.find(cookie_id) == cookie.end())
        cookie_id = rand();
    cookie[cookie_id] = GetCurrentTime();
    client->res_message_->headers_["Set-Cookie"] = std::string("id=") + to_string(cookie_id);
}

time Session::GetCurrentTime() {
    time_t timer(time(NULL));
    struct tm* t(localtime(&timer));
    time current_time(0);
    current_time += t->tm_sec;
    current_time += t->tm_min * 100;
    current_time += t->tm_hour * 10000;
    current_time += t->tm_mday * 1000000;
    current_time += t->tm_mon * 100000000;
    current_time += t->tm_year * 1000000000000;
    return current_time;
}

/* 유효하면 쿠키 전용 html 페이지
    유효하지 않으면 밖에서 SetCookie 해주기
*/
bool Session::IsValidCookie(Data* client, int cookie_id) {
    std::string client_cookie(client->req_message_->headers_["Cookie"]);
    if (client_cookie.empty())
        return false;
    std::string client_cookie_id(client_cookie.substr(client_cookie.find("=") + 1));
    int client_cookie_id_int(atoi(client_cookie_id.c_str()));
    time client_time = cookie[client_cookie_id_int];
    time timeout(0);
    timeout += TIMEOUT_SEC;
    timeout += TIMEOUT_MIN * 100;
    timeout += TIMEOUT_HOUR * 10000;
    timeout += TIMEOUT_DAY * 1000000;
    timeout += TIMEOUT_MON * 100000000;
    timeout += TIMEOUT_YEAR * 1000000000000;
    if (GetCurrentTime() - client_time < timeout)
        return true;
    else {
        cookie.erase(client_cookie_id_int);
        return false;
    }
}

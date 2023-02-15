#ifndef SESSION_HPP
#define SESSION_HPP

#include <map>
#include <ctime>
#include <cstdlib>
#include "Data.hpp"
#include "Utils.hpp"
#define TIMEOUT_SEC 5


class Session {
    public:
        typedef std::map<int, time_t> cookie_t;
        cookie_t cookie_;
        void SetCookie(Data* client); // client 에 대해서 현재시간으로 쿠키 설정해주기
        time_t GetCurrentTime();
        bool IsValidCookie(Data* client);
    private:
};

#endif

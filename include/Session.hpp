#ifndef SESSION_HPP
#define SESSION_HPP

#include <map>
#include <ctime>
#include "Data.hpp"
#include "Utils.hpp"
#define TIMEOUT_SEC 10
#define TIMEOUT_MIN 0
#define TIMEOUT_HOUR 0
#define TIMEOUT_DAY 0
#define TIMEOUT_MON 0
#define TIMEOUT_YEAR 0

typedef unsigned long long time;

class Session {
    public:
        std::map<int, time> cookie;
        void SetCookie(Data* client); // client 에 대해서 현재시간으로 쿠키 설정해주기
        time GetCurrentTime();
        bool IsValidCookie(Data* client, int cookie_id);

    private:
};

#endif

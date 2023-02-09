#ifndef CGIMANAGER_HPP
#define CGIMANAGER_HPP

#include "Data.hpp"
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


class CGIManager {
    public:
        void SetData(Data* client);
        void SetCGIEnv(Data* client); // fork 안에서 해주기
        void SendToCGI(Data* client, int kq);
        void GetFromCGI(Data* client, size_t len, int kq);
    private:
        Data* client_;
        char* ParseCGIType(char* buf);
};

#endif

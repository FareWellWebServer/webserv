#ifndef CGIMANAGER_HPP
#define CGIMANAGER_HPP

#include "Data.hpp"
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cstdlib>


class CGIManager {
    public:
        void SetData(Data* client);
        void SetCGIEnv(Data* client); // fork 안에서 해주기
        void SendToCGI(Data* client, int kq);
        char* GetFromCGI(Data* client, int64_t len, int kq);
    private:
        Data* client_;
        char* ParseCGIType(char* buf);
        void ParseFirstLine(char* buf);

        bool CheckValid(char* buf);
        void SetFirstLine();
        void ParseCGIData(char* buf);
};

#endif

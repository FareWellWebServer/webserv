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
#define READ 0
#define WRITE 1


class CGIManager {
    public:
        void SetData(Data* client);
        void SetCGIEnv(Data* client); // fork 안에서 해주기
        void SendToCGI(Data* client, int kq);
        bool WriteToCGIPipe(Data* client, int kq);
        bool GetFromCGI(Data* client, int64_t len, int kq);
    private:
        Data* client_;
        char* ParseCGIType(char* buf);
        void ParseFirstLine(char* buf);

        bool CheckValid(char* buf);
        void SetFirstLine();
        void ParseCGIData(char* buf);

        size_t SetHeaders(std::string& body);
        void SetBodyLength(std::string& body, size_t idx);
        void SetBody(char* buf, size_t idx);
};

std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
void RemoveTabSpace(std::string& str);
std::string to_string(ssize_t num);

#endif

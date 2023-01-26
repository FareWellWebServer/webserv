#include "../../include/Webserv.hpp"

ReqHandler::ReqHandler(void)
{
    std::memset(this, 0, sizeof(ReqHandler));

}

ReqHandler::~ReqHandler(void)
{
}

void    Parse_Req_msg(int c_socket)
{
    std::ifstream c_msg;
    std::string content;
    std::string line;

    while (!c_msg.eof()) {
        getline(c_msg, line);
        content.append(line);
    }
    //close를 함수 밖에서 해주는가 / 안에서 해주는가
    c_msg.close();
    
}
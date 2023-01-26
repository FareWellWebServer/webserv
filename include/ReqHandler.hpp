#ifndef ReqHandler_HPP
# define ReqHandler_HPP

#include "WebServ.hpp"

typedef struct t_req_msg{
    std::string method;
    std::string req_url;
    std::string protocol;
    std::map<std::string, std::string> headers;
    bool        is_body;
    std::string req_body;
} s_req_msg;

class ReqHandler
{
private :

public :
    s_req_msg req_msg;


    ReqHandler(void);
    ~ReqHandler(void);
};

void Parse_Req_msg(int c_socket);

#endif
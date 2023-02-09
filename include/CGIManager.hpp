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
        void SetCGIEnv(); // fork 안에서 해주기
        void SendToCGI(int kq);
        void GetFromCGI(size_t len);
    private:
        Data* client_;
};

#endif

void CGIManager::SetData(Data* client) {
    client_ = client;
}

/* fork 후 exeve 직전 */
void CGIManager::SetCGIEnv() {
    /* Content-Type */
    std::string key = "Content_Type";
    std::string value = client_->GetReqHeaderByKey(key);
    if (value.empty() == true)
        setenv("CONTENT_TYPE", value.c_str(), 1);

    /* Content-Length */
    key = "Content-Length";
    value = client_->GetReqHeaderByKey(key);
    if (value.empty() == true)
        setenv("CONTENT_LENGTH", value.c_str(), 1);

    /* Cookie */
    key = "Cookie";
    value = client_->GetReqHeaderByKey(key);
    if (value.empty() == true)
        setenv("HTTP_COOKIE", value.c_str(), 1);

    /* User-Agent */
    key = "User-Agent";
    value = client_->GetReqHeaderByKey(key);
    if (value.empty() == true)
        setenv("HTTP_USER_AGENT", value.c_str(), 1);

    /* Cgi script path - PATH_INFO */
    setenv("PATH_INFO", "../cgi/", 1);

    /* QUERY_STRING. URL 뒤에 붙은 녀석 */
    // .py? 인데, ? 부터 넣어줘야되는가? ? 뒤부터 넣어줘야 하는가?
    std::string url_query = client_->GetReqURL();
    std::string query_string = url_query.substr(url_query.find(".py?") + 1);
    if (query_string.empty() == true)
        setenv("QUERY_STRING", value.c_str(), 1);

    /* REMOTE_ADDR */
    // The IP address of the remote host making the request.
    setenv("REMOTE_ADDR", client_->GetClientName(), 1);
    setenv("REMOTE_HOST", client_->GetClientName(), 1);

    /* REQUEST_METHOD */
    setenv("REQUEST_METHOD", client_->GetReqMethod().c_str(), 1);

    /* Cgi script full path -  */
    setenv("SCRIPT_FILENAME", "../cgi/cgi.py", 1);

    /* SCRIPT_NAME */
    setenv("SCRIPT_NAME", "cgi.py", 1);

    /* SERVER_NAME */
    setenv("SERVER_NAME", client_->GetConfig()->host_.c_str(), 1);

    /* SERVER_SOFTWARE */
    setenv("SERVER_NAME", client_->GetConfig()->host_.c_str(), 1);
}


// 요청 들어온 것들 CGI 로 넘겨주는 것
// req_massage의 첫줄은 argv로, 헤더는 env로 본문은 stdin으로 보내줌
void CGIManager::SendToCGI(int kq)
{
    int p[2];
    pid_t pid;
    pipe(p);

    client_->SetPipeRead(p[0]);
    client_->SetPipeWrite(p[1]);
    pid = fork();
    if (pid > 0) {
        write(p[1], client_->GetReqBodyData(), client_->GetReqBodyLength());
        struct kevent event;
        EV_SET(&event, p[0], EVFILT_READ, EV_ADD, 0, 0, client_);
        kevent(kq, &event, 1, NULL, 0, NULL);
        close(p[1]);
    }
    else if (pid == 0) {
        SetCGIEnv();
        dup2(p[1], 1);
        dup2(p[0], 0);
        extern char** environ;
        extern char** __argv;
        if (execve("../cgi/cgi.py", __argv, environ) < 0)
            std::cout << "[CGI] execve 에러" << std::endl;
    }
    else {
        std::cout << "[CGI] fork() 에러" << std::endl;
    }
}

// kqueue에서 cgi read 발생하면!
// 이거 하고 응답메시지 보내주면될듯 
/**
 * @brief 이거 하기 전에 반드시 CGIManager.SetData해야함
 * @param len : event->data
 */
void CGIManager::GetFromCGI(size_t len)
{
    
    char* buf = new char(len);
    read(client_->GetPipeRead(), buf, len)

    while (다 읽을 떄까지)
    {

        get_next_line(pipe[read], int *len) // 길이도 받아오면 좋을듯
        resMassage.first_line = 첫줄
        '\n' or eof 나올떄까지 응답 헤더 map<string, string>에 넣음
        if (eof 아니면)
            entity 끝까지 읽으면서 하나로 합침 + 길이 더해놓기
        resMassage.entity = 위 결과 넣기
        응답 헤더 content_length map<string, string>에 넣음
    }
    close(pipe[read])
    map<client, pipe[read]> 제거
}
#include "../../include/CGIManager.hpp"

/* CGIManager 사용시 반드시 세팅하기 */
void CGIManager::SetData(Data* client) {
    client_ = client;
}

/* fork 후 exeve 직전 */
void CGIManager::SetCGIEnv(Data* client) {
    SetData(client);

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
void CGIManager::SendToCGI(Data* client, int kq)
{
    SetData(client);
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
        SetCGIEnv(client);
        dup2(p[1], 1);
        dup2(p[0], 0);
        extern char** environ;
        // extern char** __argv;
        if (execve(client_->GetReqURL().c_str(), NULL, environ) < 0) {
            #if CGI
                std::cout << "[CGI] execve 에러" << std::endl;
            #endif
        }
    }
    else {
        #if CGI
            std::cout << "[CGI] fork() 에러" << std::endl;
        #endif
    }
}

// kqueue에서 cgi read 발생하면!
// 이거 하고 응답메시지 보내주면될듯 
/**
 * @brief 이거 하기 전에 반드시 CGIManager.SetData해야함. 이후에 MSGComposer 호출하면 됨
 * @param len : kevent->data
 */
void CGIManager::GetFromCGI(Data* client, size_t len, int kq)
{
    SetData(client);

    char* buf = new char[len];
    read(client_->GetPipeRead(), buf, len);
    struct kevent event;
    EV_SET(&event, client_->GetPipeRead(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    close(client_->GetPipeRead());
    client_->SetMethodEntityLength(len);
    client_->SetMethodEntityData(buf);
    char* type = ParseCGIType(buf);
    client_->SetMethodEntityType(type);
}

/**
 * @brief 첫줄 보고 타입 뭔지 판단할 수 있게 해주기. 일단 html 만 있음
 * 
 * @param buf CGI에서 받아온 body
 * @return char* MethodEntity->type에 들어갈 type이 동적할당해서 나옴
 */
char* CGIManager::ParseCGIType(char* buf) {
    if (strncmp("<!DOCTYPE html>\n", buf, 16) == 0) {
        return strdup("text/html");
    }
    #if CGI
        std::cout << "[CGIManager] ParseCGIType() No type" << std::endl;
    #endif
    return NULL;
}

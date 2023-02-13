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
        close(p[1]);
        struct kevent event;
        EV_SET(&event, p[0], EVFILT_READ, EV_ADD, 0, 0, client_);
        kevent(kq, &event, 1, NULL, 0, NULL);
    }
    else if (pid == 0) {
        SetCGIEnv(client);
        dup2(p[1], 1);
        dup2(p[0], 0);
        extern char** environ;
        // extern char** __argv;
        // if (execve(client_->GetReqURL().c_str(), NULL, environ) < 0) {
        char **argv = new char*[3];
        argv[0] = strdup("python3");
        argv[1] = strdup("/Users/dongchoi/webserv/asset/cgi-bin/test.py");
        argv[2] = NULL;
        if (execve("/usr/bin/python3", argv, environ) < 0) {
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
char* CGIManager::GetFromCGI(Data* client, int64_t len, int kq)
{
    SetData(client);

    std::cout << "len : " << len << std::endl;
    char* buf = new char[len];
    read(client_->GetPipeRead(), buf, len);
    write(2, buf, len);
    struct kevent event;
    EV_SET(&event, client_->GetPipeRead(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    close(client_->GetPipeRead());
    // 첫줄 컨텐츠타입 잘 왔는지 확인
    if (CheckValid(buf) == true) {
        SetFirstLine();
        // 헤더 세팅(파싱)
        // 본문 넣어주기
        // 본문 길이 설정해주기
    };
    // buf delete 해주기

    ParseFirstLine(buf);
    return (buf);

}

bool CGIManager::CheckValid(char* buf) {
    if (strncmp(buf, "Content-type: text/html\n", 24) == 0)
        return true;
    client_->status_code_ == 500;
    client_->req_message_->req_url_ = client_->config_->error_pages_.find(501)->second;
    #if CGI
        std::cout << "[CGI] cgi 첫줄이 content-type이 아님. 상태코드 500번으로 설정" << std::endl;
    #endif
    return false;
}

void CGIManager::SetFirstLine() {
    client_->res_message_->http_version_ = "HTTP/1.1";
    client_->res_message_->status_code_ = 200;
    client_->res_message_->status_text_ = "OK";
}

void CGIManager::ParseCGIData(char* buf) {
    std::string body(buf);

    
}





void CGIManager::ParseFirstLine(char* buf) {
  int64_t curr_idx(0), find_idx(0), end_idx(0);
  /* 첫 줄 찾기 */
  find_idx = strcspn(buf, "\n");  // buf 에서 "\n"의 인덱스 찾는 함수
  end_idx = find_idx;

  char* tmp = new char[end_idx + 1];
  tmp[end_idx] = '\0';
  strncpy(tmp, buf, end_idx + 1);

  /* 첫 단어 Version 쪼개기 */
  find_idx = strcspn(tmp, " ");
  tmp[find_idx] = '\0';
  client_->res_message_->http_version_ = tmp;

  /* 두번째 Status code 쪼개기 */
  curr_idx = find_idx + 1;
  find_idx = strcspn(&tmp[curr_idx], " ");
  tmp[find_idx] = '\0';
  client_->res_message_->status_code_ = atoi(&tmp[curr_idx]);
  
  /* 세번째 Status text 쪼개기 */
  curr_idx = find_idx + 1;
  tmp[end_idx] = '\0';
  client_->res_message_->status_text_ = &tmp[curr_idx];
  delete[] tmp;
}

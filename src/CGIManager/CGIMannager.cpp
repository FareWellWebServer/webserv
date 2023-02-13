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
        #if CGI
            std::cout << "[CGI] fork() 완료" << std::endl;
        #endif
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
        argv[1] = strdup("/Users/dongchoi/webserv_cgi/cgi/cgitest.py");
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
    // while(1);
}

// kqueue에서 cgi read 발생하면!
// 이거 하고 응답메시지 보내주면될듯 
/**
 * @brief 이거 하기 전에 반드시 CGIManager.SetData해야함. 이후에 MSGComposer 호출하면 됨
 * @param len : kevent->data
 */
void CGIManager::GetFromCGI(Data* client, int64_t len, int kq)
{
    SetData(client);

    std::cout << "len : " << len << std::endl;
    char* buf = new char[len + 1];
    read(client_->GetPipeRead(), buf, len);
    buf[len] = '\0';
    write(2, buf, len);
    struct kevent event;
    EV_SET(&event, client_->GetPipeRead(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    close(client_->GetPipeRead());
    ParseCGIData(buf);
    delete[] buf;

    EV_SET(&event, client->GetClientFd(), EVFILT_WRITE, EV_ENABLE, 0, 0, client);
    kevent(kq, &event, 1, NULL, 0, NULL);
}

// bool CGIManager::CheckValid(char* buf) {
//     if (strncmp(buf, "Content-Type: text/html\n", 24) == 0)
//         return true;
//     client_->status_code_ = 500;
//     client_->req_message_->req_url_ = client_->config_->error_pages_.find(501)->second;
//     #if CGI
//         std::cout << "[CGI] cgi 첫줄이 content-type이 아님. 상태코드 500번으로 설정" << std::endl;
//     #endif
//     return false;
// }

void CGIManager::SetFirstLine() {
    client_->res_message_->http_version_ = "HTTP/1.1";
    client_->res_message_->status_code_ = 200;
    // status_taxt 는 비어있는 상태 MSGComposer에서 하기
}

void CGIManager::ParseCGIData(char* buf) {
    
    std::string body = buf;
    size_t idx(0);
    // 헤더 세팅
    idx = SetHeaders(body);
    // 본문 길이 설정
    SetBodyLength(body, idx);
    // 본문 넣어주기
    SetBody(buf, idx);
    // 첫줄 컨텐츠타입 잘 왔는지 확인. 없으면 CGI 에러
    if (client_->res_message_->headers_.find("Content-Type") == client_->res_message_->headers_.end())
        client_->status_code_ = 501; // 상태코드 이거 맞는지 확인 필요
    else
        SetFirstLine(); // 첫줄 세팅
}

size_t CGIManager::SetHeaders(std::string& body) {
    size_t start_idx(0), endline_idx(0), delimiter_idx(0);
    std::string key, val;
    
    while (1) {
		endline_idx = body.find_first_of('\n', start_idx);
		delimiter_idx = body.find_first_of(":", start_idx);
		key = body.substr(start_idx, delimiter_idx - start_idx);
		val = body.substr(delimiter_idx + 1, endline_idx - delimiter_idx - 1);
		RemoveTabSpace(val);
        std::cout << "@ " << key << ": " << val << std::endl;
		client_->res_message_->headers_[key] = val;
        start_idx = endline_idx + 1;
		if (body[endline_idx + 1] == '\n')
			break;
	}
    while (body[start_idx] == '\n')
        start_idx++;
    return start_idx;
}

void CGIManager::SetBodyLength(std::string& body, size_t idx) {
    size_t len(body.length() - (idx + 1));
    client_->res_message_->body_data_.length_ = len;
    if (len > 0)
        client_->res_message_->headers_["Content-Lengh"] = to_string(len);
}

void CGIManager::SetBody(char* buf, size_t idx) {
    if (client_->res_message_->body_data_.length_ != 0)
        client_->res_message_->body_data_.data_ = new char[client_->res_message_->body_data_.length_];
    memcpy(client_->res_message_->body_data_.data_, &buf[idx], client_->res_message_->body_data_.length_);
}

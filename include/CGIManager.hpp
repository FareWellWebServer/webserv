#ifndef CGIMANAGER_HPP
#define CGIMANAGER_HPP

class CGIManager {
    public:
        void sendToCGI(int client_fd, ReqMassage &req_massage);
        void getCGIToRes(ResMassage &resMassage);
    private:
        ResManager *res_manager_;
        map<int, int>(client_fd, pipe[read]); // 나중에 kqueu에서 나온 client_fd로 어떤 pipe[fd]에 접근해야 할지 접근!
};

#endif

// 요청 들어온 것들 CGI 로 넘겨주는 것
// req_massage의 첫줄은 argv로, 헤더는 env로 본문은 stdin으로 보내줌
void CGIManager::sendToCGI(int client_fd, ReqMassage &req_massage)
{
    pipe
    req header to env
    fork
    {
        if (pid > 0)
            req_entity to pipe[write]
            close(pipe[write])
            kqueue (pipe[read])
        else if (pid == 0)
            dup2(pipe[write], 1)
            dup2(pipe[read], 0)
            req_massage->first_line to argv
            req_massage->header to env
            전역 환경변수 선언
            if (execve(cgi_path, argv, env) < 0)
                에러핸들러 호출
    }
}

// kqueue에서 cgi read 발생하면!
// 이거 하고 응답메시지 보내주면될듯 
void CGIManager::getCGIToRes(ResMassage &resMassage)
{
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
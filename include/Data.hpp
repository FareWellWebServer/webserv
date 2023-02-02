#ifndef DATA_HPP
#define DATA_HPP

#include "HTTPMassage.hpp"
#include "ServerConfigInfo.hpp"
#include <string>

#define NULL 0
#define READ 0
#define WRITE 1

// enum stage {
//   READY,
//   REQ_READY,
//   REQ_FINISHED, 
//   GET_READY,
//   GET_CGI, 
//   GET_FINISHED,
//   POST_READY, 
//   POST_PROCESSING,
//   POST_
// };

/* 포인터로 갖고있는 녀석들의 메모리 관리는 어디서 해줄지? 
  객체 자신에서 안해주는거는 Data 에서 해주기
*/

/**
 * @brief Data 에서는 Get만 가능. Set은 Handler에서
 * 
 */

class Data {
  public:
    Data();
    ~Data();
    void Clear();
    int GetListenFd() const;
    int GetListenPort() const;
    int GetClientFd() const; //kevent
    int GetStatusCode() const;
    bool IsTimeout() const;
    int GetFileFd() const;
    int GetPipeWrite() const;
    int GetPipeRead() const;

    /* Get Config */
    // int GetConfig() const; // Config에서 어떤 정보를 뭘 보고 반환해줘야하는지?

    /* Get about Request Massage */
    t_req_msg* GetReqMassage() const;
    std::string GetReqMethod() const;
    std::string GetReqURL() const;
    std::string GetReqProtocol() const;
    std::string GetReqHeaderByKey(std::string &key) const;
    char* GetReqBodyData() const;
    char* GetReqBodyType() const;
    size_t GetReqBodyLength() const;

    /* Get about Response Massage */
    t_res_msg* GetResMassage() const;
    std::string GetResVersion() const;
    int GetResStatusCode() const;
    std::string GetResStatusText() const;
    std::string GetResHeaderByKey(std::string &key) const;
    char* GetResBodyData() const;
    char* GetResBodyType() const;
    size_t GetResBodyLength() const;
    void SetResMassage(t_res_msg* res_msg);
    void SetResEntity(t_entity* entity);
    t_entity* GetResEntity() const;
    


  public:
    int litsen_fd_;  // 어느 listen fd에 연결됐는지
    int port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
    int client_fd_;
    int status_code_;  // 상태코드 enum 정의 필요
    bool timeout_;
    bool cgi;
    int file_fd_;
    int pipe_[2];
    struct kevent* event_;  // fd(ident), flag들
    const ServerConfigInfo* config_;
    t_req_msg* req_message_;
    t_res_msg* res_message_; // ResHandler 에게 보내줄 내용 정리
    t_entity* res_entity_; // method가 넣어주는 곳
    // stage e_stage;
};

#endif





//TODO: timeout setting 어떻게? SetTimeOut(bool)
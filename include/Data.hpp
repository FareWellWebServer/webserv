#ifndef DATA_HPP
#define DATA_HPP

#include "HTTPMessage.hpp"
#include "ServerConfigInfo.hpp"
#include "Stage.hpp"

#include <string>

#define READ 0
#define WRITE 1


class ReqHandler;
class ResHandler;

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
    void Init();
    void Clear();
    /* 멤버변수 Getter() .ccp:54 ~ 102*/
    int GetListenFd() const;
    int GetListenPort() const;
    int GetClientFd() const;
    char* GetClientName() const;
    char* GetClientPort() const;
    int GetStatusCode() const;
    bool IsTimeout() const;
    bool IsCGI() const;
    int GetFileFd() const;
    int GetPipeWrite() const;
    int GetPipeRead() const;

    /* 멤버변수 Setter() .cpp: 106 ~ 148 */
    void SetListenFd(int listen_fd);
    void SetListenPort(int listen_port);
    void SetClientFd(int client_fd);
    void SetClientName(char* client_name);
    void SetClientPort(char* client_port);
    void SetStatusCode(int status_code);
    void SetTimeout(bool is_timeout);
    void SetCGi(bool is_CGI);
    void SetFileFd(int file_fd);
    void SetPipeWrite(int pipe_write_fd);
    void SetPipeRead(int pipe_read_fd);

    /* Get Config */
    // int GetConfig() const; // Config에서 어떤 정보를 뭘 보고 반환해줘야하는지?

    /* Request Message Getter() .cpp:150 ~ 188 */
    t_req_msg* GetReqMessage() const;
    std::string GetReqMethod() const;
    std::string GetReqURL() const;
    std::string GetReqProtocol() const;
    std::string GetReqHeaderByKey(std::string &key) const;
    char* GetReqBodyData() const;
    char* GetReqBodyType() const;
    size_t GetReqBodyLength() const;

    /* Request Message Setter()  .cpp:190 ~ 237 */
    void SetReqMessage(t_req_msg* req_message);
    void SetReqMethod(std::string req_message_method);
    void SetReqURL(std::string req_message_URL);
    void SetReqProtocol(std::string req_message_protocol);
    void SetReqHeaders(std::map<std::string, std::string> &headers);
    void SetReqBody(t_entity* body_entity);
    void SetReqBodyData(char* req_body_data);
    void SetReqBodyType(char* req_body_type);
    void SetReqBodyLength(size_t req_body_length);

    /* Response Message Getter() .cpp:239 ~ 279 */
    t_res_msg* GetResMessage() const;
    std::string GetResVersion() const;
    int GetResStatusCode() const;
    std::string GetResStatusText() const;
    std::string GetResHeaderByKey(std::string &key) const;
    char* GetResBodyData() const;
    char* GetResBodyType() const;
    size_t GetResBodyLength() const;

    /* Response Message Setter() .cpp:281 ~ 319 */
    void SetResMessage(t_res_msg* res_msg);
    void SetResVersion(std::string version);
    void SetResStatusCode(int status_code);
    void SetResStatusText(std::string status_text);
    void SetResHeaders(std::map<std::string, std::string> &headers);
    void SetResBody(t_entity* entity);
    void SetResBodyData(char* res_body_data);
    void SetResBodyType(char* res_body_type);
    void SetResBodyLength(size_t res_body_length);
    
    /* Method Entity Getter() .cpp:321 ~ 337 */
    t_entity* GetMethodEntity() const;
    char* GetMethodEntityData() const;
    size_t GetMethodEntityLength() const;
    char* GetMethodEntityType() const;

    /* Method Entity Setter() .cpp:339 ~ */
    void SetMethodEntity(t_entity* entity);
    void SetMethodEntityData(char *data);
    void SetMethodEntityLength(size_t length);
    void SetMethodEntityType(char *type);


  public:
    int litsen_fd_;  // 어느 listen fd에 연결됐는지
    int listen_port_;  // listen fd에 bind 되어있는 port 번호. config볼 때 필요
    int client_fd_;
    char* client_name_;
    char* client_port_;
    // TODO: request time도 필요한가?
    int status_code_;  // 상태코드 enum 정의 필요
    bool timeout_;
    bool cgi_;
    int file_fd_;
    int pipe_[2];
    struct kevent* event_;  // fd(ident), flag들
    const ServerConfigInfo* config_;
    t_req_msg* req_message_;
    t_res_msg* res_message_; // ResHandler 에게 보내줄 내용 정리
    t_entity* method_entity_; // method가 넣어주는 곳
    // message 구조체들은 요청 들어왔을 때 동적할당해주고, 응답할 때 프리해주면 될듯
    // stage e_stage;
};

#endif

//TODO: timeout setting 어떻게? SetTimeOut(bool)

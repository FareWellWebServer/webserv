#ifndef DATA_HPP
#define DATA_HPP

#include <string>
#include <unistd.h>

#include "HTTPMessage.hpp"
#include "ServerConfigInfo.hpp"

#define READ 0
#define WRITE 1

class ReqHandler;
class ResHandler;

class Data {
 public:
  Data();
  ~Data();
  void Init();
  void Clear();
  /* 멤버변수 Getter() .ccp:72 ~ 108*/
  int GetListenFd() const;
  int GetListenPort() const;
  int GetClientFd() const;
  char* GetClientName() const;
  char* GetClientPort() const;
  int GetStatusCode() const;
  bool IsTimeout() const;
  bool IsCGI() const;
  int GetFileFd() const;
  int GetLogFileFd() const;
  int GetPipeWrite() const;
  int GetPipeRead() const;
  ServerConfigInfo* GetConfig() const;

  /* 멤버변수 Setter() .cpp: 110 ~ 154 */
  void SetListenFd(int listen_fd);
  void SetListenPort(int listen_port);
  void SetClientFd(int client_fd);
  void SetClientName(char* client_name);
  void SetClientPort(char* client_port);
  void SetStatusCode(int status_code);
  void SetTimeout(bool is_timeout);
  void SetCGi(bool is_CGI);
  void SetDirectoryList(bool is_directory_list);
  void SetFileFd(int file_fd);
  void SetLogFileFd(int log_file_fd);
  void SetPipeWrite(int pipe_write_fd);
  void SetPipeRead(int pipe_read_fd);

  /* Request Message Getter() .cpp:156 ~ 182 */
  t_req_msg* GetReqMessage(void) const;
  std::string GetReqMethod(void) const;
  std::string GetReqURL(void) const;
  std::string GetReqProtocol(void) const;
  std::string GetReqHeaderByKey(std::string& key) const;
  t_entity GetReqBody(void) const;
  char* GetReqBodyData(void) const;
  char* GetReqBodyType(void) const;
  size_t GetReqBodyLength(void) const;

  /* Request Message Setter()  .cpp:184 ~ 233 */
  void SetReqMessage(t_req_msg* req_message);
  void SetReqMethod(std::string req_message_method);
  void SetReqURL(std::string req_message_URL);
  void SetReqProtocol(std::string req_message_protocol);
  void SetReqHeaders(std::map<std::string, std::string>& headers);
  void SetReqBody(t_entity* body_entity);
  void SetReqBodyData(char* req_body_data);
  void SetReqBodyType(char* req_body_type);
  void SetReqBodyLength(size_t req_body_length);

  /* Response Message Getter() .cpp:233 ~ 267 */
  t_res_msg* GetResMessage(void) const;
  std::string GetResVersion(void) const;
  int GetResStatusCode(void) const;
  std::string GetResStatusText(void) const;
  std::string GetResHeaderByKey(std::string& key) const;
  t_entity GetResBody(void) const;
  char* GetResBodyData(void) const;
  char* GetResBodyType(void) const;
  size_t GetResBodyLength(void) const;

  /* Response Message Setter() .cpp:269 ~ 309 */
  void SetResMessage(t_res_msg* res_msg);
  void SetResVersion(std::string version);
  void SetResStatusCode(int status_code);
  void SetResStatusText(std::string status_text);
  void SetResHeaders(std::map<std::string, std::string>& headers);
  void SetResBody(t_entity* entity);
  void SetResBodyData(char* res_body_data);
  void SetResBodyType(char* res_body_type);
  void SetResBodyLength(size_t res_body_length);

 public:
  int litsen_fd_;
  int listen_port_;
  int client_fd_;
  char* client_name_;
  char* client_port_;
  int status_code_;
  bool timeout_;
  bool cgi_;
  bool is_directory_list_;
  bool is_download;
  bool is_working;
  bool is_remain;
  bool is_first;
  bool is_loged;
  bool is_chunked;
  bool chunked_done;
  char* chunk_body;
  int file_fd_;
  int log_file_fd_;
  int binary_start_idx;
  int binary_size;
  int pipe_[2];
  int chunk_size;
  int currency;
  struct kevent* event_;
  ServerConfigInfo* config_;
  t_req_msg* req_message_;
  t_res_msg* res_message_;
  std::string post_data_;
  std::string boundary;
  std::string file_name;
};

#endif

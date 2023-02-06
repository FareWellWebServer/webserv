#ifndef REQ_HANDLER_HPP
#define REQ_HANDLER_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <sstream>

<<<<<<< HEAD
// // 테스트 끝나면 지우기

// typedef struct entity {
//   char* entity_;
//   size_t entity_length_;
// } entity;

// typedef struct t_req_msg {
//   std::string method_;
//   std::string req_url_;
//   std::string protocol_;  // 프로토콜 값을 저장하지 말고 파싱딴에서 HTTP/1.1이
//                           // 아니면 에러.
//   std::map<std::string, std::string> headers_;
//   entity body_data_;
// } s_req_msg;

// class ReqHandler {
//  public:
//   s_req_msg req_msg_;
//   void print_structure();
//   ReqHandler(void);
//   ~ReqHandler(void);

//  private:
// };

// // void Parse_Req_msg(int c_socket);
// void Parse_Req_Msg(int c_socket, ReqHandler& reqhandle);

// void Parse_Req_Msg(int c_socket, Data& m_data);

// std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
// std::vector<std::string> s_split(std::string& s, const std::string& delimiter,
//                                  int cnt);
// void Remove_Tab_Space(std::string& str);
// void Print_Map(std::map<std::string, std::string>& map);

#endif
=======
#include "Color.hpp"
#include "Data.hpp"
#include "HTTPMessage.hpp"

class ReqHandler {
 public:
  ReqHandler(void);
  ~ReqHandler(void); // buf 확인하고 해제
  void SetClient(Data* client);
  void SetReadLen(int64_t kevent_data);
  void RecvFromSocket();
  void ParseRecv(int fd);
  t_req_msg* PopReqMassage(); // 동적할당해서 나가고, 받은 쪽에서 delete 처리해주기
  void Clear();
  t_req_msg* req_msg_;

 private:
  char* buf_; // 동적할당 / 해제
  int64_t read_len_;
  Data* client_;
  int64_t ParseFirstLine();
  int64_t ParseHeaders(int start_idx);
  void ParseHeadersSetKeyValue(char *line);
  void ParseEntity(int start_idx);
};

void Print_Map(std::map<std::string, std::string>& map);
void Remove_Tab_Space(std::string& str);
std::vector<std::string> split(const std::string& s, char delimiter, int cnt);
#endif
>>>>>>> develop

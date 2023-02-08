#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <map>

/**
 * @brief HTTP 메시지 본문
 *
 * @param data_ 본문
 * @param type_
 * @param length_
 *
 */
typedef struct s_entity {
  char* data_;
  char* type_;
  size_t length_;
} t_entity;

typedef struct s_req_msg {
  std::string method_;
  std::string req_url_;
  std::string protocol_;  // 프로토콜 값을 저장하지 말고 파싱딴에서 HTTP/1.1이
                          // 아니면 에러.
  std::map<std::string, std::string> headers_;
  t_entity body_data_;
} t_req_msg;

typedef struct s_res_msg {    // 버전, 상태코드, 사유구절
  std::string http_version_;  // HTTP/1.1
  int status_code_;           // metadata에서 받아오기
  std::string status_text_;   // 상태코드에 따라 값 넣어주기

  std::map<std::string, std::string> headers_;
  t_entity body_data_;
} t_res_msg;

#endif

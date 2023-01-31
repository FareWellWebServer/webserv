#ifndef MSGCOMPOSER_HPP
#define MSGCOMPOSER_HPP

#include <stdlib.h>
#include <sstream>
#include <string>

#include "./ClientMetaData.hpp"

typedef struct s_res_msg { // 버전, 상태코드, 사유구절
  std::string http_version_; // HTTP/1.1
  int status_code_; // metadata에서 받아오기
  std::string status_text_;  // 상태코드에 따라 값 넣어주기

  std::map<std::string, std::string> headers_;
  t_entity *body_data_;
} t_res_msg;

// struct Data;

class MsgComposer {
private:
	t_res_msg res_msg_;
	Data* client_;
  std::size_t response_length_;

  void SetHeaders(void);
	void SetStatusText(void); 

public:
    MsgComposer(Data* client);
    ~MsgComposer(void);

	void InitResMsg(void);
  const char *GetResponse(void);

  std::size_t getLength(void) const;
	void Clear(void);
};


#endif
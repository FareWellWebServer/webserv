
#include "../../include/WebServ.hpp"
#include <stdio.h>
//kevent->data에 읽어야 하는 byte 수가 들어옴. 
//한번에 읽으면 될듯.
// ReqHandlerd::ReqHandlerd(void) : buf_(0), read_len_(0), client_(0), req_msg_(0)
// {}
ReqHandlerd::ReqHandlerd(void) : buf_(0), read_len_(0), client_(0)
{}

ReqHandlerd::~ReqHandlerd(void)
{
}

// void RecvFromSocket()
// {
//   if (client_ == NULL || read_len_ == 0) {
//     #if DG
//       std::cout << "[ReqHandler] Recv error. Need client data. call SetClient() or SetReadLen()" << std::endl;
//     #endif
//     return;
//   }

//   //req msg를 읽어오는 과정. recv로.
//   ssize_t recv_return(0);
//   buf_ = new char[sizeof(char) * read_len_];
//   recv_return = recv(client_->GetFd(), buf_, read_len_, 0);
//   if (recv_return == -1)
//   {
//     #if DG
//     std::cout << "[ReqHandler] recv return -1. socket : " << client_->GetFd() << std::endl;
//     #endif
//     // EV_SET;
//     // return ; ?? 어디로 돌아가야되지
//   }
//   else if (recv_return != read_len_)
//   {
//     #if DG
//     std::cout << "[ReqHandler] recv return != kevent->data : \
//     loss data?" << std::endl;
//     #endif
//   }
// }

void ReqHandlerd::ParseEntity(int start_idx) {
  while (buf_[start_idx] == '#' || buf_[start_idx] == '\n') {
    start_idx++;
    if (start_idx == read_len_) {
      #if DG
        std::cout << "[ReqHandler] There is no entity(body)" << std::endl;
      #endif
      return ;
    }
  }
  char *entity = new char[req_msg_->body_data_.entity_length_ + 1];
  memcpy(entity,&buf_[start_idx], req_msg_->body_data_.entity_length_);
  req_msg_->body_data_.entity_ = entity;
}


int64_t ReqHandlerd::ParseHeaders(int start_idx) {
  /* buf_[start_idx]를 헤더의 첫줄로 만들기 */
  while (buf_[start_idx] == '#' || buf_[start_idx] == '\n') {
    start_idx++;
    if (start_idx == read_len_) {
      return(start_idx);
    }
  }
  int64_t curr_idx(start_idx), end_idx(start_idx);

  std::cout << start_idx << std::endl;

  /* buf_에서 한줄씩 찾아서 key-value로 만들어서 넣어주기 */
  int i(0);
  while (start_idx + i < read_len_) {
    if (buf_[start_idx + i] == '#') {
      if (start_idx + i + 3 >= read_len_ || \
        strncmp(&buf_[start_idx + i], "#\n#\n", 4) == 0) { // buf_[start_idx + i + 2] == '#'
        break ;
      }
      buf_[start_idx + i] = '\0';
      // std::cout << &buf_[curr_idx] << "@@" << std::endl <<std::endl;
      ParseHeadersSetKeyValue(&buf_[curr_idx]);
      curr_idx = start_idx + i + 2;
    }
    i++;
  }
  end_idx += i;
  return (end_idx);
}

void ReqHandlerd::ParseHeadersSetKeyValue(char *line) {
  std::string tmp;
  tmp = line;
  std::vector<std::string> kv_tmp;

  std::cout << "line : "<< line << std::endl;

  kv_tmp = split(tmp, ':', 0);

  // key = tmp;
  // key.resize(tmp.find(":"));
  // value = tmp.substr(tmp.find(" ") + 1);
  // while (value.find("\n") != std::string::npos)
  // {
  //   value.erase(value.find("\n"), 1);
  // }
  std::cout << "key : " << kv_tmp[0] << std::endl;
  std::cout << "value : " << kv_tmp[1] << std::endl;
  Remove_Tab_Space(kv_tmp[0]);
  Remove_Tab_Space(kv_tmp[1]);
  if (kv_tmp[0] == "Content-Length"){
    req_msg_->body_data_.entity_length_ = atoi(kv_tmp[1].c_str());
  }
  req_msg_->headers_[kv_tmp[0]] = kv_tmp[1];
}

int64_t ReqHandlerd::ParseFirstLine() { // end_idx = '\n'
  int64_t curr_idx(0), find_idx(0), end_idx(0);
  /* 첫 줄 찾기 */
  find_idx = strcspn(buf_, "\n"); // buf 에서 "\n"의 인덱스 찾는 함수
  while (find_idx != read_len_ && buf_[find_idx - 1] != '#')
    find_idx += strcspn(&buf_[find_idx + 1], "\n");
  end_idx = find_idx;

  /* 첫 줄 유형 확인 */
  find_idx = 0;

  /* 첫 단어 Method 쪼개기 */
  find_idx = strcspn(buf_, " ");
  char* tmp = new char[end_idx + 1];
  if (find_idx >= end_idx)
  {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, buf_, find_idx);
  tmp[find_idx] = '\0';
  /* 메소드 유효성 확인 필요 */
  req_msg_->method_.append(tmp);
  curr_idx = find_idx;
  /* 두번 째 URL 쪼개기 */ 
  find_idx = strcspn(&buf_[curr_idx + 1], " ");
  if (find_idx >= end_idx)
  {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, &buf_[curr_idx + 1], find_idx);
  tmp[find_idx] = '\0';
  /* 올바른 URL 인지 확인 필요 */
  req_msg_->req_url_ = tmp;
  curr_idx = find_idx;

  /* 버전확인 406 Not Acceptable */ 
  find_idx = strcspn(&buf_[curr_idx + 1], "#");
  if (find_idx >= end_idx)
  {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }
  strncpy(tmp, &buf_[curr_idx + 1], find_idx);
  tmp[find_idx] = '\0';

  if (strncmp(tmp, "HTTP/1.1", 8) == 0)
  {
    // client_->SetStatusCode(400); // bad request
    return (read_len_);
  }

  delete[] tmp;
  return (end_idx);
}



// t_req_msg* PopReqMassage()
// {
//   t_req_msg* req(req_msg_);
//   #if DG
//     if (req_msg_ == NULL)
//       std::cout << "ReqHanlder is empty" << std::endl;
//   #endif
//   req_msg_ = NULL;
//   return (req);
// }

// void Clear()
// {
//   if (buf_ != NULL)
//   {
//     delete[] buf_;
//     buf_ = NULL;
//   }
//   read_len_ = 0;
//   client_ = NULL;
//   if (req_msg_ != NULL)
//   {
//     delete req_msg_;
//     req_msg_ = NULL;
//   }
// }

void ReqHandlerd::ParseRecv(int fd)
{
  // if (buf_ == NULL) {
  //   #if DG
  //     std::cout << "[ReqHandler] Parse error. Need buf_. call RecvFromSocket()" << std::endl;
  //   #endif
  //   return;
  // }
  // if (req_msg_ == NULL)
  req_msg_ = new t_req_msg;

  int64_t idx(0);

  read_len_ = 405;
  buf_ = new char[sizeof(char) * read_len_];

  // 첫줄 파싱 //414
  int byte = read(fd, buf_, read_len_);
  std::cout << "Byte : " << byte << std::endl;

  idx = ParseFirstLine(); // buf[idx] = 첫줄의 \n
  // 헤더 파싱
  std::cout << "BF header cout" << std::endl;
  std::cout << "HEADER :" << req_msg_->method_ <<req_msg_->req_url_ << req_msg_->protocol_ << std::endl;
  idx = ParseHeaders(idx); // buf[idx] = 마지막 헤더줄의 /r
  // entity 넣기
  Print_Map(req_msg_->headers_);
  std::cout << "content length :" << req_msg_->body_data_.entity_length_ << std::endl;

  ParseEntity(idx);
  delete[] buf_;
  buf_ = NULL;
}


void Remove_Tab_Space(std::string& str) {
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '\t' || str[i] == ' ') {
      str.erase(i, 1);
      i--;
    }
  }
}

std::vector<std::string> split(const std::string& s, char delimiter, int cnt) {
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream tokenStream(s);

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
    if (cnt == 1) {
      token = tokenStream.str();
      tokens.push_back(token);
      break;
    }
  }
  return tokens;
}

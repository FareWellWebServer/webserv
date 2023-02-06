#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../include/WebServ.hpp"

int main() {
  int fd = open("/Users/sihunlee/42Seoul/webserv/src/test2", O_RDONLY);
  // int fd = open("/Users/silee/42seoul/webserv/src/test2", O_RDONLY);
  if (fd < 0) {
    std::cout << "ERROR" << std::endl;
    std::cout << strerror(errno) << std::endl;
    return 0;
  }
  ReqHandler req;

  req.SetReadLen(405);
  req.SetBuf(fd);

  req.ParseRecv();
  // std::cout << "Entity :"<<req.req_msg_->body_data_.entity_ << std::endl;
  for (size_t i = 0; i < req.req_msg_->body_data_.length_; ++i) {
    printf("%c", req.req_msg_->body_data_.data_[i]);
  }
  // Parse_Req_Msg(fd, reqhandle);
  // system("leaks Req_handle");
}

// int64_t ReqHandlerd::ParseFirstLine() {  // end_idx = '\n'
//   int64_t curr_idx(0), find_idx(0), end_idx(0);
//   /* 첫 줄 찾기 */
//   find_idx = strcspn(buf_, "\n");  // buf 에서 "\n"의 인덱스 찾는 함수
//   while (find_idx != read_len_ && buf_[find_idx - 1] != '#')
//     find_idx += strcspn(&buf_[find_idx + 1], "\n");
//   end_idx = find_idx;

//   /* 첫 줄 유형 확인 */
//   find_idx = 0;
//   /* 첫 단어 Method 쪼개기 */
//   find_idx = strcspn(buf_, " ");
//   char* tmp = new char[end_idx + 1];
//   if (find_idx >= end_idx) {
//     // client_->SetStatusCode(400); // bad request
//     return (read_len_);
//   }
//   strncpy(tmp, buf_, find_idx);
//   tmp[find_idx] = '\0';
//   /* 메소드 유효성 확인 필요 */
//   req_msg_->method_.append(tmp);
//   curr_idx = find_idx;

//   /* 두번 째 URL 쪼개기 */
//   find_idx = strcspn(&buf_[curr_idx + 1], " ");
//   if (find_idx >= end_idx) {
//     // client_->SetStatusCode(400); // bad request
//     return (read_len_);
//   }
//   strncpy(tmp, &buf_[curr_idx + 1], find_idx);
//   tmp[find_idx] = '\0';
//   /* 올바른 URL 인지 확인 필요 */
//   req_msg_->req_url_ = tmp;
//   curr_idx = find_idx;

//   /* 버전확인 406 Not Acceptable */
//   find_idx = strcspn(&buf_[curr_idx + 1], "#");
//   if (find_idx >= end_idx) {
//     // client_->SetStatusCode(400); // bad request
//     return (read_len_);
//   }
//   strncpy(tmp, &buf_[curr_idx + 1], find_idx);
//   tmp[find_idx] = '\0';

//   if (strncmp(tmp, "HTTP/1.1", 8) == 0) {
//     // client_->SetStatusCode(400); // bad request
//     return (read_len_);
//   }
//   req_msg_->protocol_ = tmp;

//   delete[] tmp;
//   return (end_idx);
// }

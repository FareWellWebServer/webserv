#include "../../include/WebServ.hpp"

ReqHandler::ReqHandler(void) {}

void ReqHandler::print_structure() {
  std::cout << GREEN << "METHOD | " << RESET << req_msg_.method_ << std::endl;
  std::cout << GREEN << "URL | " << RESET << req_msg_.req_url_ << std::endl;
  std::cout << GREEN << "PROTOCAL | " << RESET << req_msg_.protocol_
            << std::endl;

  std::cout << "HEADERS | " << std::endl;
}

ReqHandler::~ReqHandler(void) {}


// void ParseReqMsg(int c_socket, ReqHandler& reqhandle) {
//   char* ctmp;
//   std::vector<std::string> contents;
//   for (int i = 0; (ctmp = get_next_line(c_socket)) != 0; ++i){

//     if (strncmp(ctmp, "#@\n",3 ) == 0) {
//       delete[] ctmp;
//       if (i == 0) {
//         std::cout << "ERROR : Start line starts with CRLF" << std::endl;
//         exit(1);
//       }
//       break;
//     }
//     contents.push_back(ctmp);
//     delete[] ctmp;
//   }
//   PrintVector(contents);

//   //-----------------validation required----------
//   // 첫줄에 달랑 CRLF만 있는 경우를 검사
//   for (int i = 0; i < contents.size(); i++)
//   {
//     if (i == 0 && contents[i] == "#@\n"){
//       std::cout << "ERROR : Start line starts with CRLF" << std::endl;
//       exit(1);
//     }
//   }
// }




// 테스트용 코드
void Parse_Req_Msg(int c_socket, ReqHandler& reqhandle) {
  // int seqcrlf = 0;
  char* ctmp;
  std::string contents;

  // gnl현재는 개행을 기준으로 잘라 값을string에 저장했지만
  // 실제 요청 메세지가 온다면 개행 기준이 아니라 \r\n을 기준으로 나눠야한다.

  // TODO : append에 인자로 Stmp가 아닌 ctmp로 변경 ㅇ
  // TODO : req handler에 있는 값값을  초기화 하는 메서드 추기ㅏ

  // TODO : 유효성 검사가 필요. CRLF가 연속적으로 올 경우엔 어떻게 처리해야할까?
  
  // TODO : key : value~~~~~~\nvalue 이런 식으로 value 내에 개행이 들어가는 경우는?

  for (int i = 0; (ctmp = get_next_line(c_socket)) != 0; ++i)
  {
    if (strncmp(ctmp, "#@\n",3 ) == 0) {
      delete[] ctmp;
      if (i == 0) {
        std::cout << "ERROR : Start line starts with CRLF" << std::endl;
        // TODO : 에러를 어떻게 처리할것인지.
        exit(1);
      }
      break;
    }
    contents.append(ctmp);
    delete[] ctmp;
  }
  // data에 요청메세지 전체가 들어가있음.
  std::vector<std::string> data = s_split(contents, "#@\n", 0);

  // //--------------start line parsing -----------------
  std::vector<std::string> start_line = split(data[0], ' ', 0);
  //시작줄에 옵션..? 
  // 공백으로 3개가 이상 들어오는 경우
  // TODO : unvalid protocol case 처리.
  reqhandle.req_msg_.method_ = start_line[0];
  reqhandle.req_msg_.req_url_ = start_line[1];
  if(start_line[2] != "HTTP/1.1")
  {
    std::cout << "Invaild" << std::endl;
  }
  reqhandle.req_msg_.protocol_ = start_line[2];
  start_line.clear();
  data.erase(data.begin());

  //---------------header parsing -------------------
  while (1) {
    std::vector<std::string> head_line = s_split(data[0], ": ", 1);
    if (head_line.size() == 1) {
      break;}
    Remove_Tab_Space(head_line[0]);
    if (head_line[0] == "Content-Length")
      reqhandle.req_msg_.body_data_.entity_length_ = atoi(head_line[1].c_str());
    std::pair<std::string, std::string> tmp =
        std::pair<std::string, std::string>(head_line[0], head_line[1]);
    reqhandle.req_msg_.headers_.insert(tmp);
    data.erase(data.begin());
    head_line.clear();
  }
  //TODO : 필수적으로 알아야 하는 것에 대해 판단하는 녀석. validation 과정이 필요할듯. content length랑 content type이 없으면 에러
  //------------body parsing-----------------------
  // char *buf = new char[reqhandle.req_msg_.body_data_.entity_length_ + 1];
  char *buf;

  int byte;
  if ((byte = read(c_socket, buf, reqhandle.req_msg_.body_data_.entity_length_ + 1024)) == -1)
    std::cout << "ERROR in Reading Entity" << std::endl;
  std::cout << byte << std::endl;
  std::cout << buf << std::endl;

  // size_t total_len = 0;
  // reqhandle.req_msg_.body_data_.entity_ = "";
  // for (int i = 0; (ctmp =  get_next_line(c_socket)) != 0 ||
  //                 total_len < reqhandle.req_msg_.body_data_.entity_length_;
  //      i++) {
  //       //TODO : 0000이 들어가는 경우 문제가 생길수도. char buffer로 한번에 받아서 처리.
  //   total_len += strlen(ctmp);
  //   if (i == 0)
  //     reqhandle.req_msg_.body_data_.entity_ = strdup(ctmp);
  //   else
  //     reqhandle.req_msg_.body_data_.entity_ =
  //         ft_strjoin(reqhandle.req_msg_.body_data_.entity_, ctmp);
  //   delete[] ctmp;
  // }


  // reqhandle.print_structure();
  // Print_Map(reqhandle.req_msg_.headers_);
  // std::cout << std::endl;
  // std::cout << "entity value :" << reqhandle.req_msg_.body_data_.entity_
  //           << std::endl;
}

// 실제 사용할 함수. client 소켓은 어떻게되는지..?
/*
void Parse_Req_Msg(int c_socket, Data& m_data) {
  char* ctmp;
  std::string stmp;
  std::string contents;

  // gnl현재는 개행을 기준으로 잘라 값을string에 저장했지만
  // 실제 요청 메세지가 온다면 개행 기준이 아니라 \r\n을 기준으로 나눠야한다.
  while ((ctmp = get_next_line(c_socket)) != 0) {
    stmp = ctmp;
    if (stmp == "#@\n") {
      stmp.clear();
      delete[] ctmp;
      break;
    }
    contents.append(stmp);
    stmp.clear();
      delete[] ctmp;
  }

  // data에 요청메세지 전체가 들어가있음.
  std::vector<std::string> data = s_split(contents, "#@\n", 0);

  // //--------------start line parsing -----------------
  std::vector<std::string> start_line = split(data[0], ' ', 0);
  m_data.req_message_->req_msg_.method_ = start_line[0];
  m_data.req_message_->req_msg_.req_url_ = start_line[1];
  m_data.req_message_->req_msg_.protocol_ = start_line[2];

  start_line.clear();
  data.erase(data.begin());

  //---------------header parsing -------------------
  while (1) {
    std::vector<std::string> head_line = s_split(data[0], ": ", 1);
    if (head_line.size() == 1) break;
    Remove_Tab_Space(head_line[0]);
    if (head_line[0] == "Content-Length")
      m_data.entity_->entity_length_ = atoi(head_line[1].c_str());

    std::pair<std::string, std::string> t =
        std::pair<std::string, std::string>(head_line[0], head_line[1]);
    m_data.req_message_->req_msg_.headers_.insert(t);
    data.erase(data.begin());
    head_line.clear();
  }

  //------------body parsing-----------------------
  size_t total_len = 0;
  m_data.entity_->entity_ = "";
  for (int i = 0; (ctmp = get_next_line(c_socket)) != 0 ||
                  total_len < m_data.entity_->entity_length_;
       i++) {
    total_len += strlen(ctmp);
    if (i == 0)
      m_data.entity_->entity_ = strdup(ctmp);
    else
      m_data.entity_->entity_ = ft_strjoin(m_data.entity_->entity_, ctmp);
    delete[] ctmp;
  }
}
*/
void Remove_Tab_Space(std::string& str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (str[i] == '\t' || str[i] == ' ') {
      str.erase(i, 1);
      i--;
    }
  }
}

void Print_Map(std::map<std::string, std::string>& map) {
  std::cout << RED << "[[PRINT MAP]]" << RESET << std::endl;
  for (std::map<std::string, std::string>::iterator it = map.begin();
       it != map.end(); ++it) {
    std::cout << "key : " << it->first << " || value : " << it->second
              << std::endl;
  }
}

//content length 가 들어왔다면.
int CheckSeqCRLF(char * buf)
{
  int i = 0;
  int len = 0;

  while (buf[len] != 0)
    len++;
    
std::cout << len << std::endl;
  while (buf[i] && i < len - 4) {
    std::cout << i << std::endl;
    if(buf[i] == '\r')
        if(buf[i + 1] == '\n')
            if(buf[i + 2] == '\r')
                if(buf[i + 3] == '\n')
                    return (i + 3);
    i++;
  }
  return (0);
}

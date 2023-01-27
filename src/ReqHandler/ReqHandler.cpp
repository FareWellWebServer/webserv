#include "../../include/WebServ.hpp"

// typedef struct t_req_msg {
//   std::string method;
//   std::string req_url;
//   std::string protocol;
//   std::map<std::string, std::string> headers;
//   bool is_body;
//   std::string req_body;
// } s_req_msg;

ReqHandler::ReqHandler(void) { std::memset(this, 0, sizeof(ReqHandler)); }
void ReqHandler::print_structure() {
  std::cout << GREEN << "METHOD | " << RESET << req_msg.method << std::endl;
  std::cout << GREEN << "URL | " << RESET << req_msg.req_url << std::endl;
  std::cout << GREEN << "PROTOCAL | " << RESET << req_msg.protocol << std::endl;

  std::cout << "HEADERS | " << std::endl;
}

ReqHandler::~ReqHandler(void) {}

// void Parse_Req_msg(int c_socket) {
//   char* ctmp;
//   std::string stmp;
//   std::string contents;
//   ReqHandler reqhandle;

//   while ((ctmp = get_next_line(c_socket)) != 0) {
//     stmp = ctmp;
//     contents.append(stmp);
//     free(ctmp);
//     stmp.clear();
//   }

//   // data에 요청메세지 전체가 들어가있음.
//   std::vector<std::string> data = s_split(contents, "#@\n", 1);

//   //--------------start line parsing -----------------
//   std::vector<std::string> start_line = split(data[0], ' ', 0);
//   reqhandle.req_msg.method = start_line[0];
//   reqhandle.req_msg.req_url = start_line[1];
//   reqhandle.req_msg.protocol = start_line[2];
//   start_line.clear();
//   data.erase(data.begin());

//   // 위 과정 거치며 요청 시작줄 지워짐.
//   //---------------header parsing? -------------------
//   std::vector<std::string> header_line = s_split(data[0], "#@\n", 1);

//   while (1) {
//     // 한줄 분리
//     std::cout << header_line.size() << std::endl;

//     if (header_line.size() == 1) {
//       std::vector<std::string> header_line = s_split(header_line[0], "#@\n",
//       1);
//     }

//     // 헤더 한줄 내 ": " 을 구분지어 헤더 이름과 헤더필드 값 분할
//     std::vector<std::string> header_value = s_split(header_line[0], ": ", 0);
//     if (header_value.size() == 1) break;
//     PrintVector(header_value);
//     // Remove_Tab_Space(header_value[0]);

//     // reqhandle.req_msg.headers.insert(
//     //     std::make_pair(header_value[0], header_value[1]));
//     header_value.clear();
//     header_line.erase(header_line.begin());
//     data.erase(data.begin());
//   }
//   // Print_Map(reqhandle.req_msg.headers);
// }

void Parse_Req_msg(int c_socket, ReqHandler& reqhandle) {
  char* ctmp;
  std::string stmp;
  std::string contents;

  while ((ctmp = get_next_line(c_socket)) != 0) {
    stmp = ctmp;
    contents.append(stmp);
    free(ctmp);
    stmp.clear();
  }

  // data에 요청메세지 전체가 들어가있음.
  std::vector<std::string> data = s_split(contents, "#@\n", 0);

  // //--------------start line parsing -----------------
  std::vector<std::string> start_line = split(data[0], ' ', 0);
  reqhandle.req_msg.method = start_line[0];
  reqhandle.req_msg.req_url = start_line[1];
  reqhandle.req_msg.protocol = start_line[2];
  start_line.clear();
  data.erase(data.begin());

  // // 위 과정 거치며 요청 시작줄 지워짐.
  // //---------------header parsing? -------------------

  std::cout << reqhandle.req_msg.headers.size() << std::endl;
  while (1) {
    std::vector<std::string> head_line = s_split(data[0], ": ", 1);
    // std::cout << "@@@@@@@@@@@@@@@" << head_line.size() << std::endl;
    if (head_line.size() == 1) break;

    Remove_Tab_Space(head_line[0]);

    std::cout << "" << std::endl;
    PrintVector(head_line);

    std::pair<std::string, std::string> t =
        std::pair<std::string, std::string>(head_line[0], head_line[1]);
    reqhandle.req_msg.headers.insert(t);
    // reqhandle.req_msg.headers.insert({head_line[0], head_line[1]});
    // reqhandle.req_msg.headers[head_line[0]] = head_line[1];
    data.erase(data.begin());
    head_line.clear();
  }
  // PrintVector(data);
  // Print_Map(reqhandle.req_msg.headers);
}

void Remove_Tab_Space(std::string& str) {
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '\t' || str[i] == ' ') {
      str.erase(i, 1);
      i--;
    }
  }
}

void Print_Map(std::map<std::string, std::string>& map) {
  for (std::map<std::string, std::string>::iterator it = map.begin();
       it != map.end(); ++it) {
    std::cout << "key : " << it->first << " || value : " << it->second
              << std::endl;
  }
}
#include "../../include/WebServ.hpp"

// typedef struct t_req_msg {
//   std::string method;
//   std::string req_url;
//   std::string protocol;
//   std::map<std::string, std::string> headers;
//   bool is_body;
//   std::string req_body;
// } s_req_msg;

ReqHandler::ReqHandler(void) {}

void ReqHandler::print_structure() {
  std::cout << GREEN << "METHOD | " << RESET << req_msg.method << std::endl;
  std::cout << GREEN << "URL | " << RESET << req_msg.req_url << std::endl;
  std::cout << GREEN << "PROTOCAL | " << RESET << req_msg.protocol << std::endl;

  std::cout << "HEADERS | " << std::endl;
}

ReqHandler::~ReqHandler(void) {}

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

  std::vector<std::string> data = s_split(contents, "#@\n", 0);
  // //--------------start line parsing -----------------
  std::vector<std::string> start_line = split(data[0], ' ', 0);
  reqhandle.req_msg.method = start_line[0];
  reqhandle.req_msg.req_url = start_line[1];
  reqhandle.req_msg.protocol = start_line[2];
  start_line.clear();
  data.erase(data.begin());

  // 위 과정 거치며 요청 시작줄 지워짐.
  //---------------header parsing -------------------
  while (1) {
    std::vector<std::string> head_line = s_split(data[0], ": ", 1);
    if (head_line.size() == 1) break;
    Remove_Tab_Space(head_line[0]);
    std::cout << "" << std::endl;
    PrintVector(head_line);
    std::pair<std::string, std::string> t =
        std::make_pair(head_line[0], head_line[1]);
    reqhandle.req_msg.headers.insert(t);
    data.erase(data.begin());
    head_line.clear();
  }
  std::cout << "" << std::endl;
  std::cout << "" << std::endl;
  Print_Map(reqhandle.req_msg.headers);
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
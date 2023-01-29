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
// 테스트용 코드
void Parse_Req_Msg(int c_socket, ReqHandler& reqhandle) {
  char* ctmp;
  std::string stmp;
  std::string contents;

  // gnl현재는 개행을 기준으로 잘라 값을string에 저장했지만
  // 실제 요청 메세지가 온다면 개행 기준이 아니라 \r\n을 기준으로 나눠야한다.
  while ((ctmp = get_next_line(c_socket)) != 0) {
    stmp = ctmp;
    if (stmp == "#@\n") {
      stmp.clear();
      free(ctmp);
      break;
    }
    contents.append(stmp);
    stmp.clear();
    free(ctmp);
  }

  // data에 요청메세지 전체가 들어가있음.
  std::vector<std::string> data = s_split(contents, "#@\n", 0);

  // //--------------start line parsing -----------------
  std::vector<std::string> start_line = split(data[0], ' ', 0);
  reqhandle.req_msg_.method_ = start_line[0];
  reqhandle.req_msg_.req_url_ = start_line[1];
  reqhandle.req_msg_.protocol_ = start_line[2];
  start_line.clear();
  data.erase(data.begin());

  //---------------header parsing -------------------
  while (1) {
    std::vector<std::string> head_line = s_split(data[0], ": ", 1);
    if (head_line.size() == 1) break;
    Remove_Tab_Space(head_line[0]);
    if (head_line[0] == "Content-Length")
      reqhandle.req_msg_.body_data_.entity_length_ = atoi(head_line[1].c_str());
    std::pair<std::string, std::string> t =
        std::pair<std::string, std::string>(head_line[0], head_line[1]);
    reqhandle.req_msg_.headers_.insert(t);
    data.erase(data.begin());
    head_line.clear();
  }

  //------------body parsing-----------------------
  size_t total_len = 0;
  reqhandle.req_msg_.body_data_.entity_ = "";
  for (int i = 0; (ctmp = get_next_line(c_socket)) != 0 ||
                  total_len < reqhandle.req_msg_.body_data_.entity_length_;
       i++) {
    total_len += strlen(ctmp);
    if (i == 0) {
      reqhandle.req_msg_.body_data_.entity_ = strdup(ctmp);
      free(ctmp);
    } else
      reqhandle.req_msg_.body_data_.entity_ =
          ft_strjoin(reqhandle.req_msg_.body_data_.entity_, ctmp);
  }
}

// 실제 사용할 함수. client 소켓은 어떻게되는지..?
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
      free(ctmp);
      break;
    }
    contents.append(stmp);
    stmp.clear();
    free(ctmp);
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
      m_data.entity_.entity_length_ = atoi(head_line[1].c_str());

    std::pair<std::string, std::string> t =
        std::pair<std::string, std::string>(head_line[0], head_line[1]);
    m_data.req_message_->req_msg_.headers_.insert(t);
    data.erase(data.begin());
    head_line.clear();
  }

  //------------body parsing-----------------------
  size_t total_len = 0;
  m_data.entity_.entity = "";
  for (int i = 0; (ctmp = get_next_line(c_socket)) != 0 ||
                  total_len < m_data.entity_.entity_length;
       i++) {
    total_len += strlen(ctmp);
    if (i == 0) {
      m_data.entity_.entity = strdup(ctmp);
      free(ctmp);
    } else
      m_data.entity_.entity = ft_strjoin(m_data.entity_.entity, ctmp);
  }
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
  std::cout << RED << "[[PRINT MAP]]" << RESET << std::endl;
  for (std::map<std::string, std::string>::iterator it = map.begin();
       it != map.end(); ++it) {
    std::cout << "key : " << it->first << " || value : " << it->second
              << std::endl;
  }
}
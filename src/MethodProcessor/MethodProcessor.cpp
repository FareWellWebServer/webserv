#include "../../include/MethodProcessor.hpp"

//  Public
MethodProcessor::MethodProcessor(
    const std::vector<ServerConfigInfo>& server_list) {
  size_t circuit = server_list.size();
  t_entity* ret;
  std::ifstream index;

  for (size_t i = 0; i < circuit; i++) {
    ret = new t_entity();
    if (ret == NULL) {
      /*error handling*/
    }
		
    ret->length_ = FileSize(server_list.at(i).file_path_.c_str());
    ret->data_ = new char[ret->length_];
    if (ret->data_ == NULL) {
      /*error handling*/
    }
    index.open(server_list.at(i).file_path_, std::ifstream::in);
    index.read(ret->data_, ret->length_);
    cache_entity_.insert(
        std::pair<const int, t_entity*>(server_list.at(i).port_, ret));
    index.close();
  }
}

MethodProcessor::~MethodProcessor(void) {
  // size_t limit = cache_entity_.size();
  // for (size_t i = 0; i < limit; i++) {
  //   delete[] cache_entity_.at(i)->data_;
  //   delete[] cache_entity_.at(i)->type_;
  //   delete cache_entity_.at(i);
  // }
  // cache_entity_.clear();
}

void MethodProcessor::ChangeEvents(unsigned long ident, short filter,
                                   short flags, unsigned int fflags, long data,
                                   void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
}

void MethodProcessor::GETFirst(int curfd, ClientMetaData* clients,
                               struct kevent& cur_event) {
  Data* client = clients->GetDataByFd(curfd);
  switch (client->e_stage) {
    case GET_HTML:
      GETSecond(curfd, clients, cur_event);
      break;
    case GET_FILE:
      GETSecondFile(curfd, clients, cur_event);
      break;
    case GET_CGI:
      GETSecondCgi(curfd, clients, cur_event);
      break;
    case REQ_FINISHED:
      FetchOiginalPath(client->req_message_->req_url_, *client);
      if (IsCgi(client->req_message_->req_url_)) {
        /*handling cgi*/
        client->e_stage = GET_CGI;
        int cgi_stream[2];
        int pid = 0;

        if (pipe(cgi_stream) == -1) {
          MakeErrorStatus(*client, 500);
          return;
        }

        {
          int flags = fcntl(cgi_stream[0], F_GETFL, 0);
          fcntl(cgi_stream[0], F_SETFL, flags | O_NONBLOCK);
          flags = fcntl(cgi_stream[1], F_GETFL, 0);
          fcntl(cgi_stream[1], F_SETFL, flags | O_NONBLOCK);
        }

        pid = fork();
        if (pid == -1) {
          // TODO : error handling
        }
        if (pid == 0) {
          /* CGI handling*/
          close(cgi_stream[0]);
          dup2(cgi_stream[1], 1);

          char** cgi_argv;
          size_t cgi_length = client->GetReqURL().size();

          cgi_argv = new char*[2];
          cgi_argv[0] = new char[cgi_length + 1];
          cgi_argv[0][cgi_length] = '\0';
          for (size_t i = 0; i < cgi_length; i += 2) {
            cgi_argv[0][i] = client->GetReqURL().at(i);
            if (i + 1 < cgi_length)
              cgi_argv[0][i + 1] = client->GetReqURL().at(i + 1);
          }
          cgi_argv[1] = new char[1];
          cgi_argv[1][0] = '\0';
          if (execve(cgi_argv[0], cgi_argv, environ) == -1) exit(-1);
        } else {
          /* Parent handling*/
          close(cgi_stream[1]);
          ChangeEvents(cgi_stream[0], EVFILT_READ, EV_ADD, 0, 0, clients);
          int loc = 0;
          wait3(&loc, WNOHANG, NULL);
          return;
        }
      } else if (IsFile(client->req_message_->req_url_, PNG) ||
                 IsFile(client->req_message_->req_url_, JPG) ||
                 IsFile(client->req_message_->req_url_, ICO)) {
        /* handling File */
        client->e_stage = GET_FILE;
        if (IsFile(client->req_message_->req_url_, PNG))
          client->SetResBodyType(strdup(TYPE_PNG));
        else if (IsFile(client->req_message_->req_url_, JPG))
          client->SetResBodyType(strdup(TYPE_JPEG));
        else if (IsFile(client->req_message_->req_url_, ICO))
          client->SetResBodyType(strdup(TYPE_ICON));
        else
          client->SetResBodyType(strdup(TYPE_DEFAULT));
        int data_fd = open(client->req_message_->req_url_.c_str(), O_RDONLY);
        int flags = fcntl(data_fd, F_GETFL, 0);
        fcntl(data_fd, F_SETFL, flags | O_NONBLOCK);
        ChangeEvents(data_fd, EVFILT_READ, EV_ADD, 0, 0, client);
        return;
      } else {
        /*handling index.html*/
        client->e_stage = GET_HTML;
        /* cache data index.html */
        if (cache_entity_.find(client->listen_port_) != cache_entity_.end()) {
          client->SetResBodyType(strdup(TYPE_HTML));
          client->SetResBodyData(strdup(
              cache_entity_.find(client->listen_port_).operator*().second->data_));
          client->SetResBodyLength(cache_entity_.find(client->listen_port_).operator*().second->length_);
          client->e_stage = GET_FINISHED;

          ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0,
                       NULL);
          // TODO : kevent 들고 와야 함
          return;
        } else /* GET another html */ {
          client->SetResBodyType(strdup(TYPE_HTML));
          int data_fd = open(client->GetReqURL().c_str(), O_RDONLY);
          int flags = fcntl(data_fd, F_GETFL, 0);
          fcntl(data_fd, F_SETFL, flags | O_NONBLOCK);
          ChangeEvents(data_fd, EVFILT_READ, EV_ADD, 0, 0, client);
          return;
        }
      }
      break;
    default:
      /* error handling*/
      break;
  }
}

void MethodProcessor::POSTFirst(int curfd, ClientMetaData* clients,
                                struct kevent& cur_event) {
  // TODO : POST 분기 구현
  (void)curfd;
  (void)clients;
  (void)cur_event;
}

void MethodProcessor::DELETE(int curfd, ClientMetaData* clients,
                             struct kevent& cur_event) {
  // TODO : DELETE 구현~
  (void)curfd;
  (void)clients;
  (void)cur_event;
}

// Private

void MethodProcessor::MakeErrorStatus(Data& client, int code) {
  client.status_code_ = code;
  if (client.GetResBodyData()) {
    delete[] client.GetResBodyData();
  }
  client.SetResBodyData(NULL);
  if (client.GetResBodyType()) {
    delete[] client.GetResBodyType();
  }
  client.SetResBodyType(NULL);
  client.SetResBodyLength(0);
  // delete &client.GetResBody();
}

void MethodProcessor::FetchOiginalPath(std::string& uri, Data& client) {
  uri.erase(0);
  uri.insert(0, client.config_->file_path_);
  return;
}

bool MethodProcessor::IsFetched(std::string& uri, Data& client) {
  size_t i = 0;

  while (client.config_->file_path_[i]) {
    if (client.config_->file_path_[i] == uri[i]) {
      i++;
    } else {
      return (false);
    }
  }
  return (true);
}

bool MethodProcessor::IsExistFile(std::string& uri) {
  const char* temp = uri.c_str();

  int ret = access(temp, F_OK);
  delete[] temp;

  if (ret != 0) return (false);
  return (true);
}

bool MethodProcessor::IsCgi(std::string& uri) {
  std::string::reverse_iterator it = uri.rbegin();
  if (it[0] == CGI[2]) {
    if (it[1] == CGI[1]) {
      if (it[2] == CGI[0]) {
        return (true);
      }
    }
  }
  return (false);
}

bool MethodProcessor::IsFile(std::string& uri, const char* identifier) {
  std::string::reverse_iterator it = uri.rbegin();
  if (it[0] == identifier[2]) {
    if (it[1] == identifier[1]) {
      if (it[2] == identifier[0]) {
        return (true);
      }
    }
  }
  return (false);
}

char* MethodProcessor::CopyCstr(const char* cstr, size_t length) {
  char* ret = new char[length + 1];

  if (ret == NULL) {
    /*error handling*/;
  }
  ret[length] = '\0';
  for (size_t i = 0; i < length; i += 2) {
    ret[i] = cstr[i];
    if (i + 1 < length) ret[i + 1] = cstr[i + 1];
  }
  return ret;
}

void MethodProcessor::GETSecondCgi(int curfd, ClientMetaData* clients,
                                   struct kevent& cur_event) {
  // TODO : CGI 읽기 구현
  Data* client = clients->GetData();
  client->e_stage = GET_FINISHED;
  client->SetResBodyLength(cur_event.data);
  client->SetResBodyData(new char(client->GetResBodyLength()));
  if (client->GetResBodyData() == NULL) {
    // TODO : error handling;
  }
  read(curfd, client->GetResBodyData(), client->GetResBodyLength());
  ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  return;
}

void MethodProcessor::GETSecondFile(int curfd, ClientMetaData* clients,
                                    struct kevent& cur_event) {
  Data* client = clients->GetData();
  client->e_stage = GET_FINISHED;
  client->SetResBodyLength(FileSize(client->GetReqURL().c_str()));

  client->SetResBodyData(new char(client->GetResBodyLength()));
  if (client->GetResBodyData() == NULL) {
    // TODO : error handling;
  }
  read(curfd, client->GetResBodyData(), client->GetResBodyLength());
  ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  (void)cur_event;
  return;
}

void MethodProcessor::GETSecond(int curfd, ClientMetaData* clients,
                                struct kevent& cur_event) {
  Data* client = clients->GetDataByFd(curfd);
  client->e_stage = GET_FINISHED;
  client->SetResBodyLength(FileSize(client->GetReqURL().c_str()));

  client->SetResBodyData(new char[client->GetResBodyLength()]);
  if (client->GetResBodyData() == NULL) {
    // TODO : error handling;
  }
  read(curfd, client->GetResBodyData(), client->GetResBodyLength());
  ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  (void)cur_event;
  return;
}

void MethodProcessor::POSTSecond(int curfd, ClientMetaData* clients,
                                 struct kevent& cur_event) {
  // TODO : 쓰기구현
  (void)curfd;
  (void)clients;
  (void)cur_event;
}

void MethodProcessor::POSTThird(int curfd, ClientMetaData* clients,
                                struct kevent& cur_event) {
  // TODO : POST 파일 무결성 검사 구현
  (void)curfd;
  (void)clients;
  (void)cur_event;
}

int MethodProcessor::FileSize(const char* filepath) {
  struct stat file_info;
  int ret;

  ret = stat(filepath, &file_info);
  if (ret < 0) {
    return (-1);
  }
  return (file_info.st_size);
}




// seojin

void MethodProcessor::GET(int client_fd, Data& data, struct kevent& event, t_req_msg* 
req_msg) {
	(void) client_fd;
	(void) data;
	(void) event;
	(void) req_msg;


	std::cout << "hello\n";
}

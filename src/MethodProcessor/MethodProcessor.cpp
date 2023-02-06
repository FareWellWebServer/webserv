#include "../../include/WebServ.hpp"

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
    ret->type_ = strdup(TYPE_HTML);
    ret->length_ = FileSize(server_list.at(i).file_path.c_str());
    ret->data_ = new char[ret->length_];
    if (ret->data_ == NULL) {
      /*error handling*/
    }
    index.open(server_list.at(i).file_path, std::ifstream::in);
    index.read(ret->data_, ret->length_);
    cache_entity_.insert(
        std::pair<const int, t_entity*>(server_list.at(i).port, ret));
    index.close();
  }
}

MethodProcessor::~MethodProcessor(void) {
  size_t limit = cache_entity_.size();
  for (size_t i = 0; i < limit; i++) {
    delete[] cache_entity_.at(i)->data_;
    delete[] cache_entity_.at(i)->type_;
    delete cache_entity_.at(i);
  }
  cache_entity_.clear();
}

void MethodProcessor::GETFirst(int curfd, ClientMetaData* clients,
                               struct kevent& cur_event) {
  Data* client = &clients->GetData();
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
      FetchOiginalPath(client->req_message_->req_msg_.req_url_, *client);
      if (IsCgi(client->req_message_->req_msg_.req_url_)) {
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
          int flags = fcntl(cgi_stream[1], F_GETFL, 0);
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
          size_t cgi_length = client->req_message_->req_msg_.req_url_.size();

          cgi_argv = new char*[2];
          cgi_argv[0] = new char[cgi_length + 1];
          cgi_argv[0][cgi_length] = '\0';
          for (size_t i = 0; i < cgi_length; i += 2) {
            cgi_argv[0][i] = client->req_message_->req_msg_.req_url_.at(i);
            if (i + 1 < cgi_length)
              cgi_argv[0][i + 1] =
                  client->req_message_->req_msg_.req_url_.at(i + 1);
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
      } else if (IsFile(client->req_message_->req_msg_.req_url_, PNG) ||
                 IsFile(client->req_message_->req_msg_.req_url_, JPG) ||
                 IsFile(client->req_message_->req_msg_.req_url_, ICO)) {
        /* handling File */
        client->e_stage = GET_FILE;
        if (IsFile(client->req_message_->req_msg_.req_url_, PNG))
          client->entity_->type_ = strdup(TYPE_PNG);
        else if (IsFile(client->req_message_->req_msg_.req_url_, JPG))
          client->entity_->type_ = strdup(TYPE_JPEG);
        else if (IsFile(client->req_message_->req_msg_.req_url_, ICO))
          client->entity_->type_ = strdup(TYPE_ICON);
        else
          client->entity_->type_ = strdup(TYPE_DEFAULT);
        int data_fd =
            open(client->req_message_->req_msg_.req_url_.c_str(), O_RDONLY);
        int flags = fcntl(data_fd, F_GETFL, 0);
        fcntl(data_fd, F_SETFL, flags | O_NONBLOCK);
        ChangeEvents(data_fd, EVFILT_READ, EV_ADD, 0, 0, client);
        return;
      } else {
        /*handling index.html*/
        client->e_stage = GET_HTML;
        /* cache data index.html */
        if (cache_entity_.find(client->port_) != cache_entity_.end()) {
          client->entity_->type_ = strdup(TYPE_HTML);
          client->entity_->data_ = strdup(
              cache_entity_.find(client->port_).operator*().second->data_);
          client->entity_->length_ =
              cache_entity_.find(client->port_).operator*().second->length_;
          client->e_stage = GET_FINISHED;

          ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0,
                       NULL);
          // TODO : kevent 들고 와야 함
          return;
        } else /* GET another html */ {
          client->entity_->type_ = strdup(TYPE_HTML);
          int data_fd =
              open(client->req_message_->req_msg_.req_url_.c_str(), O_RDONLY);
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
}

void MethodProcessor::DELETE(int curfd, ClientMetaData* clients,
                             struct kevent& cur_event) {
  // TODO : DELETE 구현~
}

// Private

void MethodProcessor::MakeErrorStatus(struct Data& client, int code) {
  client.status_code_ = code;
  if (client.entity_->data_) {
    delete[] client.entity_->data_;
  }
  client.entity_->data_ = NULL;
  if (client.entity_->type_) {
    delete[] client.entity_->type_;
  }
  client.entity_->type_ = NULL;
  client.entity_->length_ = 0;
  delete client.entity_;
}

void MethodProcessor::FetchOiginalPath(std::string& uri, struct Data& client) {
  uri.erase(0);
  uri.insert(0, client.config_->file_path);
  return;
}

bool MethodProcessor::IsFetched(std::string& uri, struct Data& client) {
  size_t i = 0;

  while (client.config_->file_path[i]) {
    if (client.config_->file_path[i] == uri[i]) {
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
  const char* temp = uri.c_str();

  int ret = access(temp, F_OK);
  delete[] temp;

  if (ret != 0) return (false);
  return (true);
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
  Data* client = &clients->GetData();
  client->e_stage = GET_FINISHED;
  client->entity_->length_ = cur_event.data;
  client->entity_->data_ = new char[client->entity_->length_];
  if (client->entity_->data_ == NULL) {
    // TODO : error handling;
  }
  int ret = read(curfd, client->entity_->data_, client->entity_->length_);
  ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  return;
}

void MethodProcessor::GETSecondFile(int curfd, ClientMetaData* clients,
                                    struct kevent& cur_event) {
  Data* client = &clients->GetData();
  client->e_stage = GET_FINISHED;
  client->entity_->length_ =
      FileSize(client->req_message_->req_msg_.req_url_.c_str());

  client->entity_->data_ = new char[client->entity_->length_];
  if (client->entity_->data_ == NULL) {
    // TODO : error handling;
  }
  int ret = read(curfd, client->entity_->data_, client->entity_->length_);
  ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  return;
}

void MethodProcessor::GETSecond(int curfd, ClientMetaData* clients,
                                struct kevent& cur_event) {
  Data* client = &clients->GetData();
  client->e_stage = GET_FINISHED;
  client->entity_->length_ =
      FileSize(client->req_message_->req_msg_.req_url_.c_str());

  client->entity_->data_ = new char[client->entity_->length_];
  if (client->entity_->data_ == NULL) {
    // TODO : error handling;
  }
  int ret = read(curfd, client->entity_->data_, client->entity_->length_);
  ChangeEvents(client->event_->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  return;
}

void MethodProcessor::POSTSecond(int curfd, ClientMetaData* clients,
                                 struct kevent& cur_event) {
  // TODO : 쓰기구현
}

void MethodProcessor::POSTThird(int curfd, ClientMetaData* clients,
                                struct kevent& cur_event) {
  // TODO : POST 파일 무결성 검사 구현
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

void ChangeEvents(uintptr_t ident, int16_t filter, uint16_t flags,
                  uint32_t fflags, intptr_t data, void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
}

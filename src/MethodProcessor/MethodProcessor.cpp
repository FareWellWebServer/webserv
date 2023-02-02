#include "../../include/WebServ.hpp"

MethodProcessor::MethodProcessor(void) {}

MethodProcessor::~MethodProcessor(void) {
  std::map<int, t_entity *>::iterator it = cache_entity_.begin();
  while (it != cache_entity_.end()) {
    delete[] it.operator->()->second->data_;
    it = cache_entity_.begin();
  }
  cache_entity_.clear();
}

void MethodProcessor::MethodProcessorInput(ClientMetaData *clients) {
  struct Data *client;
  client = &clients->GetData();
  if (client->req_message_->req_msg_.method_ == "GET")
    MethodGET(client);
  else if (client->req_message_->req_msg_.method_ == "HEAD")
    MethodHEAD(client);
  else if (client->req_message_->req_msg_.method_ == "POST")
    MethodPOST(client);
  else if (client->req_message_->req_msg_.method_ == "PUT")
    MethodPUT(client);
  else
    MethodDELETE(client);
}

void MethodProcessor::MakeErrorStatus(struct Data &client, int code) {
  client.status_code_ = code;
  if (client.entity_->data_) {
    delete[] client.entity_->data_;
  }
  client.entity_->data_= NULL;
  client.entity_->type_ = NULL;
  client.entity_->length_ = 0;
}

void MethodProcessor::FetchOiginalPath(std::string &uri) {
  uri.erase(0);
  uri.insert(0, INSTALLPATH);
  return;
}

bool MethodProcessor::IsFetched(std::string &uri) {
  size_t i = 0;

  while (INSTALLPATH[i]) {
    if (INSTALLPATH[i] == uri[i]) {
      i++;
    } else {
      return (false);
    }
  }
  return (true);
}

bool MethodProcessor::IsExistFile(std::string &uri) {
  const char *temp = uri.c_str();

  int ret = access(temp, F_OK);
  delete[] temp;

  if (ret != 0) return (false);
  return (true);
}

bool MethodProcessor::IsCgi(std::string &uri) {
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

bool MethodProcessor::IsFile(std::string &uri, const char *identifier) {
  std::string::reverse_iterator it = uri.rbegin();

  size_t len = strlen(identifier);

  for (size_t i = 0; i < len; i++) {
    if (it[i] == identifier[len - i])
      continue;
    else
      return (false);
  }
  return (true);
}

char *MethodProcessor::CopyCstr(const char *cstr, size_t length) {
  char *ret = new char[length + 1];
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

void MethodProcessor::MethodGETCgi(struct Data *client) {
  int cgi_stream[2];
  int pid = 0;

  if (pipe(cgi_stream) == -1) {
    MakeErrorStatus(*client, 500);
    return;
  }
  pid = fork();
  if (pid == 0) {
    /* CGI handling*/
    close(cgi_stream[0]);
    dup2(cgi_stream[1], 1);

    char **cgi_argv;
    size_t cgi_length = client->req_message_->req_msg_.req_url_.size();

    cgi_argv = new char *[2];
    cgi_argv[0] = new char[cgi_length + 1];
    cgi_argv[0][cgi_length] = '\0';
    for (size_t i = 0; i < cgi_length; i += 2) {
      cgi_argv[0][i] = client->req_message_->req_msg_.req_url_.at(i);
      if (i + 1 < cgi_length)
        cgi_argv[0][i + 1] = client->req_message_->req_msg_.req_url_.at(i + 1);
    }
    cgi_argv[1] = new char[1];
    cgi_argv[1][0] = '\0';
    if (execve(cgi_argv[0], cgi_argv, environ) == -1) exit(-1);
  } else {
    /* Parent handling*/
    close(cgi_stream[1]);

    int waitloc;
    if (wait(&waitloc) == -1) {
      MakeErrorStatus(*client, 500);
    } else {
      if (WIFEXITED(waitloc)) {
        char *buf;
        ssize_t len = 0;

        buf = new char[BUFFER_MAX + 1];
        if (!buf) {
          MakeErrorStatus(*client, 500);
          return;
        }
        std::string temp_data;
        while (true) {
          len = read(cgi_stream[0], buf, BUFFER_MAX);
          if (len == -1) {
            delete[] buf;
            MakeErrorStatus(*client, 500);
            close(cgi_stream[0]);
            return;
          }
          buf[BUFFER_MAX] = '\0';
          temp_data.append(buf);
          if (len != BUFFER_MAX) {
            delete[] buf;
            break;
          }
        }
        size_t temp_length = temp_data.size();
        client->entity_->length_ = temp_length;
        client->entity_->data_ =
            CopyCstr(temp_data.c_str(), temp_length);
      } else if (WIFSIGNALED(waitloc)) {
        MakeErrorStatus(*client, 500);
      }
      close(cgi_stream[0]);
      return;
    }
  }
}

void MethodProcessor::MethodGETFile(struct Data *client) {
  t_entity *ret;
  ret = new t_entity();
  if (ret == NULL) {
    /* error handling */
  }
  std::ifstream entityFile;

  entityFile.open(client->req_message_->req_msg_.req_url_, std::ifstream::in);
  entityFile.seekg(0, entityFile.end);
  ret->length_ = entityFile.tellg();
  entityFile.seekg(0, entityFile.beg);

  ret->data_ = new char[ret->length_ + 1];
  ret->data_[ret->length_] = '\0';

  entityFile.read(ret->data_, ret->length_);

  client->entity_->data_ = ret->data_;
  return;
}
void MethodProcessor::MethodGET(struct Data *client) {
  if (!IsFetched(client->req_message_->req_msg_.req_url_))
    FetchOiginalPath(client->req_message_->req_msg_.req_url_);
  if (!IsExistFile(client->req_message_->req_msg_.req_url_)) {
    MakeErrorStatus(*client, 404);
    return;
  }
  if (IsCgi(client->req_message_->req_msg_.req_url_)) {
    MethodGETCgi(client);
    return;
  }
  if (IsFile(client->req_message_->req_msg_.req_url_, PNG) ||
      IsFile(client->req_message_->req_msg_.req_url_, JPG) ||
      IsFile(client->req_message_->req_msg_.req_url_, ICO)) {
    MethodGETFile(client);
    if (IsFile(client->req_message_->req_msg_.req_url_, PNG))
      client->entity_->type_ = strdup("image/png");
    else if (IsFile(client->req_message_->req_msg_.req_url_, JPG))
      client->entity_->type_ = strdup("image/jpge");
    else if (IsFile(client->req_message_->req_msg_.req_url_, ICO))
      client->entity_->type_ = strdup(TYPE_ICON);
    else
      client->entity_->type_ = strdup(TYPE_DEFAULT);
    return;
  }
  std::map<int, t_entity *>::iterator check_cache =
      cache_entity_.find(client->port_);
  if (check_cache != cache_entity_.end()) {
    client->entity_->length_ = check_cache->second->length_;
    client->entity_->data_ = CopyCstr(check_cache->second->data_,
                                             client->entity_->length_);
    client->entity_->type_ = check_cache->second->type_;
    return;
  }

  t_entity *ret;
  ret = new t_entity();
  if (ret == NULL) {
    MakeErrorStatus(*client, 500);
    return;
  }
  std::ifstream entityFile;

  entityFile.open(client->req_message_->req_msg_.req_url_, std::ifstream::in);
  entityFile.seekg(0, entityFile.end);
  ret->length_ = entityFile.tellg();
  entityFile.seekg(0, entityFile.beg);

  ret->data_ = new char[ret->length_];
  if (!ret->data_) {
    MakeErrorStatus(*client, 500);
    return;
  }

  entityFile.read(ret->data_, ret->length_);

  client->entity_->data_ = ret->data_;
  client->entity_->type_ = strdup(TYPE_HTML);
  cache_entity_.insert(std::pair<const int, t_entity *>(client->port_, ret));
  return;
}

void MethodProcessor::MethodHEAD(struct Data *client) {
  MethodGET(client);
  delete[] client->entity_->data_;
  client->entity_->data_ = NULL;
  //TODO : type 넣어줘야함
  return;
}

void MethodProcessor::MethodPOST(struct Data *client) {
  static_cast<void>(client);
}

void MethodProcessor::MethodPUT(struct Data *client) {
  static_cast<void>(client);
}

void MethodProcessor::MethodDELETE(struct Data *client) {
  static_cast<void>(client);
}

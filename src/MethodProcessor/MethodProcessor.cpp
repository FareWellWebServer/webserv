#include "../../include/MethodProcessor.hpp"

#include "../../include/ErrorHandler.hpp"
#include "../../include/Logger.hpp"
#include "../../include/WebServ.hpp"
#include "../../include/WebServException.hpp"

MethodProcessor::MethodProcessor(void) {}

MethodProcessor::~MethodProcessor(void) {
  std::map<int, t_entity *>::iterator it = cache_entity_.begin();
  while (it != cache_entity_.end()) {
    delete[] it.operator->()->second->entity_data_;
    std::map<int, t_entity *>::iterator it = cache_entity_.begin();
  }
  cache_entity_.clear();
}

void MethodProcessor::MethodProcessorInput(ClientMetaData *clients) {
  struct Data *client;
  client = clients->GetData(clients->GetPort());
  if (client->req_message_->method_ == "GET")
    MethodGET(client);
  else if (client->req_message_->method_ == "HEAD")
    MethodHEAD(client);
  else if (client->req_message_->method_ == "POST")
    MethodPOST(client);
  else if (client->req_message_->method_ == "PUT")
    MethodPUT(client);
  else
    MethodDELETE(client);
}

void MethodProcessor::MakeErrorStatus(struct Data &client, int code) {
  client.status_code_ = code;
  if (client.entity_->entity_data_) delete[] client.entity_->entity_data_;
  client.entity_->entity_data_ = NULL;
  client.entity_->entity_type_ = NULL;
  client.entity_->entity_length_ = 0;
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
      if (it[2] == CGI[0]) return (true);
    }
  } else
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
    size_t cgi_length = client->req_message_->req_uri_.size();

    cgi_argv = new char *[2];
    cgi_argv[0] = new char[cgi_length + 1];
    cgi_argv[0][cgi_length] = '\0';
    for (size_t i = 0; i < cgi_length; i += 2) {
      cgi_argv[0][i] = client->req_message_->req_uri_.at(i);
      if (i + 1 < cgi_length)
        cgi_argv[0][i + 1] = client->req_message_->req_uri_.at(i + 1);
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
        size_t len = 0;

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
        client->entity_->entity_length_ = temp_length;
        client->entity_->entity_data_ =
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

  entityFile.open(client->req_message_->req_uri_, std::ifstream::in);
  entityFile.seekg(0, entityFile.end);
  ret->entity_length_ = entityFile.tellg();
  entityFile.seekg(0, entityFile.beg);

  ret->entity_data_ = new char[ret->entity_length_ + 1];
  ret->entity_data_[ret->entity_length_] = '\0';

  entityFile.read(ret->entity_data_, ret->entity_length_);

  client->entity_->entity_data_ = ret->entity_data_;
  return;
}
void MethodProcessor::MethodGET(struct Data *client) {
  if (!IsFetched(client->req_message_->req_uri_))
    FetchOiginalPath(client->req_message_->req_uri_);
  if (!IsExistFile(client->req_message_->req_uri_)) {
    MakeErrorStatus(*client, 404);
    return;
  }
  if (IsCgi(client->req_message_->req_uri_)) {
    MethodGETCgi(client);
    return;
  }
  if (IsFile(client->req_message_->req_uri_, PNG) ||
      IsFile(client->req_message_->req_uri_, JPG) ||
      IsFile(client->req_message_->req_uri_, ICO)) {
    MethodGETFile(client);
    if (IsFile(client->req_message_->req_uri_, PNG))
      client->entity_->entity_type_ = TYPE_PNG;
    else if (IsFile(client->req_message_->req_uri_, JPG))
      client->entity_->entity_type_ = TYPE_JPEG;
    else if (IsFile(client->req_message_->req_uri_, ICO))
      client->entity_->entity_type_ = TYPE_ICON;
    else
      client->entity_->entity_type_ = TYPE_DEFAULT;
    return;
  }
  std::map<int, t_entity *>::iterator check_cache =
      cache_entity_.find(client->port_);
  if (check_cache != cache_entity_.end()) {
    client->entity_->entity_length_ = check_cache->second->entity_length_;
    client->entity_->entity_data_ = CopyCstr(check_cache->second->entity_data_,
                                             client->entity_->entity_length_);
    client->entity_->entity_type_ = check_cache->second->entity_type_;
    return;
  }

  t_entity *ret;
  ret = new t_entity();
  if (ret == NULL) {
    MakeErrorStatus(*client, 500);
    return;
  }
  std::ifstream entityFile;

  entityFile.open(client->req_message_->req_uri_, std::ifstream::in);
  entityFile.seekg(0, entityFile.end);
  ret->entity_length_ = entityFile.tellg();
  entityFile.seekg(0, entityFile.beg);

  ret->entity_data_ = new char[ret->entity_length_];
  if (!ret->entity_data_) {
    MakeErrorStatus(*client, 500);
    return;
  }

  entityFile.read(ret->entity_data_, ret->entity_length_);

  client->entity_->entity_data_ = ret->entity_data_;
  client->entity_->entity_type_ = TYPE_HTML;
  cache_entity_.emplace(client->port_, ret);
  return;
}

void MethodProcessor::MethodHEAD(struct Data *client) {
  MethodGET(client);
  delete[] client->entity_->entity_data_;
  client->entity_->entity_data_ = NULL;
  return;
}

void MethodProcessor::MethodPOST(struct Data *client) {}

void MethodProcessor::MethodPUT(struct Data *client) {}

void MethodProcessor::MethodDELETE(struct Data *client) {}
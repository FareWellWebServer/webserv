#include "../../include/Server.hpp"

void Server::Get(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  t_req_msg* req_msg = client->GetReqMessage();
  struct kevent event;

  std::string req_url = req_msg->req_url_;
  std::string file_path;
  if (client->is_directory_list_ == true) {
    const std::string html_str = generate_directory_list(req_url);
    char* tmp_html_str = new char[html_str.size()];
    for (size_t i = 0; i < html_str.size(); ++i) {
      tmp_html_str[i] = html_str[i];
    }
    client->res_message_->status_code_ = 200;
    client->res_message_->body_data_.data_ = tmp_html_str;
    client->res_message_->body_data_.length_ = html_str.size();

    client->res_message_->headers_["Content-Type"] = "text/html";
    client->res_message_->headers_["Content-Length"] =
        to_string(html_str.size());

    EV_SET(&event, client->GetClientFd(), EVFILT_WRITE, EV_ENABLE, 0, 0,
           client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
  } else if (client->cgi_ == true) {
    cgi_manager_->SendToCGI(client, kq_);

  } else {
    int file_fd = open(req_msg->req_url_.c_str(), O_RDONLY);
    if (file_fd == -1) {
      client->SetStatusCode(404);
      client->res_message_->headers_["Content-Type"] = "text/html";
      client->res_message_->headers_["Content-Length"] = "22";
      client->res_message_->body_data_.data_ = strdup("<h3>404 Not Found</h3>");
      client->res_message_->body_data_.length_ = 22;
      EV_SET(&event, file_fd, EVFILT_READ, EV_ADD, 0, 0, client);
      kevent(kq_, &event, 1, NULL, 0, NULL);
      return;
    }
    std::string file_name =
        req_msg->req_url_.substr(req_msg->req_url_.rfind('/'));
    size_t pos = file_name.rfind('.');

    if (pos == std::string::npos) {
      client->res_message_->headers_["Content-Type"] =
          "text/plain; charset=UTF-8";
    } else {
      std::string file_extention = file_name.substr(pos + 1);
      if (file_extention == "html") {
        client->res_message_->headers_["Content-Type"] = "text/html";
      } else if (file_extention == "png") {
        client->res_message_->headers_["Content-Type"] = "image/png";
      } else if (file_extention == "jpg") {
        client->res_message_->headers_["Content-Type"] = "image/jpeg";
      } else if (file_extention == "txt") {
        client->res_message_->headers_["Content-Type"] =
            "ext/plain; charset=UTF-8";
      } else if (file_extention == "py") {
        client->res_message_->headers_["Content-Type"] = "text/x-python";
      } else if (file_extention == "ico") {
        client->res_message_->headers_["Content-Type"] = "image/x-icon";
      }
    }

    client->SetFileFd(file_fd);
    EV_SET(&event, file_fd, EVFILT_READ, EV_ADD, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
  }
}

void Server::Post(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  ServerConfigInfo* config = client->config_;
  t_req_msg* req_msg = client->GetReqMessage();
  struct kevent event;

  std::string content_type = req_msg->headers_["Content-Type"];
  if (content_type == "application/x-www-form-urlencoded") {
    std::string encoded_string;
    std::string decoded_string;
    size_t len = req_msg->body_data_.length_;
    for (size_t i = 0; i < len; ++i)
      encoded_string.push_back(req_msg->body_data_.data_[i]);

    decoded_string = decode(encoded_string);

    if (decoded_string.find("title") == std::string::npos ||
        decoded_string.find("content") == std::string::npos) {
      client->SetStatusCode(501);
      client->SetReqURL(config->error_pages_.find(501)->second);
      client->is_remain = false;
      Get(idx);
      return;
    }

    std::string query = decoded_string;
    int and_pos = query.find('&');

    std::string title = query.substr(0, and_pos);
    int equal_pos = title.find('=');
    title = title.substr(equal_pos + 1, and_pos - equal_pos);

    std::string content = query.substr(and_pos);
    equal_pos = content.find('=');
    content = content.substr(equal_pos + 1);

    int file_fd = open((config->upload_path_ + title).c_str(),
                       O_WRONLY | O_CREAT | O_TRUNC);
    if (file_fd == -1) {
      client->SetStatusCode(501);
      client->SetReqURL(client->config_->error_pages_.find(501)->second);
      client->is_remain = false;
      std::cout << RED << "OPEN ERROR\n";
      Get(idx);
      return;
    }
    fchmod(file_fd, S_IRWXU | S_IRWXG | S_IRWXO);

    client->post_data_ = content;

    and_pos = encoded_string.find('&');
    std::string encoded_title = encoded_string.substr(0, and_pos);
    equal_pos = encoded_title.find('=');
    encoded_title = encoded_title.substr(equal_pos + 1, and_pos - equal_pos);
    client->res_message_->headers_["Location"] =
        config->upload_path_ + encoded_title;

    std::string upload_path = config->upload_path_;
    upload_path = upload_path.substr(1);
    std::string down_link =
        "<a href=\"http://127.0.0.1:" + to_string(config->port_) +
        "/download/" + title + "\" download>" + title + "</a>";
    client->res_message_->body_data_.data_ = strdup(down_link.c_str());
    client->res_message_->body_data_.length_ = down_link.size();
    client->res_message_->headers_["Content-Type"] = "text/html; charset=UTF-8";
    client->res_message_->headers_["Content-Length"] =
        to_string(down_link.size());
    config->locations_.find("/download")
        ->second.file_path_.push_back(
            config->locations_.find("/download")->second.root_path_ + title);

    client->SetStatusCode(201);
    client->SetFileFd(file_fd);

    EV_SET(&event, file_fd, EVFILT_WRITE, EV_ADD, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
    client->is_remain = false;
  } else {
    size_t semicolon_pos = content_type.find(';');
    std::string boundary = content_type.substr(semicolon_pos + 1);
    size_t equal_pos = boundary.find('=');

    content_type = content_type.substr(0, semicolon_pos);
    boundary = boundary.substr(equal_pos + 1);
    if (content_type == "multipart/form-data") {
      if (client->req_message_->body_data_.data_ == NULL) {
        client->SetReqMethod("GET");
        client->SetStatusCode(501);
        client->req_message_->req_url_ = config->error_pages_.find(501)->second;
        Get(idx);
        return;
      }

      std::string data_info;
      size_t index = 0;
      if (client->is_first) {
        for (; strncmp(&client->req_message_->body_data_.data_[index],
                       "\r\n\r\n", 4);
             ++index) {
          data_info.push_back(client->req_message_->body_data_.data_[index]);
        }

        /* 바운더리 정보만 오고 뒤에 아무것도 안 올 때 */
        if (data_info.size() == client->req_message_->body_data_.length_) {
          client->SetReqMethod("GET");
          client->SetStatusCode(501);
          client->req_message_->req_url_ =
              config->error_pages_.find(501)->second;
          Get(idx);
          return;
        }
        std::string content_type =
            data_info.substr(data_info.find("Content-Type"));
        content_type = content_type.substr(content_type.find(':') + 2);

        if (content_type != "image/png" && content_type != "image/jpeg") {
          client->SetReqMethod("GET");
          client->SetStatusCode(501);
          client->req_message_->req_url_ =
              config->error_pages_.find(501)->second;
          client->is_remain = false;
          Get(idx);
          return;
        }

        std::string file_name = data_info.substr(data_info.find("filename="));
        file_name = file_name.substr(file_name.find('"') + 1);
        file_name = file_name.substr(0, file_name.find('"'));

        if (file_name.size() == 0) {
          client->SetStatusCode(501);
          client->req_message_->req_url_ =
              config->error_pages_.find(501)->second;
          client->is_remain = false;
          Get(idx);
          return;
        }
        client->file_name = file_name;
      }

      size_t size = 0;
      size_t tmp_idx = index;
      const char* cbody_data = client->req_message_->body_data_.data_;
      std::string body_data;
      for (; index < client->req_message_->body_data_.length_; ++index) {
        body_data.push_back(cbody_data[index]);
      }

      if (body_data.find(boundary) ==
          std::string::npos) {  // 바운더리가 없으면.
        client->SetStatusCode(100);
        client->boundary = boundary;
        if (client->is_first == true) {
          client->binary_start_idx = tmp_idx + 4;
          client->binary_size =
              client->req_message_->body_data_.length_ - data_info.size() - 4;
        } else {
          client->binary_start_idx = tmp_idx;
          client->binary_size = client->req_message_->body_data_.length_;
        }
        client->is_remain = true;
      } else {  // 바운더리가 있으면.
        index = tmp_idx;
        for (; strncmp(&cbody_data[index], boundary.c_str(), boundary.size());
             ++index) {
          ++size;
        }

        if (client->is_first == true) {
          client->binary_start_idx = tmp_idx + 4;
        } else {
          client->binary_start_idx = tmp_idx;
        }
        client->binary_size = size;
        client->res_message_->headers_["Location"] =
            config->upload_path_ + client->file_name;

        std::string upload_path = config->upload_path_;
        upload_path = upload_path.substr(1);
        std::string down_link =
            "<a href=\"http://127.0.0.1:" + to_string(config->port_) +
            "/download/" + client->file_name + "\" download>" +
            client->file_name + "</a>";
        client->res_message_->body_data_.data_ = strdup(down_link.c_str());
        client->res_message_->body_data_.length_ = down_link.size();
        client->res_message_->headers_["Content-Type"] =
            "text/html; charset=UTF-8";
        client->res_message_->headers_["Content-Length"] =
            to_string(down_link.size());

        // download 로케이션 없으면  seg fault 에러 뜸
        config->locations_.find("/download")
            ->second.file_path_.push_back(
                config->locations_.find("/download")->second.root_path_ +
                client->file_name);

        client->SetStatusCode(201);
        client->is_remain = false;
      }

      if (client->is_first == true) {
        int file_fd = open((config->upload_path_ + client->file_name).c_str(),
                           O_RDWR | O_CREAT | O_TRUNC);
        fchmod(file_fd, S_IRWXU | S_IRWXG | S_IRWXO);
        client->SetFileFd(file_fd);
        EV_SET(&event, file_fd, EVFILT_WRITE, EV_ADD, 0, 0, client);
        kevent(kq_, &event, 1, NULL, 0, NULL);
        client->is_first = false;
      } else {
        EV_SET(&event, client->GetFileFd(), EVFILT_WRITE, EV_ENABLE, 0, 0,
               client);
        kevent(kq_, &event, 1, NULL, 0, NULL);
      }
    } else {
      client->SetReqMethod("GET");
      client->SetStatusCode(501);
      client->req_message_->req_url_ = config->error_pages_.find(501)->second;
      Get(idx);
    }
  }
}

void Server::Delete(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  ServerConfigInfo* config = client->GetConfig();
  int client_fd = client->GetClientFd();

  std::string file_name =
      client->GetReqURL().substr(client->GetReqURL().rfind('/') + 1);
  int fd = open((config->upload_path_ + file_name).c_str(), O_RDONLY);
  if (fd == -1) {
    client->SetStatusCode(400);
    client->res_message_->body_data_.data_ = strdup("<h3>Not Exist File</h3>");
    client->res_message_->body_data_.length_ = 23;
    client->res_message_->headers_["Content-Type"] = "text/html";
    client->res_message_->headers_["Content-Length"] = "23";
  } else {
    unlink((config->upload_path_ + file_name).c_str());
    client->SetFileFd(fd);
    client->SetStatusCode(200);
    client->res_message_->body_data_.data_ = strdup("<h3>Success Delete</h3>");
    client->res_message_->body_data_.length_ = 23;
    client->res_message_->headers_["Content-Type"] = "text/html";
    client->res_message_->headers_["Content-Length"] = "23";
  }

  struct kevent event;
  EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}

void Server::HandleChunkedData(int idx) {
  Data* client = reinterpret_cast<Data*>(events_[idx].udata);
  const ServerConfigInfo* config = client->GetConfig();
  struct kevent event;
  int client_fd = client->GetClientFd();

  if (client->req_message_->body_data_.data_ == NULL) {
    client->SetStatusCode(100);
    EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, client);
    kevent(kq_, &event, 1, NULL, 0, NULL);
    return;
  }

  if (client->is_first == true) {
    client->file_name = "chunked_data_file";
    int file_fd = open((config->upload_path_ + client->file_name).c_str(),
                       O_RDWR | O_CREAT | O_TRUNC);
    fchmod(file_fd, S_IRWXU | S_IRWXG | S_IRWXO);
    client->SetFileFd(file_fd);
    client->is_first = false;
  }

  char* body_data = new char[client->req_message_->body_data_.length_];
  int current_size = client->currency;
  int body_size = 0;
  size_t i = 0;
  for (; i < client->req_message_->body_data_.length_; ++i) {
    if (client->chunk_size == -1) {
      int size = 0;
      while (strncmp(&client->req_message_->body_data_.data_[i], "\r\n", 2)) {
        size = (16 * size) +
               (client->req_message_->body_data_.data_[i] >= 'a'
                    ? client->req_message_->body_data_.data_[i] - 87
                    : client->req_message_->body_data_.data_[i] - 48);
        ++i;
      }
      client->chunk_size = size;
      current_size = 0;
      i += 2;
    }
    if (client->chunk_size == 0) {
      EV_SET(&event, client->GetFileFd(), EVFILT_WRITE, EV_ENABLE, 0, 0,
             client);
      kevent(kq_, &event, 1, NULL, 0, NULL);
      client->SetStatusCode(204);
      client->chunked_done = true;
      client->is_remain = false;
      break;
    }
    body_data[body_size] = client->req_message_->body_data_.data_[i];
    ++current_size;
    ++body_size;

    if (current_size == client->chunk_size) {
      client->chunk_size = -1;
      i += 2;
    }
  }
  client->currency = current_size;
  client->chunk_body = body_data;
  client->binary_size = body_size;
  EV_SET(&event, client->GetFileFd(), EVFILT_WRITE, EV_ADD, 0, 0, client);
  kevent(kq_, &event, 1, NULL, 0, NULL);
}

#include "../../include/Server.hpp"

Server::Server(const Config& config)
    : kq_(kqueue()),
      server_infos_(config.GetServerConfigInfos()),
      logger_(Logger(kq_, config.GetLogPath())) {
  clients_ = new ClientMetaData;
  req_handler_ = new ReqHandler;
  msg_composer_ = new MsgComposer;
  cgi_manager_ = new CGIManager;
  session_ = new Session;
}

Server::~Server(void) {
  close(kq_);
  servers_.clear();
  delete clients_;
  delete req_handler_;
  delete msg_composer_;
  delete session_;
}

void Server::Run(void) {
  if (servers_.size() == 0) {
    throw std::runtime_error("[Server Error]: no listening server");
  }
  while (true) {
    Act();
  }
}

// private
void Server::Act(void) {
  int n = kevent(kq_, NULL, 0, events_, MAXLISTEN, NULL);
#if SERVER
  std::cout << "////////// [Server] event count : " << n << " //////////"
            << std::endl;
#endif
  if (n == -1) {
    throw std::runtime_error("Error: kevent()");
  }
  for (int idx = 0; idx < n; ++idx) {
    // log file
    if (static_cast<int>(events_[idx].ident) == logger_.GetLogFileFD()) {
      ExecuteLogEvent();
      continue;
    };

    /* listen port로 새로운 connect 요청이 들어옴 */
    if (IsListenFd(events_[idx].ident) && events_[idx].filter == EVFILT_READ) {
      AcceptNewClient(idx);
      continue;
    }

    /* accept 된 port로 request 요청메세지 들어옴 */
    if (events_[idx].filter == EVFILT_READ) {
      ExecuteReadEvent(idx);
      continue;
    }

    if (events_[idx].filter == EVFILT_WRITE) {
      ExecuteWriteEvent(idx);
      continue;
    }

    if (events_[idx].filter == EVFILT_PROC) {
      ExecuteProcessEvent(idx);
      continue;
    }

    // /* timeout 발생시 */
    if (events_[idx].filter == EVFILT_TIMER) {
      ExecuteTimerEvent(idx);
      continue;
    }
  }
}

#ifndef MSG_COMPOSER_HPP
#define MSG_COMPOSER_HPP

#include <sstream>
#include "Data.hpp"

class MsgComposer {
 public:
  MsgComposer();
  ~MsgComposer(void);
  void SetData(Data* client);
  void InitResMsg(Data* client);
  const char* GetResponse(Data* client);
  std::size_t GetLength(void) const;

 private:
  Data* client_;
  size_t response_length_;
  std::map<int, std::string> status_infos_;

  void SetHeaders(void);
  void SetStatusText(void);
};

#endif

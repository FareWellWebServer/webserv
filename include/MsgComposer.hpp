#ifndef MSG_COMPOSER_HPP
#define MSG_COMPOSER_HPP

#include "Data.hpp"
#include "Types.hpp"

class MsgComposer {
 public:
  MsgComposer(Data* client);
  ~MsgComposer(void);
  void InitResMsg(void);
  const char* GetResponse(void);
  std::size_t getLength(void) const;
  void Clear(void);

 private:
  t_res_msg res_msg_;
  Data* client_;
  std::size_t response_length_;
  std::map<int, std::string> status_infos_;

  void SetHeaders(void);
  void SetStatusText(void);
};

#endif

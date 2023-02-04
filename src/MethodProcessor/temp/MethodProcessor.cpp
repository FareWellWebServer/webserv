#include "../../include/WebServ.hpp"

//  Public

MethodProcessor::MethodProcessor(
    const std::vector<ServerConfigInfo>& server_list) {
  size_t circuit = server_list.size();

  for (size_t i = 0; i < circuit; i++) {
  }
}

MethodProcessor::~MethodProcessor(void) {}
int MethodProcessor::MethodProcessorInput(ClientMetaData* clients) {}
void MethodProcessor::GETFirst(struct Data* client) {}
void MethodProcessor::POSTFirst(struct Data* client) {}
void MethodProcessor::DELETE(struct Data* client) {}

// Private

void MethodProcessor::MakeErrorStatus(struct Data& client, int code) {}
void MethodProcessor::FetchOiginalPath(std::string& uri, struct Data& client) {}
bool MethodProcessor::IsFetched(std::string& uri, struct Data& client) {}
bool MethodProcessor::IsExistFile(std::string& uri) {}
bool MethodProcessor::IsCgi(std::string& uri) {}
bool MethodProcessor::IsFile(std::string& uri, const char* identifier) {}
char* MethodProcessor::CopyCstr(const char* cstr, size_t length) {}
void MethodProcessor::GETSecondCgi(struct Data* client) {}
void MethodProcessor::GETSecondFile(struct Data* client) {}
void MethodProcessor::GETSecond(struct Data* client) {}
void MethodProcessor::POSTSecond(struct Data* client) {}
void MethodProcessor::POSTThird(struct Data* client) {}
int MethodProcessor::FileSize(const char* filepath) {}
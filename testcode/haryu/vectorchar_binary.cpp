#include <fstream>
#include <iostream>
#include <vector>

int main(void) {
  std::ifstream png;
  int file_length;

  // 파일 오픈
  png.open("./test.png", std::ifstream::in);

  // 길이 측정
  png.seekg(0, png.end);
  file_length = png.tellg();
  png.seekg(0, png.beg);

  // 데이터 담기
  char *buf = new char[file_length];
  png.read(buf, file_length);

  // 데이터 각 개별 데이터로 담기 위해 for문을 활용
  // code optimization 기법에서 발췌, 다중 작업을 통해 데이터 전달 속도를 늘림.
  std::vector<char> entity_data;
  for (int i = 0; i < file_length; i += 4) {
    entity_data.push_back(buf[i]);
    if (i + 1 == file_length) break;
    entity_data.push_back(buf[i + 1]);
    if (i + 2 == file_length) break;
    entity_data.push_back(buf[i + 2]);
    if (i + 3 == file_length) break;
    entity_data.push_back(buf[i + 3]);
  }
  std::cout << buf[0] << buf[1] << buf[2] << std::endl;
  std::cout << entity_data[0] << entity_data[1] << entity_data[2] << std::endl;

  // data 무결성 확인용
  std::ofstream copied;
  copied.open("copied.png", std::ofstream::out | std::ofstream::app);

  for (int i = 0; i < file_length; i += 2) {
    copied << entity_data[i];
    if (i + 1 == file_length) break;
    copied << entity_data[i + 1];
    if (i + 2 == file_length) break;
    copied << entity_data[i + 2];
    if (i + 2 == file_length) break;
    copied << entity_data[i + 3];
  }
#if CHAR
  std::cout
      << "====================== test char stdout =========================="
      << std::endl;
  for (int i = 0; i < file_length; i++) {
    std::cout << buf[i];
  }
  std::cout << std::endl;
#else
  std::cout
      << "====================== test vector out =========================="
      << std::endl;
  for (int i = 0; i < file_length; i += 2) {
    std::cout << entity_data[i];
    if (i + 1 == file_length) break;
    std::cout << entity_data[i];
  }
  std::cout << std::endl;
#endif
  png.close();
  copied.close();

  // 파일 사이즈 점검
  std::cout << "file size : " << file_length << std::endl;
  std::cout << "vector size : " << entity_data.size() << std::endl;
  delete[] buf;
  return 0;
}
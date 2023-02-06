#include <iostream>
#include <map>

struct data {
  char *data;
  char *type;
  int length;
};

int main(void) {
  data *test;
  std::map<int, data *> list;

  for (int i = 0; i < 4; i++) {
    test = new data();
    test->data = strdup("helloworld");
    test->type = strdup("test/html");
    test->length = strlen(test->data);

    list.insert(std::pair<int, data *>(i, test));
  }

  for (int i = 0; i < 4; i++) {
    std::cout << i << " : " << list.at(i)->data << " : " << list.at(i)->type
              << std::endl;
  }

  for (int i = 0; i < 4; i++) {
    delete[] list.at(i)->data;
    delete[] list.at(i)->type;
    delete list.at(i);
  }
  list.clear();
  system("leaks a.out");
  return 0;
}
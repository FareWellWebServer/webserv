#include <iostream>

#define COUT std::cout
#define CEND std::endl

int main(int ac, char **av) {
  if (ac == 1) {
    COUT << "Usage : ./a.out {argv 1} {argv 2} ..." << CEND;
    return (0);
  }

#if DG
  COUT << "This is debug mode, so you can make print some inner data" << CEND;
  (void)av;
#else
  COUT << "Print argv" << CEND;
  for (int i = 0; i < ac; i++) {
    COUT << i << " : " << av[i] << CEND;
  }
#endif
  return 0;
}

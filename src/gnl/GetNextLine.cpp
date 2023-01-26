#include "../../include/Webserv.hpp"

char *get_next_line(int fd) {
  static char *str;
  static int is_end;
  char *ret;

  if (fd < 0 || BUFFER_SIZE <= 0 || is_end == 1) {
    if (is_end == 1) is_end = 0;
    return (0);
  }
  str = txt_read(fd, str, &is_end);
  if (str == 0) return (0);
  ret = ret_line(&str);
  if (ret == 0) {
    free(str);
    return (0);
  }
  return (ret);
}

char *txt_read(int fd, char *str, int *is_end) {
  int byte;
  char *buffer;

  buffer = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
  if (buffer == 0) return (0);
  byte = 1;
  while (byte > 0 && find_next(str, '\n') < 0) {
    byte = read(fd, buffer, BUFFER_SIZE);
    if (byte <= 0) {
      if (byte == 0) *is_end = 1;
      break;
    }
    buffer[byte] = '\0';
    str = ft_strjoin(str, buffer);
    if (str == 0) break;
  }
  free(buffer);
  if (byte < 0 || str == 0) return (0);
  return (str);
}

int find_next(char *s, char c) {
  int i;

  i = 0;
  if (s == 0) return (-1);
  while (s[i] != '\0') {
    if (s[i] == c) return (i);
    i++;
  }
  return (-2);
}

int ft_strlen(char *str) {
  int len;

  if (str == 0) return (0);
  len = 0;
  while (str[len] != '\0') len++;
  return (len);
}

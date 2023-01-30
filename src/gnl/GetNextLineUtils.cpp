#include "../../include/WebServ.hpp"

char *ft_strjoin(char *str, char *buffer) {
  int sum;

  sum = 0;
  if (buffer == 0 || *buffer == '\0') return (str);
  sum = ft_strlen(str) + ft_strlen(buffer);

  char *result = new char[sum + 1];
  if (result == 0) return (0);
  *result = '\0';
  ft_strcat(result, str);
  ft_strcat(result, buffer);
  if (str != 0) {
    delete[] str;
  }
  return (result);
}

char *ft_strcat(char *s1, char *s2) {
  int i;
  int j;

  i = 0;
  j = 0;
  if (s2 == 0) return (s1);
  while (s1[i] != '\0') i++;
  while (s2[j] != '\0') {
    s1[i + j] = s2[j];
    j++;
  }
  s1[i + j] = '\0';
  return (s1);
}

char *dup_loc(char *str, int loc_of_next) {
  int i;

  if (loc_of_next == -1) return (0);
  if (loc_of_next == -2) loc_of_next = ft_strlen(str) - 1;
  char *p = new char[loc_of_next + 2];
  if (p == 0) return (0);
  i = 0;
  while (*(str + i) != '\0' && i <= loc_of_next) {
    *(p + i) = *(str + i);
    i++;
  }
  *(p + i) = '\0';
  return (p);
}

char *ft_substr(char *str, int start, int str_len) {
  int i;

  if (start == -1) {
    delete[] str;
    return (0);
  }
  i = 0;
  char *substr = new char[str_len - start + 1];
  if (substr == 0) return (0);
  while (i < str_len - start) {
    *(substr + i) = *(str + start + i);
    i++;
  }
  *(substr + i) = '\0';
  delete[] str;
  return ((char *)substr);
}

char *ret_line(char **str) {
  int loc_of_next;
  int strlen;
  char *oneline;

  if (**str == 0) return (0);
  strlen = ft_strlen(*str);
  loc_of_next = find_next(*str, '\n');
  if (loc_of_next == -1) return (0);
  oneline = dup_loc(*str, loc_of_next);
  if (oneline == 0) return (0);
  *str = ft_substr(*str, loc_of_next + 1, strlen);
  if (*str == 0 && oneline != 0 && loc_of_next != -2) {
    delete[] oneline;
    return (0);
  }
  return (oneline);
}

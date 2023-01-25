/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EnChar.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haryu <haryu@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/21 00:12:05 by haryu             #+#    #+#             */
/*   Updated: 2023/01/21 00:36:04 by haryu            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EnChar_hpp_
#define EnChar_hpp_

#include <iostream>
#include <vector>

class EnChar {
 public:
  typedef char value_type;
  typedef std::vector<char>::iterator iterator;
  typedef std::vector<char>::const_iterator const_iterator;
  typedef std::vector<char>::reverse_iterator reverse_iterator;
  typedef std::vector<char>::const_reverse_iterator const_reverse_iterator;
  typedef std::vector<char>::pointer pointer;
  typedef std::vector<char>::const_pointer const_pointer;
  typedef std::vector<char>::reference reference;
  typedef std::vector<char>::const_reference const_reference;
  typedef std::vector<char>::size_type size_type;

 private:
  std::vector<char> entity_;

 public:
  /**
   * @brief char * 로 저장된 데이터 가공및 사용을 편리하게 만든 vector
   * wrapping class
   *
   * @param buf_ char * entity raw data 포인터를 넣기
   * @param len_ char * buf 할당 시 획득한 데이터 사이즈
   */
  EnChar(size_t len_, char* buf_);
  EnChar(const EnChar& target_);
  ~EnChar(void);
  EnChar& operator=(const EnChar& target_);
  value_type* getbuf() const;
  void clear();
  void assign(size_t len_, char* buf_);
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;
  size_type size() const;
  bool empty() const;
  size_type capacity() const;
  value_type operator[](size_type n_) const;
  reference at(size_type n_);
  reference front();
  const_reference front() const;
  reference back();
  const_reference back() const;
  void push_back(const value_type& val_);
  void pop_back();
  iterator insert(iterator position_, const value_type& val_);
  void insert(iterator position_, size_type n_, const value_type& val_);
  iterator erase(iterator position_);
  iterator erase(iterator first_, iterator last_);
  void swap(EnChar& other_);
};

bool operator==(const EnChar& lhs, const EnChar& rhs);
bool operator<(const EnChar& lhs, const EnChar& rhs);
bool operator<=(const EnChar& lhs, const EnChar& rhs);
bool operator>(const EnChar& lhs, const EnChar& rhs);
bool operator>=(const EnChar& lhs, const EnChar& rhs);
void swap(EnChar& x_, EnChar& y_);

#endif
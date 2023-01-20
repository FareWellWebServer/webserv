/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enChar.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haryu <haryu@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/21 00:12:08 by haryu             #+#    #+#             */
/*   Updated: 2023/01/21 00:44:50 by haryu            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EnChar.hpp"

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

EnChar::EnChar(size_t len_, char* buf_) {
  for (int i = 0; i < len_; i += 2) {
    entity_.push_back(buf_[i]);
    if (i + 1 == len_) break;
    entity_.push_back(buf_[i + 1]);
  }
}

EnChar::EnChar(const EnChar& target) { entity_ = target.entity_; }

EnChar::~EnChar(void) { entity_.clear(); }

EnChar& EnChar::operator=(const EnChar& target) {
  entity_ = target.entity_;
  return *this;
}

value_type* EnChar::getbuf() const {
  size_t len = this->size();
  char* temp = new char[len];

  for (int i = 0; i != this->size(); i += 2) {
    temp[i] = entity_[i];
    if (i + 1 == this->size()) break;
    temp[i + 1] = entity_[i + 1];
  }
  return (temp);
}
void EnChar::clear() { entity_.clear(); }
// void EnChar::assign(size_t len_, char* buf_) {}
iterator EnChar::begin() { return entity_.begin(); }
const_iterator EnChar::begin() const { return entity_.begin(); }
iterator EnChar::end() { return entity_.end(); }
const_iterator EnChar::end() const { return entity_.end(); }
reverse_iterator EnChar::rbegin() { return entity_.rbegin(); }
const_reverse_iterator EnChar::rbegin() const { return entity_.rbegin(); }
reverse_iterator EnChar::rend() { return entity_.rend(); }
const_reverse_iterator EnChar::rend() const { return entity_.rend(); }
size_type EnChar::size() const { return entity_.size(); }
bool EnChar::empty() const { return entity_.empty(); }
size_type EnChar::capacity() const { return entity_, capacity(); }
value_type EnChar::operator[](size_type n_) const { return entity_[n_]; }
reference EnChar::at(size_type n_) { return entity_.at(n_); }
reference EnChar::front() { return entity_.front(); }
const_reference EnChar::front() const { return entity_.front(); }
reference EnChar::back() { return entity_.back(); }
const_reference EnChar::back() const { return entity_.back(); }
void EnChar::push_back(const value_type& val_) { entity_.push_back(val_); }
void EnChar::pop_back() { entity_.resize(entity_.size() - 1); }
// iterator EnChar::insert(iterator position_, const value_type& val_) {}
// void EnChar::insert(iterator position_, size_type n_, const value_type& val_)
// {} iterator EnChar::erase(iterator position_) {} iterator
// EnChar::erase(iterator first_, iterator last_) {} void EnChar::swap(EnChar&
// other_) {}
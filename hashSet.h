#ifndef HASH_SET_H
#define HASH_SET_H
#include <cmath>
#include <iterator>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "HashIterator.h"

template <class T>
struct FwdListNode;

template
<
  class Key,
  class Hash = std::hash<Key>,
  class KeyEqual = std::equal_to<Key>
>
class HashSet
{
public:
  using iterator = HashIterator<Key>;
  using const_iterator = HashIterator<Key>;
  using node_t = FwdListNode<Key>;
  using this_t = HashSet;

  explicit HashSet(std::size_t capacity = 100);
  ~HashSet();
  HashSet(const this_t& rhs);
  this_t& operator=(const this_t& rhs);
  HashSet(this_t&& rhs) noexcept;
  this_t& operator=(this_t&& rhs) noexcept;
  std::size_t size() const noexcept;
  double loadFactor() const noexcept;
  void rehash();
  void reserve(std::size_t capacity);
  bool insert(const Key& key);
  bool remove(const Key& key);
  iterator find(const Key& key) const;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  iterator begin() const noexcept;
  iterator end() const noexcept;
  void clear() noexcept;

private:
  std::size_t size_{ 0 };
  std::size_t bucket_count_;
  node_t** set_;
  static constexpr double MAX_LOAD_FACTOR{ 0.7 };
  static constexpr double EXPANSION_COEFFICIENT{ 2.0 };

  void swap(this_t& rhs) noexcept;
  std::size_t hash(const Key& key) const;
  void copyFrom(const this_t& source, std::size_t newSize_);
  void removeContainer() noexcept;
};

template <class T>
struct FwdListNode
{
  T data_;
  FwdListNode* next_;
  FwdListNode(const T& data, FwdListNode* next = nullptr) :
    data_(data),
    next_(next)
  {}
};

template <class Key, class Hash, class KeyEqual>
std::size_t HashSet<Key, Hash, KeyEqual>::size() const noexcept
{
  return size_;
}

template <class Key, class Hash, class KeyEqual>
HashSet<Key, Hash, KeyEqual>::HashSet(std::size_t capacity)
{
  if (capacity == 0)
  {
    throw std::invalid_argument("Invalid capacyty_");
  }
  bucket_count_ = static_cast<std::size_t>(capacity / MAX_LOAD_FACTOR) + 1;
  set_ = new node_t*[bucket_count_] {};  
}

template <class Key, class Hash, class KeyEqual>
void HashSet<Key, Hash, KeyEqual>::clear() noexcept
{
  for (std::size_t i = 0; i < bucket_count_; ++i)
  {
    node_t* cur = set_[i];
    while (cur)
    {
      node_t* next = cur->next_;
      delete cur;
      cur = next;
    }
    set_[i] = nullptr;
  }
  size_ = 0;
}

template <class Key, class Hash, class KeyEqual>
void HashSet<Key, Hash, KeyEqual>::removeContainer() noexcept
{
  clear();
  delete[] set_;
  set_ = nullptr;
  bucket_count_ = 0;
}

template <class Key, class Hash, class KeyEqual>
HashSet<Key, Hash, KeyEqual>::~HashSet()
{
  removeContainer();
}

template <class Key, class Hash, class KeyEqual>
HashSet<Key, Hash, KeyEqual>::HashSet(this_t&& rhs) noexcept:
  size_(rhs.size_),
  bucket_count_(rhs.bucket_count_),
  set_(rhs.set_)
{
  rhs.set_ = nullptr;
  rhs.bucket_count_ = 0;
  rhs.size_ = 0;
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::operator=(this_t&& rhs) noexcept -> this_t&
{
  if (this != &rhs)
  {
    this_t tmp(std::move(rhs));
    removeContainer();
    swap(tmp);    
  }
  return (*this);
}

template <class Key, class Hash, class KeyEqual>
void HashSet<Key, Hash, KeyEqual>::copyFrom(const this_t& source, std::size_t newSize_)
{
  this_t tmp(newSize_);
  for (const auto& x: source)
  {
    auto bucket = tmp.hash(x);
    tmp.set_[bucket] = new node_t(x, tmp.set_[bucket]);
    ++tmp.size_;
  }
  (*this) = std::move(tmp);
}

template <class Key, class Hash, class KeyEqual>
HashSet<Key, Hash, KeyEqual>::HashSet(const this_t& rhs):
  set_(nullptr),
  bucket_count_(0)
{
  copyFrom(rhs, rhs.size_);
}

template <class Key, class Hash, class KeyEqual>
void HashSet<Key, Hash, KeyEqual>::swap(this_t& rhs) noexcept
{
  std::swap(size_, rhs.size_);
  std::swap(bucket_count_, rhs.bucket_count_);
  std::swap(set_, rhs.set_);
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::operator=(const this_t& rhs) -> this_t&
{
  if (this != &rhs)
  {
    removeContainer();
    copyFrom(rhs, rhs.size_);
  }
  return *this;
}

template <class Key, class Hash, class KeyEqual>
double HashSet<Key, Hash, KeyEqual>::loadFactor() const noexcept
{
  assert(bucket_count_ != 0);
  return static_cast<double>(size_) / bucket_count_;
}

template <class Key, class Hash, class KeyEqual>
void HashSet<Key, Hash, KeyEqual>::rehash()
{
  copyFrom(*this, static_cast<std::size_t>(bucket_count_ * MAX_LOAD_FACTOR));
}

template <class Key, class Hash, class KeyEqual>
void HashSet<Key, Hash, KeyEqual>::reserve(std::size_t capacity)
{
  if (capacity > size_)
  {
    copyFrom(*this, capacity);
  }
}

template <class Key, class Hash, class KeyEqual>
bool HashSet<Key, Hash, KeyEqual>::insert(const Key& key)
{ 
  auto bucket = hash(key);
  auto current = set_[bucket];
  KeyEqual keyEqual;
  while (current && !keyEqual(current->data_, key))
  {
    current = current->next_;
  }
  if (current)
  {
    return false;
  }
  if (loadFactor() >= MAX_LOAD_FACTOR)
  {
    copyFrom(*this, size_ * static_cast<std::size_t>(EXPANSION_COEFFICIENT));
    bucket = hash(key);
  }
  set_[bucket] = new node_t(key, set_[bucket]);
  ++size_;
  return true;
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::cbegin() const noexcept -> const_iterator
{
  auto firstNode = set_;
  while ((firstNode != set_ + bucket_count_) && !(*firstNode))
  {
    ++firstNode;
  }
  if (firstNode == set_ + bucket_count_)
  {
    return const_iterator(firstNode, nullptr, set_ + bucket_count_);
  }
  return const_iterator(firstNode, *firstNode, set_ + bucket_count_);
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::cend() const noexcept -> const_iterator
{
  return const_iterator(set_ + bucket_count_, nullptr, set_ + bucket_count_);
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::begin() const noexcept -> iterator
{
  return cbegin();
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::end() const noexcept -> iterator
{
  return cend();
}

template <class Key, class Hash, class KeyEqual>
auto HashSet<Key, Hash, KeyEqual>::find(const Key& key)  const -> iterator
{
  auto bucket = hash(key);
  auto current = set_[bucket];
  KeyEqual keyEqual;
  while (current && !keyEqual(current->data_, key))
  {
    current = current->next_;
  }
  if (current)
  {
    return iterator(set_ + bucket, current, set_ + bucket_count_);
  }
  return end();
}

template <class Key, class Hash, class KeyEqual>
std::size_t HashSet<Key, Hash, KeyEqual>::hash(const Key& key) const
{
  return Hash{}(key) % bucket_count_;
}

template <class Key, class Hash, class KeyEqual>
bool HashSet<Key, Hash, KeyEqual>::remove(const Key& key)
{
  auto bucket = hash(key);
  auto current = set_[bucket];
  if (current && KeyEqual{}(current->data_, key))
  {
    set_[bucket] = current->next_;
    delete current;
    --size_;
    return true;
  }
  while (current && current->data_ != key)
  {
    auto next = current->next_;
    if (next && next->data_ == key)
    {
      current->next_ = next->next_;
      delete next;
      --size_;
      return true;
    }
    current = next;
  }
  return false;
}
#endif

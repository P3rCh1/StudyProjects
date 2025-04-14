#ifndef HASH_SET_H
#define HASH_SET_H
#include <cmath>
#include <iostream>
template <class Data>
class HashSet
{
public:
  HashSet(std::size_t capacity = 100);
  ~HashSet();
  HashSet(const HashSet&) = delete;
  HashSet& operator=(const HashSet&) = delete;
  HashSet(HashSet&& rhs) noexcept;
  HashSet& operator=(HashSet&& rhs) noexcept;
  bool insert(const Data& key);
  bool remove(const Data& key);
  void print(Data key, std::ostream& out = std::cout) const;
  void print(std::ostream& out = std::cout) const;

private:
  struct Line
  {
    Data key_;
    bool occupied_{ false };
    bool deleted_{ false };
    bool isExpectedPlace(Data key) const;
    bool isIntermediate() const;
  };

  std::size_t size_;
  std::size_t capacity_;
  Line* map_;
  static constexpr double MAX_OCCUPANCY = 0.7;

  static void normalize(Data& key);
  std::size_t hash(Data key) const;

  std::size_t getPlacement(const Data& key, std::size_t& i) const;
  std::size_t search(const Data& key, std::size_t start, std::size_t i = 1) const;
};

template <class Data>
HashSet<Data>::HashSet(std::size_t capacity) :
  capacity_(capacity),
  map_(new HashSet::Line[capacity]),
  size_(0)
{}

template <class Data>
HashSet<Data>::~HashSet()
{
  delete[] map_;
}

template <class Data>
HashSet<Data>::HashSet(HashSet<Data>&& rhs) noexcept :
  capacity_(rhs.capacity_),
  map_(rhs.map_),
  size_(rhs.size_)
{
  rhs.map_ = nullptr;
}

template <class Data>
HashSet<Data>& HashSet<Data>::operator=(HashSet<Data>&& rhs) noexcept
{
  if (this != &rhs)
  {
    delete[] map_;
    capacity_ = rhs.capacity_;
    map_ = rhs.map_;
    size_ = rhs.size_;
    rhs.map_ = nullptr;
  }
  return *this;
}

template <>
void HashSet<double>::normalize(double& key)
{
  if (key < 0)
  {
    key = -key;
  }
  while (key >= 1)
  {
    key /= 10;
  }
}

template <>
std::size_t HashSet<double>::hash(double key) const
{
  normalize(key);
  return std::floor(key * static_cast<double>(capacity_));
}

template <class Data>
bool HashSet<Data>::Line::isIntermediate() const
{
  return occupied_ || deleted_;
}

template <class Data>
bool HashSet<Data>::Line::isExpectedPlace(Data key) const
{
  return (!occupied_) || (key_ == key);
}

template <class Data>
std::size_t HashSet<Data>::getPlacement(const Data& key, std::size_t& i) const
{
  auto place = hash(key);
  for (i; !map_[place].isExpectedPlace(key); ++i)
  {
    place = (place + i * i) % capacity_;
  }
  return place;
}

template <class Data>
std::size_t HashSet<Data>::search(const Data& key, std::size_t start, std::size_t i) const
{
  for (i; map_[start].isIntermediate(); ++i)
  {
    if (!map_[start].deleted_ && map_[start].key_ == key)
    {
      return start;
    }
    start = (start + i * i) % capacity_;
  }
  return capacity_;
}

template <class Data>
bool HashSet<Data>::insert(const Data& key)
{
  if ((size_ / capacity_) >= HashSet::MAX_OCCUPANCY)
  {
    throw std::overflow_error("Max occupancy has been exceeded");
  }
  std::size_t i = 1;
  auto place = getPlacement(key, i);
  if (search(key, place, i) != capacity_)
  {
    return false;
  }
  map_[place].key_ = key;
  map_[place].occupied_ = true;
  map_[place].deleted_ = false;
  ++size_;
  return true;
}

template <class Data>
bool HashSet<Data>::remove(const Data& key)
{
  auto place = search(key, hash(key));
  if (place == capacity_)
  {
    return false;
  }
  map_[place].deleted_ = true;
  map_[place].occupied_ = false;
  --size_;
  return true;
}

template <class Data>
void HashSet<Data>::print(Data key, std::ostream& out) const
{
  auto place = hash(key);
  for (std::size_t i = 1; map_[place].isIntermediate(); ++i)
  {
    if (hash(map_[place].key_) == hash(key))
    {
      out << map_[place].key_ << ' ';
    }
    place = (place + i * i) % capacity_;
  }
}

template <class Data>
void HashSet<Data>::print(std::ostream& out) const
{
  for (std::size_t i = 0; i < capacity_; i++)
  {
    if (map_[i].occupied_)
    {
      out << map_[i].key_ << ' ';
    }
  }
}
#endif

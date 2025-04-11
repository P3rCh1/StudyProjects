#include "hashSet.h"

HashSet::HashSet(std::size_t capacity) :
  capacity_(capacity),
  map_(new HashSet::Line[capacity]),
  size_(0)
{}

HashSet::~HashSet()
{
  delete[] map_;
}

HashSet::HashSet(HashSet&& rhs) noexcept :
  capacity_(rhs.capacity_),
  map_(rhs.map_),
  size_(rhs.size_)
{
  rhs.map_ = nullptr;
}

HashSet& HashSet::operator=(HashSet&& rhs) noexcept
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

void HashSet::normalize(double& key) const
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

std::size_t HashSet::hash(double key) const
{
  normalize(key);
  return std::floor(key * static_cast<double>(capacity_));
}

bool HashSet::Line::isIntermediate() const
{
  return occupied_ || deleted_;
}

bool HashSet::Line::isExpectedPlace(double key) const
{
  return (!occupied_) || (key_ == key);
}

std::size_t HashSet::getPlacement(const double& key, std::size_t& i) const
{
  auto place = hash(key);
  for (i; !map_[place].isExpectedPlace(key); ++i)
  {
    place = (place + i * i) % capacity_;
  }
  return place;
}

std::size_t HashSet::search(const double& key, std::size_t start, std::size_t i) const
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

bool HashSet::insert(const double& key)
{
  if ((size_ / capacity_) >= HashSet::MAX_OCCUPANCY)
  {
    throw std::overflow_error("Max occupancy has been exceeded");
  }
  std::size_t i = 1;
  auto place = getPlacement(key,i);
  if (search(key,place,i) != capacity_)
  {
    return false;
  }
  map_[place].key_ = key;
  map_[place].occupied_ = true;
  map_[place].deleted_ = false;
  ++size_;
  return true;
}

bool HashSet::remove(const double& key)
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

void HashSet::print(double key, std::ostream& out) const
{
  auto place = hash(key);
  for (auto i = 1; map_[place].isIntermediate(); ++i)
  {
    if (hash(map_[place].key_) == hash(key))
    {
      out << map_[place].key_ << ' ';
    }
    place = (place + i * i) % capacity_;
  }
}

void HashSet::print(std::ostream& out) const
{
  for (std::size_t i = 0; i < capacity_; i++)
  {
    if (map_[i].occupied_)
    {
      out << map_[i].key_ << ' ';
    }
  }
}
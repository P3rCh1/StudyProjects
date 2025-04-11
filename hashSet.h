#ifndef HASH_SET_H
#define HASH_SET_H
#include <cmath>
#include <iostream>
class HashSet
{
public:
  HashSet(std::size_t capacity = 100);
  ~HashSet();
  HashSet(const HashSet&) = delete;
  HashSet& operator=(const HashSet&) = delete;
  HashSet(HashSet&& rhs) noexcept;
  HashSet& operator=(HashSet&& rhs) noexcept;
  bool insert(const double& key);
  bool remove(const double& key);
  void print(double key, std::ostream& out = std::cout) const;
  void print(std::ostream& out = std::cout) const;

private:
  struct Line
  {
    double key_;
    bool occupied_{ false };
    bool deleted_{ false };
    bool isExpectedPlace(double key) const;
    bool isIntermediate() const;
  };

  std::size_t size_;
  std::size_t capacity_;
  Line* map_;
  static constexpr double MAX_OCCUPANCY = 0.7;
  void normalize(double& key) const;
  std::size_t hash(double key) const;
  std::size_t getPlacement(const double& key, std::size_t& i) const;
  std::size_t search(const double& key, std::size_t start, std::size_t i = 1) const;
};
#endif

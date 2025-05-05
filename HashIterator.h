#ifndef HASH_ITERATOR_H
#define HASH_ITERATOR_H
#include <iterator>
#include <cassert>
template <class T>
struct FwdListNode;

template <class T>
class HashIterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = const T*;
  using reference = const T&;
  using node_t = FwdListNode<T>;
  using this_t = HashIterator;

  HashIterator() noexcept;
  explicit HashIterator
  (
    const node_t* const* currentBucket,
    const node_t* current,
    const node_t* const* end
  ) noexcept;
  HashIterator(const this_t& rhs) noexcept = default;
  this_t& operator=(const this_t& rhs) noexcept = default;
  this_t& operator++();
  this_t operator++(int);
  reference operator*() const;
  pointer operator->() const;
  bool operator==(const this_t& rhs) const noexcept;
  bool operator!=(const this_t& rhs) const noexcept;

private:
  const node_t* const* currentBucket_;
  const node_t* current_;
  const node_t* const* end_;
};

template <class T>
HashIterator<T>::HashIterator() noexcept:
  currentBucket_(nullptr),
  current_(nullptr),
  end_(nullptr)
{}

template <class T>
HashIterator<T>::HashIterator
(
  const node_t* const* currentBucket,
  const node_t* current,
  const node_t* const* end
) noexcept:
  currentBucket_(currentBucket),
  current_(current),
  end_(end)
{}

template <class T>
auto HashIterator<T>::operator++() -> this_t&
{
  assert(current_);
  if (current_->next_)
  {
    current_ = current_->next_;
    return *this;
  }
  ++currentBucket_;
  while ((currentBucket_ != end_) && (*currentBucket_ == nullptr))
  {
    ++currentBucket_;
  }
  if (currentBucket_ != end_)
  {
    current_ = *currentBucket_;
  }
  else
  {
    current_ = nullptr;
  }
  return *this;
}

template <class T>
auto HashIterator<T>::operator++(int) -> this_t
{
  this_t tmp(*this);
  ++(*this);
  return tmp;
}

template <class T>
auto HashIterator<T>::operator*() const -> reference
{
  assert(current_);
  return current_->data_;
}

template <class T>
auto HashIterator<T>::operator->() const -> pointer
{
  assert(current_);
  return &(current_->data_);
}

template <class T>
bool HashIterator<T>::operator==(const this_t& rhs) const noexcept
{
  return current_ == rhs.current_;
}

template <class T>
bool HashIterator<T>::operator!=(const this_t& rhs) const noexcept
{
  return current_ != rhs.current_;
}
#endif

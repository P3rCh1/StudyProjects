#ifndef QUEUE_H
#define QUEUE_H
#include <string>
#include <algorithm>
template <class Data>
class Queue
{
public:
	virtual ~Queue() = default;
	virtual void enQueue(const Data& data) = 0; 
	virtual Data deQueue() = 0; 
	virtual bool isEmpty() = 0;
};

class QueueOverflow : public std::exception
{
public:
	QueueOverflow(std::string reason = "Queue overflow") :
		reason_(reason)
	{}

	const std::string& what()
	{
		return reason_;
	}
private:
	std::string reason_;
};

class QueueUnderflow : public std::exception
{
public:
	QueueUnderflow(std::string reason = "Queue underflow") :
		reason_(reason)
	{}

	const std::string& what()
	{
		return reason_;
	}
private:
	std::string reason_;
};

class WrongQueueSize : public std::exception
{
public:
	WrongQueueSize(std::string reason = "Wrong queue size") :
		reason_(reason)
	{}

	const std::string& what()
	{
		return reason_;
	}
private:
	std::string reason_;
};

template <class Data>
class QueueArray : public Queue<Data>
{
public:
	QueueArray(size_t size = 100) :
		size_(size),
		head_(0, size),
		tail_(0, size),
		isEmpty_(true)
	{
		if (size == 0)
		{
			throw WrongQueueSize();
		}
		array_ = new Data[size_];
	}

	QueueArray(const QueueArray& other)
	{
		assignZero();
		QueueArray temp(other.size_);
		temp.head_ = other.head_;
		temp.tail_ = other.tail_;
		temp.isEmpty_ = other.isEmpty_;
		if (!temp.isEmpty_)
		{
			for (RingIterator i = temp.head_; ; i.next())
			{
				temp.array_[i.iterator_] = other.array_[i.iterator_];
				if (i.showNext() == temp.tail_.iterator_)
				{
					break;
				}
			}
		}
		swap(temp);
	}

	QueueArray(QueueArray&& other)
	{
		assignOther(other);
		other.assignZero();
	}

	QueueArray& operator=(const QueueArray& other)
	{
		if (this != &other)
		{
			QueueArray temp(other);
			swap(temp);
		}
		return *this;
	}

	QueueArray& operator=(QueueArray&& other)
	{
		if (this != &other)
		{
			QueueArray temp(std::move(other));
			swap(temp);
		}
		return *this;
	}

	~QueueArray()
	{
		delete[] array_;
	}

	void enQueue(const Data& data);

	Data deQueue();

	bool isEmpty();

private:
	struct RingIterator
	{
		size_t iterator_;
		size_t size_;
		RingIterator(size_t iterator = 0, size_t size = 0) :
			iterator_(iterator),
			size_(size)
		{}
		void next()
		{
			iterator_ = showNext();
		}

		size_t showNext()
		{
			return (iterator_ + 1) % size_;
		}
	};

	Data* array_;
	RingIterator head_;
	RingIterator tail_;
	size_t size_;
	bool isEmpty_;

	void swap(QueueArray& other);
	void assignZero();
	void assignOther(const QueueArray& other);
};

template <class Data>
void QueueArray<Data>::swap(QueueArray<Data>& other)
{
	std::swap(array_, other.array_);
	std::swap(head_.iterator_, other.head_.iterator_);
	std::swap(head_.size_, other.head_.size_);
	std::swap(tail_.iterator_, other.tail_.iterator_);
	std::swap(tail_.size_, other.tail_.size_);
	std::swap(size_, other.size_);
	std::swap(isEmpty_, other.isEmpty_);
}

template <class Data>
void QueueArray<Data>::assignZero()
{
	size_ = 0;
	array_ = nullptr;
	head_ = RingIterator(0, 0);
	tail_ = RingIterator(0, 0);
	isEmpty_ = true;
}

template <class Data>
void QueueArray<Data>::assignOther(const QueueArray<Data>& other)
{
	size_ = other.size_;
	array_ = other.array_;
	head_ = other.head_;
	tail_ = other.tail_;
	isEmpty_ = other.isEmpty_;
}

template <class Data>
void QueueArray<Data>::enQueue(const Data& data)
{
	if (head_.iterator_ == tail_.iterator_ && !isEmpty_)
	{
		throw QueueOverflow();
	}
	array_[tail_.iterator_] = data;
	tail_.next();
	isEmpty_ = false;
}

template <class Data>
Data QueueArray<Data>::deQueue()
{
	if (isEmpty_)
	{
		throw QueueUnderflow();
	}
	Data temp = array_[head_.iterator_];
	head_.next();
	if (head_.iterator_ == tail_.iterator_)
	{
		isEmpty_ = 1;
	}
	return temp;
}

template <class Data>
bool QueueArray<Data>::isEmpty()
{
	return isEmpty_;
}
#endif
#ifndef STACK_H
#define STACK_H
#include <string>
#include <algorithm>
template <class Data>
class Stack
{
public:
	virtual ~Stack() = default;
	virtual void push(const Data& data) = 0;
	virtual Data pop() = 0;
	virtual bool isEmpty() = 0;
};

class StackUnderflow : public std::exception
{
public:
	StackUnderflow(const std::string& reason = "Stack Overflow") :
		reason_(reason)
	{}

	const std::string& what()
	{
		return reason_;
	}
private:
	const std::string reason_;
};

template <class Data>
class StackList : public Stack<Data>
{
public:

	StackList() :
		head_(nullptr)
	{}

	StackList(const StackList& other) :
		head_(nullptr)
	{
		StackList temp;
		Node* currentOther = other.head_;
		if (currentOther)
		{
			temp.head_ = new Node(currentOther->data_);
			currentOther = currentOther->next_;
		}
		Node* currentTemp = temp.head_;
		while (currentOther)
		{
			currentTemp->next_ = new Node(currentOther->data_);
			currentOther = currentOther->next_;
			currentTemp = currentTemp->next_;
		}
		swap(temp);
	}

	StackList& operator=(const StackList& other)
	{
		if (this->head_ == other.head_)
		{
			return *this;
		}
		StackList temp(other);
		swap(temp);
		return *this;
	}

	StackList(StackList&& other) :
		head_(other.head_)
	{
		other.head_ = nullptr;
	}

	StackList& operator=(StackList&& other)
	{
		if (this->head_ == other.head_)
		{
			return *this;
		}
		clear();
		head_ = other.head_;
		other.head_ = nullptr;
		return *this;
	}

	~StackList()
	{
		clear();
	}

	void swap(StackList& other) noexcept;

	void clear();

	void push(const Data& data);

	Data pop();

	bool isEmpty();

private:
	struct Node
	{
		Data data_;
		Node* next_;
		Node(const Data& data, Node* next = nullptr) :
			data_(data),
			next_(next)
		{}
	};
	Node* head_;
};

template <class Data>
void StackList<Data>::swap(StackList<Data>& other) noexcept
{
	std::swap(this->head_, other.head_);
}

template <class Data>
void StackList<Data>::clear()
{
	while (head_)
	{
		pop();
	}
}

template <class Data>
void StackList<Data>::push(const Data& data)
{
	head_ = new Node(data, head_);
}

template <class Data>
Data StackList<Data>::pop()
{
	if (!head_) {
		throw StackUnderflow();
	}
	typename StackList<Data>::Node* temp = head_;
	head_ = head_->next_;
	Data tempData = std::move(temp->data_);
	delete temp;
	return tempData;
}

template <class Data>
bool StackList<Data>::isEmpty()
{
	return head_ == nullptr;
}

#endif

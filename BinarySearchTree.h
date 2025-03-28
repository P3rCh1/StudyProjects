#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H
#include <algorithm>
#include <iostream>
#include "Queue.h"
template <class Data>
class BinarySearchTree
{
public:
	BinarySearchTree();
	BinarySearchTree(BinarySearchTree<Data>&& rhs) noexcept;
	BinarySearchTree<Data>& operator=(BinarySearchTree<Data>&& rhs) noexcept;
	virtual ~BinarySearchTree();
	BinarySearchTree(const BinarySearchTree<Data>&) = delete;
	BinarySearchTree<Data>& operator=(const BinarySearchTree<Data>&) = delete;

	bool searchIterative(const Data& data) const;
	bool insert(const Data& data);
	bool remove(const Data& data);
	void output(std::ostream& out) const;
	int getNumberOfNodes() const;
	int getHeight() const;
	template<class Operation>
	void inorderWalkIterative(const Operation& operation) const;
	template<class Operation>
	void inorderWalk(const Operation& operation) const;
	template<class Operation>
	void walkByLevels(const Operation& operation) const;
	size_t countNodesBetween(const Data& low, const Data& high) const;
	void clear();

private:
	struct Node
	{
		Data data_;
		Node* left_;
		Node* right_;
		Node* p_;
		Node(Data data, Node* p = nullptr) :
			data_(data),
			p_(p),
			left_(nullptr),
			right_(nullptr)
		{}
	};

	Node* root_;

	Node* nextSearchNode(Node* current, const Data& data) const;
	Node* searchNodeIterative(const Data& data) const;
	void clear(Node* node);
	Node* searchExpectedParent(const Data& data, Node* root) const;
	Node* max(Node* root) const;
	Node* min(Node* root) const;
	Node* getSuccessor(Node* root);
	void removeSuccessorSourceNode(Node* leaf);
	bool isLeaf(Node* node) const;
	void printLower(std::ostream& out, Node* root) const;
	void output(std::ostream& out, Node* root) const;
	int getNumberOfNodes(const Node* node) const;
	int getHeight(const Node* node) const;
	template<class Operation>
	void inorderWalk(Node* node, const Operation& operation) const;
	Node* getNext(Node* current) const;
	Node* searchNextLower(Node* current) const;
	Node* searchNextHigher(Node* current) const;
	Node* searchFirstNotLess(const Data& data) const;
	size_t pow2(size_t degree) const;
	template<class Operation>
	void processingWalkByLevelsNode(QueueArray<Node*>& queue, const Operation& operation) const;
};

template <class Data>
void BinarySearchTree<Data>::clear(Node* node)
{
	if (!node)
	{
		return;
	}
	clear(node->left_);
	clear(node->right_);
	delete node;
}

template <class Data>
void BinarySearchTree<Data>::clear()
{
	clear(root_);
	root_ = nullptr;
}

template <class Data>
BinarySearchTree<Data>::BinarySearchTree() :
	root_(nullptr)
{}

template <class Data>
BinarySearchTree<Data>::BinarySearchTree(BinarySearchTree&& rhs) noexcept :
	root_(rhs.root_)
{
	rhs.root_ = nullptr;
}

template <class Data>
BinarySearchTree<Data>& BinarySearchTree<Data>::operator=(BinarySearchTree<Data>&& rhs) noexcept
{
	clear();
	root_ = rhs.root_;
	rhs.root_ = nullptr;
	return *this;
}

template <class Data>
BinarySearchTree<Data>::~BinarySearchTree()
{
	clear();
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::searchNodeIterative(const Data& data) const
{
	Node* current = root_;
	while (current && current->data_ != data)
	{
		current = nextSearchNode(current, data);
	}
	return current;
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::nextSearchNode(Node* current, const Data& data) const
{
	if (current->data_ < data)
	{
		return current->right_;
	}
	else
	{
		return current->left_;
	}
}

template <class Data>
bool BinarySearchTree<Data>::searchIterative(const Data& data) const
{
	return searchNodeIterative(data);
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::searchExpectedParent(const Data& data, Node* current) const
{
	Node* expectedParrent = nullptr;
	while (current && current->data_ != data) {
		expectedParrent = current;
		current = nextSearchNode(current, data);
	}
	if (current)
	{
		return nullptr;
	}
	return expectedParrent;
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::max(Node* root) const
{
	if (root)
	{
		while (root->right_)
		{
			root = root->right_;
		}
	}
	return root;
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::min(Node* root) const
{
	if (root)
	{
		while (root->left_)
		{
			root = root->left_;
		}
	}
	return root;
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::getSuccessor(Node* root)
{
	if (root->left_)
	{
		return max(root->left_);
	}
	if (root->right_)
	{
		return min(root->right_);
	}
	return root;
}

template <class Data>
bool BinarySearchTree<Data>::insert(const Data& data)
{
	if (!root_)
	{
		root_ = new Node(data);
		return true;
	}
	Node* expectedParrent = searchExpectedParent(data, root_);
	if (!expectedParrent)
	{
		return false;
	}
	if (data < expectedParrent->data_)
	{
		expectedParrent->left_ = new Node(data, expectedParrent);
	}
	else
	{
		expectedParrent->right_ = new Node(data, expectedParrent);
	}
	return true;
}

template <class Data>
void BinarySearchTree<Data>::removeSuccessorSourceNode(Node* source)
{
	if (!source->p_)
	{
		delete source;
		root_ = nullptr;
		return;
	}
	Node* sourceChild;
	if (source->left_)
	{
		sourceChild = source->left_;
	}
	else
	{
		sourceChild = source->right_;
	}
	if (source->p_->left_ == source)
	{
		source->p_->left_ = sourceChild;
	}
	else
	{
		source->p_->right_ = sourceChild;
	}
	if (sourceChild)
	{
		sourceChild->p_ = source->p_;
	}
	delete source;
}

template <class Data>
bool BinarySearchTree<Data>::remove(const Data& data)
{
	Node* expected = searchNodeIterative(data);
	if (!expected)
	{
		return false;
	}
	Node* forDelete = getSuccessor(expected);
	expected->data_ = forDelete->data_;
	removeSuccessorSourceNode(forDelete);
	return true;
}

template <class Data>
bool BinarySearchTree<Data>::isLeaf(Node* node) const
{
	return !(node->left_ || node->right_);
}

template <class Data>
void BinarySearchTree<Data>::printLower(std::ostream& out, Node* root) const
{
	if (isLeaf(root))
	{
		return;
	}
	out << '(';
	output(out, root->left_);
	out << ")(";
	output(out, root->right_);
	out << ')';
}

template <class Data>
void BinarySearchTree<Data>::output(std::ostream& out, Node* root) const
{
	if (!root)
	{
		return;
	}
	out << root->data_;
	printLower(out, root);
}

template <class Data>
void BinarySearchTree<Data>::output(std::ostream& out) const
{
	out << '(';
	output(out, root_);
	out << ')';
}

template <class Data>
int BinarySearchTree<Data>::getNumberOfNodes(const Node* node) const
{
	if (!node)
	{
		return 0;
	}
	return 1 + getNumberOfNodes(node->left_) + getNumberOfNodes(node->right_);
}

template <class Data>
int BinarySearchTree<Data>::getNumberOfNodes() const
{
	return getNumberOfNodes(root_);
}

template <class Data>
int BinarySearchTree<Data>::getHeight(const Node* node) const
{
	if (!node)
	{
		return 0;
	}
	return 1 + std::max(getHeight(node->left_), getHeight(node->right_));
}

template <class Data>
int BinarySearchTree<Data>::getHeight() const
{
	if (!root_)
	{
		return 0;
	}
	return getHeight(root_) - 1;
}

template <class Data>
template <class Operation>
void BinarySearchTree<Data>::inorderWalk(Node* node, const Operation& operation) const
{
	if (node)
	{
		inorderWalk(node->left_, operation);
		operation(node->data_);
		inorderWalk(node->right_, operation);
	}
}

template <class Data>
template <class Operation>
void BinarySearchTree<Data>::inorderWalk(const Operation& operation) const
{
	inorderWalk(root_, operation);
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::searchNextLower(Node* current) const
{
	if (!current || !current->right_)
	{
		return nullptr;
	}
	return min(current->right_);
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::searchNextHigher(Node* current) const
{
	if (!current)
	{
		return nullptr;
	}
	while (current->p_ && (current->p_->right_ == current))
	{
		current = current->p_;
	}
	if (!current->p_)
	{
		return nullptr;
	}
	return current->p_;
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::getNext(Node* current) const
{
	Node* expectedNext = searchNextLower(current);
	if (!expectedNext)
	{
		expectedNext = searchNextHigher(current);
	}
	return expectedNext;
}

template <class Data>
template <class Operation>
void BinarySearchTree<Data>::inorderWalkIterative(const Operation& operation) const
{
	Node* current = min(root_);
	while (current)
	{
		operation(current->data_);
		current = getNext(current);
	}
}

template <class Data>
template <class Operation>
void BinarySearchTree<Data>::processingWalkByLevelsNode(QueueArray<Node*>& queue, const Operation& operation) const
{
	Node* tmp = queue.deQueue();
	operation(tmp->data_);
	if (tmp->left_)
	{
		queue.enQueue(tmp->left_);
	}
	if (tmp->right_)
	{
		queue.enQueue(tmp->right_);
	}
}

template <class Data>
size_t BinarySearchTree<Data>::pow2(size_t degree) const
{
	size_t res = 1;
	while (degree > 0)
	{
		res *= 2;
		--degree;
	}
	return res;
}

template <class Data>
template <class Operation>
void BinarySearchTree<Data>::walkByLevels(const Operation& operation) const
{
	if (root_)
	{
		QueueArray<Node*> queue(pow2(getHeight()));
		queue.enQueue(root_);
		while (!queue.isEmpty())
		{
			processingWalkByLevelsNode(queue, operation);
		}
	}
}

template <class Data>
typename BinarySearchTree<Data>::Node* BinarySearchTree<Data>::searchFirstNotLess(const Data& data) const
{
	Node* current = root_;
	Node* previous = nullptr;
	while (current && current->data_ != data)
	{
		previous = current;
		current = nextSearchNode(current, data);
	}
	if (current)
	{
		return current;
	}
	if (previous && previous->data_ < data)
	{
		previous = getNext(previous);
	}
	return previous;
}

template <class Data>
size_t BinarySearchTree<Data>::countNodesBetween(const Data& low, const Data& high) const
{
	size_t count = 0;
	Node* current = searchFirstNotLess(low);
	while (current && current->data_ <= high)
	{
		++count;
		current = getNext(current);
	}
	return count;
}

template <class Data>
class Print
{
public:
	Print(std::ostream& out = std::cout) :
		out_(out)
	{}
	void operator()(const Data& data) const
	{
		out_ << data << ' ';
	}
private:
	std::ostream& out_;
};

#endif
#ifndef DICTIONARY_LIST
#define DICTIONARY_LIST
#include <iostream>
template <class Data>
class DictionaryList {
	struct Node {
		Node* next_;
		Node* previous_;
		Data data_;
		Node(const Data& data, Node* previous = nullptr, Node* next = nullptr) : 
			data_(data),
			previous_(previous),
			next_(next) 
		{}
		Node& operator=(const Node& node) = default;
	};
	Node* head_;
	Node* tail_;

	void pushBack(const Data& data) {
		Node* temp = new Node(data, tail_);
		if (tail_) {
			tail_->next_ = temp;
			tail_ = tail_->next_;
		}
		else {
			head_ = temp;
			tail_ = temp;
		}
	}

	void insertBefore(const Data& data, Node* currentNode) {
		Node* temp = new Node(data, currentNode->previous_, currentNode);
		currentNode->previous_ = temp;
		if (temp->previous_) {
			temp->previous_->next_ = temp;
		}
		else {
			head_ = temp;
		}
	}

	Node* SearchFirstNotLessItem(const Data& data) const {
		Node* currentNode = head_;
		while (currentNode && (currentNode->data_ < data)) {
			currentNode = currentNode->next_;
		}
		return currentNode;
	}

	void deleteItem(Node* node) {
		if (!node) {
			return;
		}
		if ((!node->previous_) && (!node->next_)) {
			head_ = nullptr;
			tail_ = nullptr;
		}
		else if (!node->previous_) {
			head_ = node->next_;
			node->next_->previous_ = nullptr;
		}
		else if (!node->next_) {
			tail_ = node->previous_;
			node->previous_->next_ = nullptr;
		}
		else {
			node->previous_->next_ = node->next_;
			node->next_->previous_ = node->previous_;
		}
		delete node;
	}

	Node* getNextIntersectionNode(Node*& first, Node*& second) {
		while (first && second) {
			if (first->data_ == second->data_){
				Node* nextNode = new Node(first->data_, tail_);
				first = first->next_;
				second = second->next_;
				return nextNode;
			}
			else if (first->data_ > second->data_) {
				second = second->next_;
			}
			else {
				first = first->next_;
			}
		}
		return nullptr;
	}

	void pushFirstIntersectionNode(Node*& first, Node*& second) {
		Node* firstNode = getNextIntersectionNode(first, second);
		head_ = firstNode;
		tail_ = firstNode;
	}

	void pushIntersectionNodes(Node* first, Node* second) {
		pushFirstIntersectionNode(first, second);
		Node* nextNode = getNextIntersectionNode(first, second);
		while (nextNode) {
			tail_->next_ = nextNode;
			tail_ = tail_->next_;
			nextNode = getNextIntersectionNode(first, second);
		}
	}

	void compareItemsForMerge(Node*& currentThis, Node*& currentOther) {
		if (currentThis->data_ > currentOther->data_) {
			insertBefore(currentOther->data_, currentThis);
			currentOther = currentOther->next_;
		}
		else if (currentThis->data_ == currentOther->data_) {
			currentThis = currentThis->next_;
			currentOther = currentOther->next_;
		}
		else {
			currentThis = currentThis->next_;
		}
	}

	void finishCopyForMerge(Node*& currentSource) {
		while (currentSource) {
			pushBack(currentSource->data_);
			currentSource = currentSource->next_;
		}
	}

	void CompareItemsForDelete(Node*& currentThis, Node*& currentOther) {
		if (currentThis->data_ == currentOther->data_) {
			Node* temp = currentThis;
			currentThis = currentThis->next_;
			currentOther = currentOther->next_;
			deleteItem(temp);
		}
		else if (currentThis->data_ > currentOther->data_) {
			currentOther = currentOther->next_;
		}
		else {
			currentThis = currentThis->next_;
		}
	}

public:
	DictionaryList() : head_(nullptr), tail_(nullptr) {}

	~DictionaryList() {
		clear();
	}

	DictionaryList(const DictionaryList& other) {
		head_ = nullptr;
		tail_ = nullptr;
		*this = other;
	}

	DictionaryList& operator=(const DictionaryList& other) {
		DictionaryList temp;
		Node* currentNode = other.head_;
		while (currentNode) {
			temp.pushBack(currentNode->data_);
			currentNode = currentNode->next_;
		}
		std::swap(temp.head_, head_);
		std::swap(temp.tail_, tail_);
		return *this;
	}

	DictionaryList(DictionaryList&& other) noexcept {
		head_ = other.head_;
		tail_ = other.tail_;
		other.head_ = nullptr;
		other.tail_ = nullptr;
	}

	DictionaryList& operator=(DictionaryList&& other) noexcept {
		clear();
		head_ = other.head_;
		tail_ = other.tail_;
		other.head_ = nullptr;
		other.tail_ = nullptr;
	}

	bool insertItem(const Data& data) {
		Node* firstNotLessItem = SearchFirstNotLessItem(data);
		if (!firstNotLessItem) {
			pushBack(data);
			return true;
		}
		if (data < firstNotLessItem->data_) {
			insertBefore(data, firstNotLessItem);
			return true;
		}
		return false;
	}

	bool searchItem(const Data& data) const {
		Node* firstNotLessItem = SearchFirstNotLessItem(data);
		return (firstNotLessItem && firstNotLessItem->data_ == data);
	}

	bool deleteItem(const Data& data) {
		Node* node = SearchFirstNotLessItem(data);
		if ((!node) || (node->data_ != data)) {
			return false;
		}
		deleteItem(node);
		return true;
	}

	void clear() {
		while (head_) {
			Node* nextNode = head_->next_;
			delete head_;
			head_ = nextNode;
		}
		tail_ = nullptr;
	}

	friend std::ostream& operator <<(std::ostream& out, const DictionaryList& list) {
		Node* currentNode = list.head_;
		if (currentNode) {
			out << currentNode->data_;
			currentNode = currentNode->next_;
		}
		while (currentNode) {
			out << ' ' << currentNode->data_;
			currentNode = currentNode->next_;
		}
		return out;
	}

	void reversePrint(std::ostream& out) const {
		Node* currentNode = tail_;
		if (currentNode) {
			out << currentNode->data_;
			currentNode = currentNode->previous_;
		}
		while (currentNode) {
			out << ' ' << currentNode->data_;
			currentNode = currentNode->previous_;
		}
	}

	friend DictionaryList getIntersection(const DictionaryList& first, const DictionaryList& second) {
		DictionaryList intersection;
		intersection.pushIntersectionNodes(first.head_, second.head_);
		return intersection;
	}

	void merge(DictionaryList& other) {
		Node* currentThis = head_;
		Node* currentOther = other.head_;
		while (currentThis && currentOther) {
			compareItemsForMerge(currentThis, currentOther);
		}
		finishCopyForMerge(currentOther);
	}

	void deleteItems(DictionaryList& other) {
		Node* currentThis = head_;
		Node* currentOther = other.head_;
		while (currentThis && currentOther) {
			CompareItemsForDelete(currentThis, currentOther);
		}
	}
};
#endif
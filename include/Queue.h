#include <iostream>

template<typename T>
struct Node {
	Node<T>* next;
	T data;

	Node(T data) : data(data), next(nullptr) {}
};

template <typename T>
class Queue {
private:
	Node<T>* tail;
	Node<T>* head;
	size_t _size = 0;
public:
	size_t size() const;
	Queue();
	~Queue();
	void add(T data);
	T pop();
	void clear();
	void print() const;
	bool is_empty() const;
};

template<typename T>
inline size_t Queue<T>::size() const
{
	return this->_size;
}

template<typename T>
inline Queue<T>::Queue() : head(nullptr), tail(nullptr) {}

template<typename T>
inline Queue<T>::~Queue()
{
	Node<T>* temp = head;
	while (temp) {
		temp = temp->next;
		delete head;
		head = temp;
	}
}

template<typename T>
inline void Queue<T>::add(T data)
{
	this->_size++;

	Node<T>* temp = new Node<T>(data);
	if (is_empty()) {
		head = tail = temp;
	}
	else {
		tail->next = temp;
		tail = temp;
	}
}

template<typename T>
inline T Queue<T>::pop()
{
	this->_size--;

	Node<T>* temp = head;
	T data = temp->data;
	temp = temp->next;
	delete head;
	head = temp;

	return data;
}

template<typename T>
inline void Queue<T>::clear()
{
	while (this->_size != 0)
		this->pop();
}

template<typename T>
inline void Queue<T>::print() const // for refrences &
{
	Node<T>* temp = head;
	while (temp) {
		std::cout << *temp->data << " ";
		temp = temp->next;
	}
}

template<typename T>
inline bool Queue<T>::is_empty() const
{
	if (head == nullptr)
		return true;
	else
		return false;
}

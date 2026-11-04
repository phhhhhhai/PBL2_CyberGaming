#ifndef QUEUE_H
#define QUEUE_H

#include "Node.h"

template <class T>
class Queue {
private:
    Node<T>* front;
    Node<T>* rear;
    int count;

public:
    Queue() : front(nullptr), rear(nullptr), count(0) {}
    
    ~Queue() { clear(); }

    void enqueue(T data) {
        Node<T>* newNode = new Node<T>(data);
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }

    T dequeue() {
        if (isEmpty()) return nullptr; 
        Node<T>* temp = front;
        T data = temp->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        count--;
        return data;
    }

    bool isEmpty() const { return count == 0; }
    int getSize() const { return count; }
    Node<T>* getFront() const { return front; }

    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }
};

#endif
#ifndef NODE_H
#define NODE_H

template <class T>
class Node {
public:
    T data;         // Chứa dữ liệu (User*, Order*, MonAn*...)
    Node<T>* next;
    Node<T>* prev;  // Phục vụ cho Danh sách liên kết kép

    Node(T data) : data(data), next(nullptr), prev(nullptr) {}
};

#endif
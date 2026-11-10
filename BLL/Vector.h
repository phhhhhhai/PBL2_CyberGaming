#ifndef VECTOR_H
#define VECTOR_H

template <typename T>
class Vector {
private:
    T* data;
    int capacity;
    int currentSize;

    void resize() {
        capacity = (capacity == 0) ? 2 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < currentSize; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    Vector() : data(nullptr), capacity(0), currentSize(0) {}
    
    ~Vector() { delete[] data; }

    Vector(const Vector& other) {
        capacity = other.capacity;
        currentSize = other.currentSize;
        data = (capacity > 0) ? new T[capacity] : nullptr;
        for (int i = 0; i < currentSize; i++) data[i] = other.data[i];
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            currentSize = other.currentSize;
            data = (capacity > 0) ? new T[capacity] : nullptr;
            for (int i = 0; i < currentSize; i++) data[i] = other.data[i];
        }
        return *this;
    }

    void push_back(const T& val) {
        if (currentSize == capacity) resize();
        data[currentSize++] = val;
    }

    int size() const { return currentSize; }

    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    bool contains(const T& val) const {
        for (int i = 0; i < currentSize; i++) {
            if (data[i] == val) return true;
        }
        return false;
    }
};

#endif
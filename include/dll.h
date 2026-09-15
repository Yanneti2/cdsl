#include <iostream>
#include <cstddef>

#ifndef DLL_H
#define DLL_H

class DoubleLinkedList {
public:
    struct Node {
        int data;
        Node* next;
        Node* prev;

        Node(int val) : data(val), next(nullptr), prev(nullptr) {}
    };

private:
    long m_size;
    Node* m_head;
    Node* m_tail;

public:
    // creates an empty list O(1)
    DoubleLinkedList();

    // destructor frees the memory used to store the list
    ~DoubleLinkedList();

    // print the list in one line O(n)
    void print() const;

    // insert a node with data = valor at the beginning of the list O(1)
    void insert(int valor);

    // remove the first element of a list O(1)
    void eject();

    // add a node to the end of the list O(1)
    void push(int valor);

    // delete an element at the end of the list O(1)
    void pop();

    // gets the element at index and prints it O(n)

    // pode ser O(1) se  guardar o index de cada elemento na lista

    // If you want to get the element at index, and index is in the
    // first half of the list, search forward from the head. If it's
    // in the second half, search backward from the tail. This can cutW
    // your maximum search time in half O(n/2)
    void get(size_t index) const;

    // set an element of data x to data = y
    void set(int x, int y);

    // insert an element at a given position O(n)
    void insert_at(int valor, size_t index);

    // remove an element at a given position
    void remove_at(size_t index);

    // reverse the order of the dll
    void reverse();

    long size() const { return m_size; }
};

#endif

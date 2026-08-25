#include "stack.h"

// A linked list implementation of a stack

Stack::Stack() : m_top(nullptr) {}

Stack::~Stack() {
    while (m_top) {
        pop();
    }
}

// adds an element to the top of the stack
void Stack::push(int valor) {
    Node* nnode = new Node(valor);
    nnode->next = m_top;
    m_top = nnode;
}

// pops the top element of the stack
void Stack::pop() {
    if (m_top == nullptr)
        return;
    Node* aux = m_top;
    m_top = m_top->next;
    delete aux;
}

// returns the top element of the stack
Stack::Node* Stack::last() const {
    return m_top;
}

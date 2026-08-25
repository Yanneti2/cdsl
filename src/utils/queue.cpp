#include "queue.h"

// A linked list queue implementation

Queue::Queue() : m_front(nullptr), m_rear(nullptr) {}

Queue::~Queue() {
    while (m_front) {
        dequeue();
    }
}

// push an element to the end of the queue O(1)
void Queue::enqueue(int valor) {
    Node* nnode = new Node(valor);
    if (m_front == nullptr) { // empty queue
        m_front = m_rear = nnode;
    } else {
        m_rear->next = nnode;
        m_rear = nnode;
    }
}

// removes the first element of the queue O(1)
void Queue::dequeue() {
    if (m_front == nullptr)
        return;
    Node* aux = m_front;
    if (m_front == m_rear) { // only one element
        m_front = nullptr;
        m_rear = nullptr;
    } else {
        m_front = m_front->next;
    }
    delete aux;
}

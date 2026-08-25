#include "ll2.h"

// A List ADT Implementation using Linked Lists

LinkedList::LinkedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

LinkedList::~LinkedList() {
    Node* curr = m_head;
    while (curr) {
        Node* aux = curr->next;
        delete curr;
        curr = aux;
    }
}

void LinkedList::print() const {
    Node* temporary = m_head;
    std::cout << "List(" << m_size << "): ";
    while (temporary) {
        std::cout << temporary->valor << "->";
        temporary = temporary->next;
    }
    std::cout << "NULL" << std::endl;
}

void LinkedList::insert(long valor) {
    Node* nnode = new Node(valor);
    if (m_head == nullptr && m_tail == nullptr) {
        m_head = nnode;
        m_tail = nnode;
    } else {
        nnode->next = m_head;
        m_head = nnode;
    }
    m_size++;
}

void LinkedList::eject() {
    if (m_head != nullptr) {
        Node* temporary = m_head;
        if (m_head->next == nullptr) {
            m_head = nullptr;
            m_tail = nullptr;
        } else {
            m_head = m_head->next;
        }
        delete temporary;
        m_size--;
    }
}

void LinkedList::push(long valor) {
    Node* nnode = new Node(valor);
    if (m_head == nullptr && m_tail == nullptr) {
        m_head = nnode;
        m_tail = nnode;
    } else {
        m_tail->next = nnode;
        m_tail = nnode;
    }
    m_size++;
}

void LinkedList::pop() {
    if (m_head != nullptr) {
        Node* temporary = m_tail;
        if (m_head->next == nullptr) {
            m_head = nullptr;
            m_tail = nullptr;
        } else {
            Node* curr = m_head;
            while (curr->next != temporary)
                curr = curr->next;
            m_tail = curr;
            curr->next = nullptr;
        }
        delete temporary;
        m_size--;
    }
}

void LinkedList::get(size_t index) const {
    Node* temporary = m_head;
    for (long i = 0; i < m_size; i++) {
        if ((size_t)i == index) {
            std::cout << "get(" << index << "): " << temporary->valor << std::endl;
            return;
        }
        temporary = temporary->next;
    }
    std::cout << "index out of range" << std::endl;
}

void LinkedList::set(long valor_antigo, long valor_novo) {
    Node* temporary = m_head;
    while (temporary) {
        if (temporary->valor == valor_antigo) {
            temporary->valor = valor_novo;
            return;
        }
        temporary = temporary->next;
    }
}

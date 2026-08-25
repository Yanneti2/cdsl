#include "dll.h"

// Double Linked List Implementation

DoubleLinkedList::DoubleLinkedList() : m_size(0), m_head(nullptr), m_tail(nullptr) {}

DoubleLinkedList::~DoubleLinkedList() {
    Node* curr = m_head;
    while (curr) {
        Node* aux = curr;
        curr = curr->next;
        delete aux;
    }
}

void DoubleLinkedList::print() const {
    std::cout << "LISTA(" << m_size << "): NULL <->";
    Node* curr = m_head;
    while (curr) {
        std::cout << " " << curr->data << " <->";
        curr = curr->next;
    }
    std::cout << " NULL" << std::endl;
}

// insere um node com data = valor no inicio da lista O(1)
void DoubleLinkedList::insert(int valor) {
    Node* nnode = new Node(valor);
    if (m_head == nullptr && m_tail == nullptr) { // lista vazia
        m_head = nnode;
        m_tail = nnode;
    } else {
        nnode->next = m_head;
        m_head->prev = nnode;
        m_head = nnode;
    }
    m_size++;
}

// remove o primeiro elemento de uma lista O(1)
void DoubleLinkedList::eject() {
    if (m_head == nullptr) return; // lista vazia

    Node* aux = m_head;
    if (m_head->next == nullptr) { // lista unitaria
        m_head = nullptr;
        m_tail = nullptr;
    } else { // caso geral
        m_head = aux->next;
        m_head->prev = nullptr;
    }
    delete aux;
    m_size--;
}

// adds a node to the end of the list O(1)
void DoubleLinkedList::push(int valor) {
    Node* nnode = new Node(valor);
    if (m_head == nullptr && m_tail == nullptr) {
        m_head = nnode;
        m_tail = nnode;
    } else {
        m_tail->next = nnode;
        nnode->prev = m_tail;
        m_tail = nnode;
    }
    m_size++;
}

// deletes a element at the end of the list O(1)
void DoubleLinkedList::pop() {
    if (m_tail == nullptr) return; // lista vazia

    Node* aux = m_tail;
    if (m_head->next == nullptr) {
        m_head = nullptr;
        m_tail = nullptr;
    } else {
        m_tail = aux->prev;
        m_tail->next = nullptr;
    }
    delete aux;
    m_size--;
}

// gets the element at index and prints it O(n)

// pode ser O(1) se  guardar o index de cada elemento na lista

// If you want to get the element at index, and index is in the 
// first half of the list, search forward from the head. If it's 
// in the second half, search backward from the tail. This can cut 
// your maximum search time in half O(n/2)
void DoubleLinkedList::get(size_t index) const {
    if (index >= (size_t)m_size) {
        std::cout << "index out of range" << std::endl;
        return;
    }
    Node* aux = m_head;
    for (size_t i = 0; i < index; i++) {
        aux = aux->next;
    }
    std::cout << "get(" << index << "): " << aux->data << std::endl;
}

// sets a element of data x to data = y
void DoubleLinkedList::set(int x, int y) {
    Node* aux = m_head;
    while (aux) {
        if (aux->data == x) {
            aux->data = y;
            return;
        }
        aux = aux->next;
    }
}

// inserts a element at a given position O(n)
void DoubleLinkedList::insert_at(int valor, size_t index) {
    if (index > (size_t)m_size) {
        std::cout << "index out of range" << std::endl;
        return;
    } else if (index == 0) {
        insert(valor);
        return;
    } else if (index == (size_t)m_size) {
        push(valor);
        return;
    }

    Node* curr = m_head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->next;
    }
    Node* nnode = new Node(valor);
    Node* anterior = curr->prev;

    nnode->next = curr;
    anterior->next = nnode;
    nnode->prev = anterior;
    curr->prev = nnode;
    m_size++;
}

// removes a element at a given position
void DoubleLinkedList::remove_at(size_t index) {
    if (index >= (size_t)m_size) {
        std::cout << "index out of range" << std::endl;
        return;
    } else if (index == 0) {
        eject();
        return;
    } else if (index == (size_t)m_size - 1) {
        pop();
        return;
    }

    Node* aux = m_head;
    for (size_t i = 0; i < index; i++) {
        aux = aux->next;
    }
    Node* anterior = aux->prev;
    Node* posterior = aux->next;
    anterior->next = posterior;
    posterior->prev = anterior;
    delete aux;
    m_size--;
}

// reverses the order of the dll
void DoubleLinkedList::reverse() {
    Node* curr = m_head;
    Node* temporary = nullptr;
    while (curr) {
        temporary = curr->prev;
        curr->prev = curr->next;
        curr->next = temporary;
        curr = curr->prev;
    }
    if (temporary != nullptr) {
        m_tail = m_head;
        m_head = temporary->prev;
    }
}

#include "dll.h"

int main(void){
    DoubleLinkedList lista;
    lista.insert(0);
    lista.insert(1);
    lista.insert(2);
    lista.insert(3);
    lista.print();
    lista.eject();
    lista.print();
    lista.eject();
    lista.eject();
    lista.push(2);
    lista.push(4);
    lista.push(8);
    lista.print();
    lista.pop();
    lista.print();
    lista.get(0);
    lista.get(1);
    lista.get(2);
    lista.get(3);
    lista.get(-1);
    lista.get(4);
    lista.set(4, 2);
    lista.set(0, 2);
    lista.set(-1, 100);
    lista.print();
    lista.insert_at(20, 1);
    lista.print();
    lista.insert_at(10, 0);
    lista.print();
    lista.remove_at(2);
    lista.print();
    lista.reverse();
    lista.print();
    return 0;
}

    ouds.hpp"
#include "general_tree.hpp"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cmath>


bool fc = false;
bool lc = false;
bool c = false;
bool cren = false;
bool ps = false;
bool ns = false;
bool node_map = false;
bool node_sel = false;
bool p = false;
bool cr = false;

void parse_arg(int argc char* argv[]) {
    for (int i = 1; i < argc; i++) {
        const char* cur_arg = argv[i];
        if(strcmp(cur_arg, "-fc") == 0 || strcmp(cur_arg, "-fchild")) 
            fc = true;
        if(strcmp(cur_arg, "-lc") == 0 || strcmp(cur_arg, "-lchild"))
            lc = true;
        if(strcmp(cur_arg, "-c") == 0 || strcmp(cur_arg, "-child"))
            c = true;
        if(strcmp(cur_arg, "-cren") == 0 || strcmp(cur_arg, "-children"))
            cren = true;
        if(strcmp(cur_arg, "-ps") == 0 || strcmp(cur_arg, "-psibling"))
            ps = true;
        if(strcmp(cur_arg, "-ns") == 0 || strcmp(cur_arg, "-nsibling"))
            ns = true;
        if(strcmp(cur_arg, "-node_map") == 0 || strcmp(cur_arg, "-nm")) {
            node_map = true;
        }
        if(strcmp(cur_arg, "-node_sel") == 0 || strcmp(cur_arg, "-nsel")) {
            node_sel = true;
        }
        if(strcmp(cur_arg, "-parent") == 0 || strcmp(cur_arg, "-p"))
            parent = true;
        if(strcmp(cur_arg, "-cr") == 0 || strcmp(cur_arg, "-childrank"))
            cr = true;
    }
}

int main(int argc, char* argv[]){

    for (size_t order = 3; order < 9; order++) {
        num = pow(10, order)
        Gtree* T = rand_tree(n);
        LOUDS L = LOUDS(T);
        auto start = std::chrono::high_resolution_clock::now();
        if (L.is_louds()) {
            if(fc) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.fchild(n);
            }    
        }
            if(lc) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.lchild(n);
            }
            }
            if(c) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.child(n, 3);
            }
            }
            if(cren) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.children(n);
            }
        }
            if(ps){
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.psibling(n);
            }
        }
            if(ns) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.nsibling(n);
            }
        }
            if(node_map) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.nodemap(n);
            }
        }
            if(node_sel){
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.nodeselect(n);
            }
        }
            if(p) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.parent(n);
            }
        }
            if(cr) {
            for (size_t i = 0; i < num; i++) {
            unsigned long long n = rand() % num
            L.childrank(n);
            }
        }
            }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, nano> elapsed_time{end - start};
        cout << elapsed_time / pow(10, order) << endl;
    }
}

#include "parentheses_tree_rmmq.hpp"
#include "iostream"

using namespace std;

int main(void) {
    auto T = ParenthesesTreeRMMQ("(()((()())(()(())))(()((()())(()(()))))(()((()())(()(()))))(()((()())(()(())))))");
    //                            012123434323434543212323454543454565432123234545434545654321232345454345456543210
    cout << "\n\n=== FWDSEARCH ===\n";
    cout << "( 0,  0) -> " << T.forward_search(0, 0) << endl;
    cout << "( 1,  0) -> " << T.forward_search(1, 0) << endl;
    cout << "( 2,  0) -> " << T.forward_search(2, 0) << endl;
    cout << "( 3,  0) -> " << T.forward_search(3, 0) << endl;
    cout << "( 3, -1) -> " << T.forward_search(3, -1) << endl;
    cout << "( 3, -2) -> " << T.forward_search(3, -2) << endl;
    cout << "(31, -3) -> " << T.forward_search(31, -3) << endl;
    cout << "( 4,  0) -> " << T.forward_search(4, 0) << endl;
    cout << "( 4, -1) -> " << T.forward_search(4, -1) << endl;

    cout << "\n\n=== BWDSEARCH ===\n";
    cout << "( 3,  0) -> " << T.backward_search(3, 0) << endl;
    cout << "( 1, -1) -> " << T.backward_search(1, -1) << endl;
    cout << "(32,  0) -> " << T.backward_search(32, 0) << endl;
    cout << "(32,  0) -> " << T.backward_search(80, 0) << endl;

// ( 0,  0) -> 80
// ( 1,  0) -> 3
// ( 2,  0) -> 4
// ( 3,  0) -> 19
// ( 3, -1) -> 80
// ( 3, -2) -> 18446744073709551615
// (31, -3) -> 38
// ( 4,  0) -> 10
// ( 4, -1) -> 19
}

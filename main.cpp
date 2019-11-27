#include <iostream>
#include "tree.h"

int main() {
    Tree <double> difTree ('P', "../formula.txt");
    difTree.dump();

    return 0;
}

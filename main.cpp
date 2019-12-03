//#include <iostream>
#include "tree.h"

Node <double> *diffConst(Node <double> *node, Tree <double> *answerTree);

Node <double> *diffNode(Node <double> *node, Tree <double> *answerTree);

Tree <double> *diffTree(Tree <double> *expression, Tree <double> *answerTree);

Node <double> *diffAdd(Node <double> *node, Tree <double> *answerTree);

Node <double> *diffVar(Node <double> *node, Tree <double> *answerTree);

Node <double> *diffMul(Node <double> *node, Tree <double> *answerTree);

void partDiffForMulDiv(Node <double> *answer, Node <double> *node, Tree <double> *answerTree);

Node <double> *diffDiv(Node <double> *node, Tree <double> *answerTree);

Node <double> *diffSin(Node <double> *node, Tree <double> *answerTree);

int main() {
    Tree <double> expression ('P', "../formula.txt");
    expression.dump();
    expression.saveTex();
    Tree <double> answerTree;

    diffTree(&expression, &answerTree);
//    answerTree.dump();
    return 0;
}

Tree <double> *diffTree(Tree <double> *expression, Tree <double> *answerTree) {
    assert(expression);
    assert(answerTree);

    answerTree->setRoot(diffNode(expression->getRoot(), answerTree));
    return answerTree;
}

Node <double> *diffNode(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    Node <double> *answer = nullptr;

    if (node->nodeType == OPERATION){

#define DEF_CMD(name, num, sign, code, texCode) \
        if (node->value == num) code \
        else
#include "dsl.h"
#undef DEF_CMD
        {}

        if (node->value > 4){
            Node <double> *fullAnswer = answerTree->newNode(MUL, OPERATION);

            fullAnswer->rightChild = answer;
            fullAnswer->leftChild = diffNode(node->rightChild, answerTree);

            return fullAnswer;
        }
    }

    else if (node->nodeType == NUMBER) answer =  diffConst(node, answerTree);
    else if (node->nodeType == VARIABLE) answer =  diffVar(node, answerTree);

    return answer;
}

Node <double> *diffConst(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    return answerTree->newNode(0, NUMBER);
}

Node <double> *diffVar(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    return answerTree->newNode(1, NUMBER);
}

Node <double> *diffAdd(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    Node <double> *answer = answerTree->newNode(node->value, OPERATION);
    answer->leftChild = diffNode(node->leftChild, answerTree);
    answer->rightChild = diffNode(node->rightChild, answerTree);
    return answer;
}

Node <double> *diffMul(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    Node <double> *answer = answerTree->newNode(ADD, OPERATION);
    partDiffForMulDiv(answer, node, answerTree);
    return answer;
}

Node <double> *diffDiv(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    Node <double> *answer = answerTree->newNode(DIV, OPERATION);

    answer->leftChild = answerTree->newNode(SUB, OPERATION);
    partDiffForMulDiv(answer->leftChild, node, answerTree);

    Node <double> *denom = answerTree->newNode(DEG, OPERATION);
    denom->leftChild = answerTree->newNode(node->rightChild->value, node->rightChild->nodeType);
    denom->rightChild = answerTree->newNode(2, NUMBER);

    answer->rightChild = denom;
    return answer;
}

void partDiffForMulDiv(Node <double> *answer, Node <double> *node, Tree <double> *answerTree) {
    assert(answer);
    assert(node);
    assert(answerTree);

    Node <double> *leftMul = answer->leftChild = answerTree->newNode(MUL, OPERATION);
    leftMul->leftChild = diffNode(node->leftChild, answerTree);
    leftMul->rightChild = answerTree->copySubtree(node->rightChild);

    Node <double> *rightMul = answer->rightChild = answerTree->newNode(MUL, OPERATION);
    rightMul->leftChild = answerTree->copySubtree(node->leftChild);
    rightMul->rightChild = diffNode(node->rightChild, answerTree);
}

Node <double> *diffSin(Node <double> *node, Tree <double> *answerTree) {
    assert(node);
    assert(answerTree);

    Node <double> *answer = answerTree->newNode(COS, OPERATION);
    Node <double> *rightVal = answerTree->copySubtree(node->rightChild);

    answer->rightChild = rightVal;
    return answer;
}


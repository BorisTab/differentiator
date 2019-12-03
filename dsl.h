//
// Created by boristab on 26.11.2019.
//

#include "tree.h"

DEF_CMD(ADD, 1, "+", {
    answer =  diffAdd(node, answerTree);
}, {
    nodeToTex(node->leftChild, tex);
    fprintf(tex, "+");
    nodeToTex(node->rightChild, tex);
})

DEF_CMD(SUB, 2, "-", {
    answer = diffAdd(node, answerTree);
}, {
    nodeToTex(node->leftChild, tex);
    fprintf(tex, "-");
    nodeToTex(node->rightChild, tex);
})

DEF_CMD(MUL, 3, "*", {
    answer = diffMul(node, answerTree);
}, {
    nodeToTex(node->leftChild, tex);
    fprintf(tex, "\\cdot");
    nodeToTex(node->rightChild, tex);
})

DEF_CMD(DIV, 4, "/", {
    answer = diffDiv(node, answerTree);
}, {
    fprintf(tex, "\\frac{");
    nodeToTex(node->leftChild, tex);
    fprintf(tex, "}{");
    nodeToTex(node->rightChild, tex);
    fprintf(tex, "}");
})

DEF_CMD(DEG, 5, "^", {}, {})

DEF_CMD(SIN, 6, "sin", {
    answer = diffSin(node, answerTree);
}, {
    fprintf(tex, "\\sin{");
//    if (node->rightChild->nodeType == OPERATION) fprintf(tex, "(");
    nodeToTex(node->rightChild, tex);
//    if (node->rightChild->nodeType == OPERATION) fprintf(tex, ")");
    fprintf(tex, "}");
})

DEF_CMD(COS, 7, "cos", {}, {})

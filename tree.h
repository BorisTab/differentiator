//
// Created by boristab on 08.11.2019.
//

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include "fileRead.h"

enum errors {
    SUCCESS = 0,
    FILE_OPEN_FAILED = 1,
    NOT_FOUND_TREE_IN_FILE = 2,
    WRONG_KEY = 3,
    SYNTAX_ERROR = 4,
};

enum nodeTypes {
    NO_TYPE = 0,
    NUMBER = 1,
    OPERATION = 2,
    VARIABLE = 3,
};

enum operations {
    NOTHING = 0,
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    DEG = 5,
};

const char dumpFilePath[FILENAME_MAX] = "../TreeDumpFile.txt";

int spaceN (const char *buffer);

template <class elemType>
class Node {
public:
    Node <elemType> *parent = nullptr;
    Node <elemType> *leftChild = nullptr;
    Node <elemType> *rightChild = nullptr;
    elemType value = {};
    int nodeType = 0;

    explicit Node(elemType value) {
        this->value = value;
    }
};


template <class elemType>
class Tree {
private:
    Node <elemType> *root = nullptr;
    char *s = nullptr;

    void printNodeInit(std::ofstream *dumpFile, Node <elemType> *node) {
        assert(node);
        assert(dumpFile);

        *dumpFile << "node_" << node << " [shape=record, label=\" { "<< node
            << " | { Val: " << node->value << " | Type: " << node->nodeType
            << " } | { left: " << node->leftChild
            << " | right: " << node->rightChild << " } } \"];\n";

        if (node->leftChild) printNodeInit(dumpFile, node->leftChild);
        if (node->rightChild) printNodeInit(dumpFile, node->rightChild);
    }

    void printNodeRel(std::ofstream *dumpFile, Node <elemType> *node) {
        if (node->leftChild) *dumpFile << "node_" << node << "-- node_" << node->leftChild << ";\n";
        if (node->rightChild) *dumpFile << "node_" << node << "-- node_" << node->rightChild << ";\n";

        if (node->leftChild) printNodeRel(dumpFile, node->leftChild);
        if (node->rightChild) printNodeRel(dumpFile, node->rightChild);
    }

    void saveNode(std::ofstream *outFile, Node <elemType> *node) {
        assert(outFile);
        assert(node);

        *outFile << "{ \"" << node->value << "\" ";

        if (!node->leftChild && !node->rightChild) {
            *outFile << "} ";
            return;
        }

        if (node->leftChild) saveNode(outFile, node->leftChild);
        else *outFile << "$ ";

        if (node->rightChild) saveNode(outFile, node->rightChild);
        else *outFile << "$ ";

        *outFile << "} ";
    }

    void writeNode(char **buffer, Node <elemType> *node) {
        if (**buffer == '$') (*buffer) += 2;
        else if (**buffer == '{'){
            (*buffer) += 2 + spaceN((*buffer) + 1);
            *(strchr(*buffer, '"')) = '\0';

            insertLeft(node, *buffer);
            (*buffer) += strlen(*buffer) + 2;
            writeNode(buffer, node->leftChild);
        }

        if (**buffer == '$') (*buffer) += 2;
        else if (**buffer == '{'){
            (*buffer) += 2 + spaceN((*buffer) + 1);
            *(strchr(*buffer, '"')) = '\0';

            insertRight(node, *buffer);
            (*buffer) += strlen(*buffer) + 2;
            writeNode(buffer, node->rightChild);
        }

        if (**buffer == '}') {
            (*buffer) += 2;
            return;
        }
    }

//    void infixRead(const char *inPath) {
//        int fileSize = getFileSize(inPath);
//
//        char *buffer = new char[fileSize] ();
//        char *bufferStart = buffer;
//        readFile(inPath, buffer, fileSize);
//
//        if (*buffer != '{') {
//            printf("Error: No tree in file");
//            exit(NOT_FOUND_TREE_IN_FILE);
//        }
//
//        if (*(buffer + spaceN(buffer + 1)) == '}') {
//            printf("Error: tree is empty");
//            exit(NOT_FOUND_TREE_IN_FILE);
//        }
//
//        buffer += 2 + spaceN(buffer + 1);
//        *(strchr(buffer + 1, '"')) = '\0';
//        auto *node = newNode(buffer);
//        root = node;
//        buffer += strlen(buffer);
//        buffer += spaceN(buffer + 1) + 1;
//
//        writeNode(&buffer, root);
//    }

    void prefixRead(const char *inPath) {
        int fileSize = getFileSize(inPath);

        char *buffer = new char[fileSize] ();
        char *bufferStart = buffer;
        readFile(inPath, buffer, fileSize);

        getG(buffer);
    }

    Node <elemType> *getG(char *str) {
        s = str;
        Node <elemType> *valNode = getE();
        if (*s != '\0') {
            printf("Syntax error: expected end of row\n");
            exit(SYNTAX_ERROR);
        }
        root = valNode;
        return valNode;
    }

    Node <elemType> *getN() {

        if ((*s == '-' && isdigit(*(s + 1))) || isdigit(*s)){
            elemType val = 0;
            int size = 0;

            sscanf(s, "%lf%n", &val, &size);
            s += size;

            Node <elemType> *node = newNode(val);
            node->nodeType = NUMBER;
            return node;
        }

        double val = *s - 'a';
        s++;

        Node <elemType> *node = newNode(val);
        node->nodeType = VARIABLE;
        return node;
    }

    Node <elemType> *getT() {
        Node <elemType> *valLeft = getD();
        Node <elemType> *node = nullptr;
        Node <elemType> *valRight = nullptr;

        while (*s == '*' || *s == '/') {
            char op = *s;
            s++;

            valRight = getD();

            if (op == '*') {
                node = newNode(MUL);
            }else node = newNode(DIV);
        }

        if (!node) return valLeft;
        tyingNodes(node, valLeft, valRight);
        node->nodeType = OPERATION;

        return node;
    }

    void tyingNodes(Node <elemType> *node, Node <elemType> *valLeft, Node <elemType> *valRight) {
        node->leftChild = valLeft;
        node->leftChild->parent = node;

        node->rightChild = valRight;
        node->rightChild->parent = node;
    }

    Node <elemType> *getE() {
        Node <elemType> *valLeft = getT();
        Node <elemType> *node = nullptr;
        Node <elemType> *valRight = nullptr;

        while (*s == '+' || *s == '-') {
            char op = *s;
            s++;

            valRight = getT();

            if (op == '+') {
                node = newNode(ADD);
            }else node = newNode(SUB);
        }

        if (!node) return valLeft;
        tyingNodes(node, valLeft, valRight);
        node->nodeType = OPERATION;

        return node;
    }

    Node <elemType> *getP() {
        if (*s == '(') {
            s++;
            Node <elemType> *valNode = getE();
            if (*s != ')') {
                printf("Syntax error: expected ')'\n");
                exit(SYNTAX_ERROR);
            }
            s++;
            return valNode;
        } else return getN();
    }

    Node <elemType> *getD() {
        Node <elemType> *valLeft = getP();
        Node <elemType> *node = nullptr;
        Node <elemType> *valRight = nullptr;

        while (*s == '^') {
            char op = *s;
            s++;

            valRight = getP();
            node = newNode(DEG);
        }

        if (!node) return valLeft;
        tyingNodes(node, valLeft, valRight);
        node->nodeType = OPERATION;

        return node;
    }

public:
    explicit Tree(elemType val) {
        auto *node = newNode(val);

        root = node;
    }

    Tree(char type, const char *inPath) {
        if (type == 'P') prefixRead(inPath);

//        else if (type == 'I') infixRead(inPath);

        else {
            printf("Error: Wrong key");
            exit(WRONG_KEY);
        }
    }

    Node <elemType> *newNode(elemType val) {
        return new Node <elemType> (val);
    }

    void insertNodeLeft(Node <elemType> *parent, Node <elemType> *node) {
        parent->leftChild = node;
    }

    void insertNodeRight(Node <elemType> *parent, Node <elemType> *node) {
        parent->rightChild = node;
    }

    void insertLeft(Node <elemType> *parentNode, elemType val) {
        auto *node = newNode(val);

        parentNode->leftChild = node;
        node->parent = parentNode;
    }

    void insertRight(Node <elemType> *parentNode, elemType val) {
        auto *node = newNode(val);

        parentNode->rightChild = node;
        node->parent = parentNode;
    }

    void deleteSubTree(Node <elemType> *node) {
        if (!node) return;

        if (node->leftChild) {
            deleteSubTree(node->leftChild);
        } else if (node->rightChild) {
            deleteSubTree(node->rightChild);
        } else {
            delete node;
        }
    }

    Node <elemType> *getRoot() {
        return root;
    }

    Node <elemType> *getLeftChild(Node <elemType> *node) {
        assert(node);

        return node->leftChild;
    }

    Node <elemType> *getRightChild(Node <elemType> *node) {
        assert(node);

        return node->rightChild;
    }

    Node <elemType> getParent(Node <elemType> *node) {
        assert(node);

        return node->parent;
    }

    elemType getVal(Node <elemType> *node) {
        return node->value;
    }

    Node <elemType> *findElem(Node <elemType> *subtree, elemType val) {
        assert(subtree);

        if(subtree->value == val) return subtree;
        if (subtree->leftChild) findElem(subtree->leftChild);
        if (subtree->rightChild) findElem(subtree->rightChild);
    }

    void changeVal(Node <elemType> *node, elemType val) {
        node->value = val;
    }

    void dump() {
        std::ofstream  dumpFile (dumpFilePath);
        if (!dumpFile) {
            printf("File isn't open\n");
            exit(FILE_OPEN_FAILED);
        }

        dumpFile << "graph G{\n";

        if (root) {
            Node <elemType> *currentNode = root;
            printNodeInit(&dumpFile, root);
            printNodeRel(&dumpFile, root);
        }

        dumpFile << "}\n";

        dumpFile.close();

        char dotCommand[FILENAME_MAX] = "";
        sprintf(dotCommand, "dot -Tpng -O %s", dumpFilePath);
        std::system(dotCommand);
    }

    void saveTo(const char *path) {
        std::ofstream outFile (path);

        if (!outFile) {
            printf("File isn't open\n");
            exit(FILE_OPEN_FAILED);
        }

        saveNode(&outFile, root);
        outFile.close();
    }
};

int spaceN (const char *buffer) {
    int count = 0;

    while (*buffer + count == ' ') {
        count++;
    }

    return count;
}

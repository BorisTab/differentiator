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

#define DEF_CMD(name, num, sign, code, texCode) \
    name = num,

enum operations {
    NOTHING = 0,
#include "dsl.h"
#undef DEF_CMD
};

#define DEF_CMD(name, num, sign, code, texCode) \
    case num: return sign;

const char *getEnumName(int op) {
    switch (op) {
#include "dsl.h"
#undef DEF_CMD
        default: return nullptr;
    }
}

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
            << " | { Val: ";

        if (node->nodeType == VARIABLE) *dumpFile << (char) (node->value + 'a');
        else if (node->nodeType == OPERATION) *dumpFile << getEnumName(node->value);
        else *dumpFile << node->value;

        *dumpFile << " | Type: " << node->nodeType
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
        if (valLeft) node->leftChild->parent = node;

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
        Node <elemType> *valLeft = getF();
        Node <elemType> *node = nullptr;
        Node <elemType> *valRight = nullptr;

        while (*s == '^') {
            char op = *s;
            s++;

            valRight = getF();
            node = newNode(DEG);
        }

        if (!node) return valLeft;
        tyingNodes(node, valLeft, valRight);
        node->nodeType = OPERATION;

        return node;
    }

    Node <elemType> *getF() {
        Node <elemType> *node = nullptr;
        Node <elemType> *valRight = nullptr;

        if (!isalpha(*s)) {
            return getP();
        }

        char func[10] = "";
        char *funcStart = s;
        while (isalpha(*s)) {
            func[s - funcStart] = *s;
            s++;
        }

        if (*s != '(') {
            s = funcStart;
            return getN();
        }
        s++;

        int val = 0;
#define DEF_CMD(name, num, sign, code, texCode) \
    if (!strcmp(func, sign)) val = num; \
    else
#include "dsl.h"
#undef DEF_CMD
        {
            printf("Syntax error: unknown function %s", func);
            exit(SYNTAX_ERROR);
        }

        valRight = getE();

        if (*s != ')') {
            printf("Syntax error: expected ')' after %s argument", func);
            exit(SYNTAX_ERROR);
        }
        s++;

        node = newNode(val, OPERATION);
        tyingNodes(node, nullptr, valRight);
        return node;
    }

    void nodeToTex(Node <double> *node, FILE *tex) {
        if (node->nodeType == NUMBER) fprintf(tex, "%g", node->value);
        else if (node->nodeType == VARIABLE) fprintf(tex, "%c", (char) (node->value + 'a'));
        else if (node->nodeType == OPERATION) {
#define DEF_CMD(name, num, sign, code, texCode) \
            case num: texCode break;

            switch ((int) node->value) {
#include "dsl.h"

                default: printf("Error: unknown function");
            }
#undef DEF_CMD

        }
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

    Tree() = default;

    Node <elemType> *newNode(elemType val, elemType type = NOTHING) {
        auto *node = new Node <elemType> (val);
        node->nodeType = type;
        return node;
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

    void setRoot(Node <elemType> *node) {
        root = node;
    }

    Node <elemType> *copySubtree(Node <elemType> *node) {
        Node <elemType> *subtree = newNode(node->value, node->nodeType);

        if (node->leftChild) subtree->leftChild = copySubtree(node->leftChild);
        if (node->rightChild) subtree->rightChild = copySubtree(node->rightChild);

        return subtree;
    }

    void saveTex() {
        FILE *tex = fopen("../diff.tex", "w");

        fprintf(tex, "\\documentclass[12pt,a4paper]{scrartcl}\n"
                     "\\usepackage[utf8]{inputenc}\n"
                     "\\usepackage[english,russian]{babel}\n"
                     "\\usepackage{indentfirst}\n"
                     "\\usepackage{misccorr}\n"
                     "\\usepackage{graphicx}\n"
                     "\\usepackage{amsmath}\n"
                     "\\begin{document}\n");
        fprintf(tex, "\\begin{equation}\\label{eq:1}");

        fprintf(tex, "f = ");
        nodeToTex(root, tex);

        fprintf(tex, "\\end{equation}");
        fprintf(tex, "\\end{document}");

        fclose(tex);
        std::system("pdflatex ../diff.tex");
    }
};

int spaceN (const char *buffer) {
    int count = 0;

    while (*buffer + count == ' ') {
        count++;
    }

    return count;
}

#ifndef BTREENODE_H
#define BTREENODE_H
#include <vector>
#include "index.h"
#include <iostream>

#define BYTE char
#define BLOCK_SIZE 4096

enum nodeType{INIT,ROOT,INNODE,LEAF};
class bTreeNode
{
    private:
        nodeType type;
        indexType indtype;
        unsigned int valNum;
        unsigned int maxNum;
        std::vector<unsigned int> ptrList;
        std::vector<Index> indexList;
        unsigned int parentPtr;
        BYTE* blockPtr;
        int blockNo;
        unsigned int charLength;
        bool dirty;
    public:
        friend class bTree;
        bTreeNode();
        bTreeNode(indexType it, unsigned int len, nodeType tp, BYTE* bPtr);
        ~bTreeNode();
        void init();
        void writeBack();
        void testOutput();
        bool isFull();
};

#endif

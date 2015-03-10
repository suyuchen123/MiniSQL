#ifndef BTREE_H
#define BTREE_H
#include "buffer.h"
#include "bTreeNode.h"

class bTree
{
    private:
        string indexName;
        indexType indtype;
        unsigned int root;
        unsigned int charLength;
        std::vector<unsigned int> emptyBlock;
        std::vector<unsigned int> findPath;
        buffer* bm;
        //test
        //int blockCount;
        //std::vector<BYTE*> blocks;
        //test
    public:
        bTree(string name, buffer* bfm, indexType it, int length);
        bTree(string name, indexType t, int length, buffer* bfm);
        ~bTree();
        string getName();
        bTreeNode findFirstNode(Index ind);
        std::vector<unsigned int> findAll(Index ind);
        std::vector<unsigned int> findGreater(Index ind, bool equal);
        std::vector<unsigned int> findLess(Index ind, bool equal);
        bTreeNode assignNode(unsigned int blockNo);
        bTreeNode createNode(nodeType nt);
        void freeNode(bTreeNode& node);
        void insertIndex(Index ind);
        void insertNode(unsigned int parent, unsigned int child, unsigned int pos, Index ind);
        Index findMinIndex(bTreeNode& node);
        void deleteIndex(Index ind);
        void deleteNode(unsigned int parent, unsigned int path);
        bTreeNode findBro(bTreeNode& node);
        void cleanNode(bTreeNode& node);
        void printTree();
        void bfs(bTreeNode& b, std::vector<bTreeNode>& t);
};

#endif

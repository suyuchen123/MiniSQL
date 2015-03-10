#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H
#include "bTree.h"

class IndexManager{
    private:
        std::vector<bTree> bTreeBuffer;
        int bufferMax;
        buffer* bfm;
        indexType type;
        int charLength;

        bTree& findTree(string indexName);
    public:
        IndexManager(buffer& bm);
        IndexManager(buffer& bm, int max);
        void setIndex(indexType it, int length);
        void setBufferSize(int max);
        std::vector<unsigned int> find(string indexName,Index ind);
        void insert(string indexName,Index ind);
        void del(string indexName,Index ind);
        void create(string indexName,indexType it,int length);
        void drop(string indexName);
        std::vector<unsigned int> findLess(string indexName,Index ind,bool equal);
        std::vector<unsigned int> findGreater(string indexName,Index ind,bool equal);
        void print(string name);
};

#endif

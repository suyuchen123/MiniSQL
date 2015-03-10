#include "indexManager.h"

IndexManager::IndexManager(buffer& bm){
    bufferMax = 200;
    bfm = &bm;
}

IndexManager::IndexManager(buffer& bm, int max){
    bufferMax = max;
    if(bufferMax<1)
        bufferMax = 1;
    bfm = &bm;
}

void IndexManager::setBufferSize(int max){
    bufferMax = max;
    if(bufferMax<1)
        bufferMax = 1;
}

std::vector<unsigned int> IndexManager::find(string indexName,Index ind){
    bTree t = findTree(indexName);
    return t.findAll(ind);
}

std::vector<unsigned int> IndexManager::findGreater(string indexName,Index ind,bool equal){
    bTree t = findTree(indexName);
    return t.findGreater(ind,equal);
}

std::vector<unsigned int> IndexManager::findLess(string indexName,Index ind,bool equal){
    bTree t = findTree(indexName);
    return t.findLess(ind,equal);
}

void IndexManager::insert(string indexName,Index ind){
    bTree t = findTree(indexName);
    t.insertIndex(ind);
}

void IndexManager::del(string indexName,Index ind){
    bTree t = findTree(indexName);
    t.deleteIndex(ind);
}

void IndexManager::create(string indexName, indexType it, int length){
    bTreeBuffer.push_back(bTree(indexName, it, length, bfm));
    if(bTreeBuffer.size()>bufferMax)
        bTreeBuffer.erase(bTreeBuffer.begin());
}

void IndexManager::drop(string indexName){
    bfm->deleteFile(indexName);
}

bTree& IndexManager::findTree(string indexName){
    std::vector<bTree>::iterator iter;
    for (iter = bTreeBuffer.begin(); iter < bTreeBuffer.end(); ++iter)
    {
        if(iter->getName()==indexName)
            return *iter;
    }
    bTreeBuffer.push_back(bTree(indexName,bfm,type,charLength));
    if(bTreeBuffer.size()>bufferMax)
        bTreeBuffer.erase(bTreeBuffer.begin());
    return bTreeBuffer.back();
}

void IndexManager::setIndex(indexType it, int length){
    type = it;
    charLength = length;
}

void IndexManager::print(string name){
    bTree t = findTree(name);
    t.printTree();
}

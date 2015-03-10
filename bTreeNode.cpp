#include "bTreeNode.h"

bTreeNode::bTreeNode(){
    dirty = false;
    valNum = 0;
    parentPtr = 0;
    indtype = NONE;
    type = INIT;
}

bTreeNode::bTreeNode(indexType it, unsigned int len, nodeType tp, BYTE* bPtr){
    dirty = false;
    valNum = 0;
    parentPtr = 0;
    indtype = it;
    type = tp;
    blockPtr = bPtr;
    int length;
    if(it==INT)
        length=sizeof(int);
    if(it==FLOAT)
        length=sizeof(float);
    if(it==STRING)
    {
        charLength = len;
        length=len*sizeof(char);
    }
    maxNum = (BLOCK_SIZE - sizeof(char) - sizeof(unsigned int) - sizeof(unsigned int) -sizeof(char) - sizeof(unsigned int) - sizeof(unsigned int)) / (sizeof(unsigned int) + length);

    //test
    //maxNum = 3;
    //test
}

bTreeNode::~bTreeNode(){
    ptrList.clear();
    indexList.clear();
}

void bTreeNode::init(){
    unsigned int i,k;
    BYTE* cur;
    cur=blockPtr;
    cur+=sizeof(char);
    parentPtr=*(unsigned int*)cur;
    cur+=sizeof(unsigned int);
    valNum=*(unsigned int*)cur;
    cur+=sizeof(unsigned int);
    for (i = 0; i < valNum; ++i)
    {
        Index ind;
        unsigned int l = *(unsigned int*)cur;
        cur+=sizeof(unsigned int);
        ptrList.push_back(l);
        switch (indtype)
        {
            case INT:
                int t;
                t = *(int*)cur;
                cur+=sizeof(int);
                ind.init(t);
                break;
            case FLOAT:
                float f;
                f = *(float*)cur;
                cur+=sizeof(float);
                ind.init(f);
                break;
            case STRING:
                string s="";
                for (k = 0; k < charLength; ++k)
                {
                    char c = *(char*)cur;
                    cur+=sizeof(char);
                    if (c == '\0')
                        break;
                    else
                        s+=c;
                }
                ind.init(s);
                break;
        }
        if(type != LEAF)
        {
            l = *(unsigned int*)cur;
            cur+=sizeof(unsigned int);
        }
        ind.setTuple(l);
        indexList.push_back(ind);
    }
    unsigned int l = *(unsigned int*)cur;
    cur+=sizeof(unsigned int);
    ptrList.push_back(l);
}

void bTreeNode::writeBack(){
    if(dirty)
    {
        int i;
        BYTE* cur;
        cur=blockPtr;
        switch(type)
        {
            case ROOT:
                *(char*)cur='R';
                break;
            case INNODE:
                *(char*)cur='I';
                break;
            case LEAF:
                *(char*)cur='L';
                break;
        }
        cur+=sizeof(char);
        *(unsigned int*)cur=parentPtr;
        cur+=sizeof(unsigned int);
        *(unsigned int*)cur=valNum;
        cur+=sizeof(unsigned int);
        for (i = 0; i < valNum; ++i)
        {
            unsigned int l = ptrList[i];
            *(unsigned int*)cur=l;
            cur+=sizeof(unsigned int);
            switch (indtype)
            {
                int k;
                case INT:
                    *(int*)cur = indexList[i].getInt();
                    cur+=sizeof(int);
                    break;
                case FLOAT:
                    *(float*)cur = indexList[i].getFloat();
                    cur+=sizeof(float);
                    break;
                case STRING:
                    string s = indexList[i].getString();
                    for (k = 0; k < s.length(); ++k)
                    {
                        *(char*)cur = s[k];
                        cur+=sizeof(char);
                    }
                    if(k<charLength)
                    {
                        *(char*)cur = '\0';
                        cur+=sizeof(char);
                    }
                    break;
            }
            if(type != LEAF)
            {
                *(unsigned int*)cur = indexList[i].getTuple();;
                cur+=sizeof(unsigned int);
            }
        }
        *(unsigned int*)cur = ptrList.back();
        cur+=sizeof(unsigned int);
    }
    else
        return;
}

void bTreeNode::testOutput(){
    unsigned int i;
    for (i = 0; i < valNum; ++i)
    {
        std::cout<<"["<<ptrList[i]<<"] ";
        switch (indtype)
        {
            case INT:
                std::cout<<indexList[i].getInt()<<' ';
                break;
            case FLOAT:
                std::cout<<indexList[i].getFloat()<<' ';
                break;
            case STRING:
                std::cout<<indexList[i].getString()<<' ';
                break;
            default:
                std::cout<<"error";
                break;
        }
    }
    std::cout<<"["<<ptrList.back()<<"]";
}

bool bTreeNode::isFull(){
    return valNum>maxNum;
}

#include "bTree.h"

bTree::bTree(string name, buffer* bfm, indexType it, int length){
    indexName = name;
    bm = bfm;
    indtype = it;
    charLength = length;
    root = 0;

    int n = bm->getNumberOfBlocks(name);
    int i;
    //std::cout<<"number:"<<n<<std::endl;
    for (i = 1; i <= n; ++i)
    {
        BYTE* ptr = bm->getoffsetBlockAddress(name, i-1);
        if (ptr == NULL)
        {
            std::cout<<"bTree：fail to read "<<indexName<<" No "<<i-1<<"block"<<std::endl;
        }
        //std::cout<<"address:"<<ptr<<std::endl;
        char c = *ptr;
        //std::cout<<"char:"<<c<<std::endl;
        if(c=='E')
            emptyBlock.push_back(i);
        if(c=='R')
            root = i;
    }
    if(root == 0)
        std::cout<<"bTree：初始化时无法找到根节点"<<std::endl;
    //test
    //root=1;
    //indtype=INT;
    //BYTE* t;
    //t = new BYTE[100];
    //blocks.push_back(t);
    //*t='R';
    //t+=sizeof(char);
    //*(unsigned int*)t = 0;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 1;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 2;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 4;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 1;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 3;
    //t+=sizeof(unsigned int);

    //t = new BYTE[100];
    //blocks.push_back(t);
    //*t='L';
    //t+=sizeof(char);
    //*(unsigned int*)t = 1;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 3;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 4;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 1;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 7;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 2;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 5;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 3;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 3;
    //t+=sizeof(unsigned int);

    //t = new BYTE[100];
    //blocks.push_back(t);
    //*t='L';
    //t+=sizeof(char);
    //*(unsigned int*)t = 1;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 2;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 3;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 4;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 2;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 5;
    //t+=sizeof(unsigned int);
    //*(unsigned int*)t = 0;
    //t+=sizeof(unsigned int);

    //blockCount = 3;
    //test

}

bTree::bTree(string name, indexType t, int length, buffer* bfm){
    bm = bfm;
    indexName = name;
    indtype = t;
    charLength = length;
    bm->createNewFile(indexName);
    bTreeNode r = createNode(ROOT);
    bTreeNode l = createNode(LEAF);
    r.ptrList.push_back(l.blockNo);
    l.ptrList.push_back(0);
    root = r.blockNo;
    l.parentPtr = root;
    freeNode(r);
    freeNode(l);
}

bTreeNode bTree::findFirstNode(Index ind){

    bTreeNode cur = assignNode(root);
    bTreeNode par = cur;
    findPath.clear();

    int ser;
    while(cur.type!=LEAF)
    {
        for (ser = 0; ser < cur.valNum; ++ser)
        {
            if(ind<cur.indexList[ser])
                break;
        }
        findPath.push_back(ser);
        if(cur.type!=ROOT)
        {
            freeNode(par);
            par = cur;
        }
        cur = assignNode(cur.ptrList[ser]);
    }
    freeNode(par);
    return cur;
}

std::vector<unsigned int> bTree::findAll(Index ind){
    unsigned int i;
    std::vector<unsigned int> res;
    bTreeNode cur = findFirstNode(ind);

    for (i = 0; i < cur.valNum; ++i)
    {
       if(cur.indexList[i]==ind) 
           break;
    }
    if(i>=cur.valNum)
    {
        freeNode(cur);
        if(cur.ptrList.back()==0)
            return res;
        cur = assignNode(cur.ptrList.back());
        if(cur.indexList.front()!=ind)
            return res;
        i = 0;
    }
    while(cur.indexList[i]==ind)
    {
        res.push_back(cur.ptrList[i++]);
        if(i==cur.valNum)
        {
            if(cur.ptrList[i]==0)
                break;
            freeNode(cur);
            cur = assignNode(cur.ptrList[i]);
            i = 0;
        }
    }
    freeNode(cur);
    return res;
}

bTreeNode bTree::assignNode(unsigned int blockNo){
    BYTE* ptr;

    //get pointer from buffer
    ptr = bm->getoffsetBlockAddress(indexName, blockNo-1);
    if(ptr == NULL)
    {
        std::cout<<"bTree：在"<<indexName<<"中无法找到第"<<blockNo-1<<"块block"<<std::endl;
    }

    //test
    //ptr = blocks[blockNo-1];
    //test

    nodeType nt;

    //get nodeType from block
    char t = *(char*)ptr;
    switch(t)
    {
        case 'R':
            nt = ROOT;
            break;
        case 'L':
            nt = LEAF;
            break;
        case 'I':
            nt = INNODE;
            break;
        default:
            std::cout<<"bTree：在"<<indexName<<"中第"<<blockNo-1<<"块block数据有误"<<std::endl;
            break;
    }

    bTreeNode temp(indtype,charLength,nt,ptr);
    temp.init();
    temp.blockNo = blockNo;
    return temp;
}

void bTree::freeNode(bTreeNode& node){
    node.writeBack();
    //int r = bm->freeBlock(indexName,node.blockNo-1,node.dirty);
    int r = bm->freeBlock_static(indexName,node.blockNo-1,node.dirty);
    if(!r)
        std::cout<<"bTree：在释放"<<indexName<<"中第"<<node.blockNo-1<<"块时失败"<<std::endl;

    //unlock buffer
    return;
}

void bTree::insertIndex(Index ind){
    unsigned int i;
    bTreeNode cur = findFirstNode(ind);

    for (i = 0; i < cur.valNum; ++i)
    {
        if (ind<cur.indexList[i])
            break; 
    }
    cur.dirty = true;
    //if(i==cur.valNum)
        //cur.indexList.push_back(ind);
    //else
        //cur.indexList.insert(cur.indexList.begin()+i,ind);

    //std::cout<<"num:"<<cur.valNum<<std::endl;

    if (cur.valNum>0)
        cur.indexList.insert(cur.indexList.begin()+i,ind);
    else
        cur.indexList.push_back(ind);
    cur.valNum++;
    cur.ptrList.insert(cur.ptrList.begin()+i,ind.getTuple());
    
    //std::cout<<ind.getInt()<<std::endl;
    //cur.testOutput();
    //std::cout<<std::endl;

    if (cur.isFull())
    {
        bTreeNode newNode = createNode(LEAF);
        unsigned int half = cur.valNum/2;
        newNode.valNum = cur.valNum - half;
        for (i = 0; i < newNode.valNum; ++i)
        {
            newNode.indexList.push_back(cur.indexList[i+half]);
            newNode.ptrList.push_back(cur.ptrList[i+half]);
        }
        newNode.ptrList.push_back(cur.ptrList.back());
        newNode.parentPtr = cur.parentPtr;
        cur.indexList.erase(cur.indexList.begin()+half,cur.indexList.end());
        cur.ptrList.erase(cur.ptrList.begin()+half,cur.ptrList.end());
        cur.ptrList.push_back(newNode.blockNo);
        cur.valNum = half;
        freeNode(newNode);
        freeNode(cur);
        insertNode(newNode.parentPtr,newNode.blockNo,findPath.back(),newNode.indexList.front());
    }
    else
        freeNode(cur);
    return;
}

bTreeNode bTree::createNode(nodeType nt){
    BYTE* ptr;
    unsigned int blockNo;
    if(!emptyBlock.empty())
    {

        //get pointer from buffer by emptyBlock.back()
        //test
        //ptr = blocks.back();
        //blockNo = ++blockCount;
        //blocks.push_back(ptr);
        //test

        ptr = bm->getoffsetBlockAddress(indexName,emptyBlock.back()-1);
        blockNo = emptyBlock.back();
        if(ptr == NULL)
            std::cout<<"bTree：在"<<indexName<<"中无法找到第"<<emptyBlock.back()-1<<"块block"<<std::endl;

        emptyBlock.pop_back();

    }
    else
    {

        //get pointer from buffer by create a new block
        //test
        //ptr = new BYTE[100];
        //blockNo = ++blockCount;
        //blocks.push_back(ptr);
        //test
        
        int n = bm->addNewBlock(indexName);


        n = bm->block[n].offset;

        ptr = bm->getoffsetBlockAddress(indexName,n);
        blockNo = n + 1;
        if(ptr == NULL)
            std::cout<<"bTree：在"<<indexName<<"中无法找到第"<<blockNo-1<<"块block"<<std::endl;
    }
    bTreeNode temp(indtype,charLength,nt,ptr);
    temp.blockNo = blockNo;
    temp.dirty = true;
    return temp;
}

void bTree::insertNode(unsigned int parent, unsigned int child, unsigned int pos, Index ind){
    findPath.pop_back();
    bTreeNode cur = assignNode(parent);
    cur.dirty = true;
    if (cur.valNum>0)
        cur.indexList.insert(cur.indexList.begin()+pos,ind);
    else
        cur.indexList.push_back(ind);
    cur.valNum++;
    cur.ptrList.insert(cur.ptrList.begin()+pos+1,child);
    Index minInd;
    if (cur.isFull())
    {
        unsigned int i;
        bTreeNode newNode = createNode(INNODE);
        unsigned int half = (cur.valNum+1)/2;
        cur.valNum = cur.valNum - half;
        newNode.valNum = half - 1;
        for (i = 0; i < newNode.valNum+1; ++i)
        {
            newNode.indexList.push_back(cur.indexList[i+half]);
            newNode.ptrList.push_back(cur.ptrList[i+half+1]);
        }

        minInd = newNode.indexList.front();
        newNode.parentPtr = cur.parentPtr;
        cur.indexList.erase(cur.indexList.begin()+half,cur.indexList.end());
        cur.ptrList.erase(cur.ptrList.begin()+half+1,cur.ptrList.end());
        newNode.indexList.erase(newNode.indexList.begin());
        for (i = 0; i < newNode.valNum+1; ++i)
        {
            bTreeNode temp = assignNode(newNode.ptrList[i]);
            temp.dirty = true;
            temp.parentPtr = newNode.blockNo;
            freeNode(temp);
        }
        if(cur.type!=ROOT)
        {
            freeNode(newNode);
            freeNode(cur);
            insertNode(newNode.parentPtr,newNode.blockNo,findPath.back(),minInd);
        }
        else
        {
            bTreeNode newRoot = createNode(ROOT);
            cur.type = INNODE;
            cur.parentPtr = newRoot.blockNo;
            newNode.parentPtr = cur.parentPtr;
            newRoot.indexList.push_back(minInd);
            newRoot.ptrList.push_back(cur.blockNo);
            newRoot.ptrList.push_back(newNode.blockNo);
            newRoot.valNum = 1;
            freeNode(newNode);
            freeNode(newRoot);
            freeNode(cur);
            root = newRoot.blockNo;
        }
    }
    else
        freeNode(cur);
    return;
}

Index bTree::findMinIndex(bTreeNode& node){
    Index res;
    if(node.type==LEAF)
        res=node.indexList.front();
    else
    {
        bTreeNode temp = assignNode(node.ptrList.front());
        res = findMinIndex(temp);
        freeNode(temp);
    }
    return res;
}

void bTree::deleteIndex(Index ind){
    unsigned int i;
    bTreeNode cur = findFirstNode(ind);

    for (i = 0; i < cur.valNum; ++i)
    {
        if (ind<=cur.indexList[i] && ind>=cur.indexList[i])
            break; 
    }
    if(i == cur.valNum)
        return;
    cur.dirty = true;
    cur.indexList.erase(cur.indexList.begin()+i);
    cur.ptrList.erase(cur.ptrList.begin()+i);
    cur.valNum--;

    if(cur.type != ROOT && cur.valNum<(cur.maxNum+1)/2)
    {
        bTreeNode left,right;
        unsigned int path = findPath.back();
        bTreeNode temp = findBro(cur);
        if(temp.type == INIT)
        {
            freeNode(cur);
            return;
        }
        if(temp.indexList.front()<cur.indexList.front())
        {
            left = temp;
            right = cur;
        }
        else
        {
            left = cur;
            right = temp;
            path++;
        }
        left.dirty = true;
        right.dirty = true;
        if(left.valNum + right.valNum <= left.maxNum)
        {
            left.ptrList.pop_back();
            unsigned int i;
            for (i = 0; i < right.valNum; ++i)
            {
                left.indexList.push_back(right.indexList[i]);
                left.ptrList.push_back(right.ptrList[i]);
            }
            left.ptrList.push_back(right.ptrList.back());
            left.valNum+=right.valNum;
            freeNode(left);
            cleanNode(right);
            deleteNode(right.parentPtr,path);
        }
        else
        {
            unsigned int half = (left.valNum + right.valNum) / 2;
            unsigned int i;
            for (i = left.valNum; i < half; ++i)
            {
                left.indexList.push_back(right.indexList[i-left.valNum]);
                left.ptrList.insert(left.ptrList.end()-1,right.ptrList.back());
                left.valNum++;
                right.indexList.erase(right.indexList.begin());
                right.ptrList.erase(right.ptrList.begin());
                right.valNum--;
            }
            for (i = left.valNum-1; i >= half; --i)
            {
                right.indexList.insert(right.indexList.begin(),left.indexList.back());
                right.ptrList.insert(right.ptrList.begin(),left.ptrList[left.valNum-1]);
                right.valNum++;
                left.indexList.erase(left.indexList.end()-1);
                left.ptrList.erase(left.ptrList.end()-2);
                left.valNum--;
            }
            bTreeNode temp = assignNode(right.parentPtr);
            temp.dirty = true;
            temp.indexList[path-1] = right.indexList.front();
            freeNode(temp);
            freeNode(left);
            freeNode(right);
        }
    }
    else
        freeNode(cur);

    return;
}

bTreeNode bTree::findBro(bTreeNode& node){
    bTreeNode par = assignNode(node.parentPtr);
    if(findPath.back() > 0 && findPath.back() < par.valNum)
    {
        bTreeNode l = assignNode(par.ptrList[findPath.back()-1]);
        bTreeNode r = assignNode(par.ptrList[findPath.back()+1]);
        freeNode(par);
        if(l.valNum<r.valNum)
        {
            freeNode(r);
            return l;
        }
        else
        {
            freeNode(l);
            return r;
        }
    }
    freeNode(par);
    if(findPath.back() > 0)
        return assignNode(par.ptrList[findPath.back()-1]);
    if(findPath.back() < par.valNum)
        return assignNode(par.ptrList[findPath.back()+1]);
    return bTreeNode();
}

void bTree::deleteNode(unsigned int parent, unsigned int path){
    findPath.pop_back();

    unsigned int i;
    bTreeNode cur = assignNode(parent);

    cur.dirty = true;
    Index backupInd;
    backupInd = cur.indexList.front();
    if (path > 0)
        cur.indexList.erase(cur.indexList.begin()+path-1);
    else
        cur.indexList.erase(cur.indexList.begin()+path);
    cur.ptrList.erase(cur.ptrList.begin()+path);
    cur.valNum--;

    if(cur.type != ROOT && cur.valNum+1<(cur.maxNum+2)/2)
    {
        bTreeNode left,right;
        unsigned int path = findPath.back();
        bTreeNode temp = findBro(cur);
        if(temp.type == INIT)
        {
            freeNode(cur);
            return;
        }
        if(temp.indexList.front()<backupInd)
        {
            left = temp;
            right = cur;
        }
        else
        {
            left = cur;
            right = temp;
            path++;
        }
        left.dirty = true;
        right.dirty = true;
        if(left.valNum + right.valNum + 2 <= left.maxNum + 1)
        {
            left.indexList.push_back(findMinIndex(right));
            unsigned int i;
            for (i = 0; i < right.valNum; ++i)
            {
                left.indexList.push_back(right.indexList[i]);
                left.ptrList.push_back(right.ptrList[i]);
                bTreeNode temp = assignNode(right.ptrList[i]);
                temp.dirty = true;
                temp.parentPtr = left.blockNo;
                freeNode(temp);
            }
            left.ptrList.push_back(right.ptrList.back());
            bTreeNode temp = assignNode(right.ptrList.back());
            temp.dirty = true;
            temp.parentPtr = left.blockNo;
            freeNode(temp);
            left.valNum+=right.valNum+1;
            freeNode(left);
            cleanNode(right);
            deleteNode(right.parentPtr,path);
        }
        else
        {
            unsigned int half = (left.valNum + right.valNum + 2) / 2;
            unsigned int i;
            right.indexList.insert(right.indexList.begin(),findMinIndex(right));
            for (i = left.valNum+1; i < half; ++i)
            {
                left.indexList.push_back(right.indexList.front());
                left.ptrList.push_back(right.ptrList.front());
                left.valNum++;
                right.ptrList.erase(right.ptrList.begin());
                right.indexList.erase(right.indexList.begin());
                right.valNum--;
                bTreeNode temp = assignNode(left.ptrList.back());
                temp.dirty = true;
                temp.parentPtr = left.blockNo;
                freeNode(temp);
            }
            for (i = left.valNum; i >= half; --i)
            {
                right.indexList.insert(right.indexList.begin(),left.indexList.back());
                right.ptrList.insert(right.ptrList.begin(),left.ptrList.back());
                right.valNum++;
                left.ptrList.erase(left.ptrList.end());
                left.indexList.erase(left.indexList.end());
                left.valNum--;
                bTreeNode temp = assignNode(right.ptrList.front());
                temp.dirty = true;
                temp.parentPtr = right.blockNo;
                freeNode(temp);
            }
            right.indexList.erase(right.indexList.begin());
            bTreeNode temp = assignNode(right.parentPtr);
            temp.dirty = true;
            temp.indexList[path-1] = findMinIndex(right);
            freeNode(temp);
            freeNode(left);
            freeNode(right);
        }
    }
    else if(cur.type==ROOT&&cur.valNum==0)
    {
        bTreeNode temp = assignNode(cur.ptrList.front());
        if(temp.type == LEAF)
        {
            freeNode(temp);
            freeNode(cur);
            return;
        }
        temp.dirty = true;
        temp.type = ROOT;
        temp.parentPtr = 0;
        cleanNode(cur);
        freeNode(temp);
        root = temp.blockNo;
    }
    else
        freeNode(cur);
    return;
}

void bTree::cleanNode(bTreeNode& node){
    *(unsigned char*)node.blockPtr = 'E';
    emptyBlock.push_back(node.blockNo);
    node.dirty = false;
    freeNode(node);
    return;
}

void bTree::printTree(){
    bTreeNode r = assignNode(root);
    //std::cout<<r.type<<std::endl;
    std::vector<bTreeNode> temp;
    bfs(r,temp);
    std::vector<bTreeNode>::iterator iter;
    for (iter = temp.begin(); iter < temp.end(); ++iter)
    {
        std::cout<<iter->blockNo<<":";
        iter->testOutput();
        std::cout<<std::endl;
    }
    return;
}

void bTree::bfs(bTreeNode& b,std::vector<bTreeNode>& t){
    t.push_back(b);
    int c=0;
    while(c < t.size())
    {
            //std::cout<<t[c].blockNo<<":";
            //std::cout<<t[c].type<<" ";
                    //t[c].testOutput();
                    //std::cout<<std::endl;
        if(t[c].type!=LEAF)
        {
            int i;
            for (i = 0; i <= t[c].valNum; ++i)
            {
                //if(t[c].ptrList[i] == 0)
                //{
                    //std::cout<<"error: ";
                    //t[c].testOutput();
                //}

                t.push_back(assignNode(t[c].ptrList[i]));
            }
        }
        c++;
    }
    return;
}


//test
bTree::~bTree(){
    //int i;
    //for (i = 0; i < blocks.size(); ++i)
    //{
        //delete[] blocks[i];
        //blocks.clear();
    //}
}

string bTree::getName(){
    return indexName;
}

std::vector<unsigned int> bTree::findLess(Index ind, bool equal){
    unsigned int i;
    std::vector<unsigned int> res;
    bTreeNode cur = assignNode(root);
    while(cur.type != LEAF)
    {
        freeNode(cur);
        cur = assignNode(cur.ptrList.front());
    }

    i=0;
    if(i>=cur.valNum)
    {
        freeNode(cur);
        return res;
    }
    while(cur.indexList[i]!=ind)
    {
        if(equal || cur.indexList[i]!=ind)
            res.push_back(cur.ptrList[i]);
        i++;
        if(i>=cur.valNum)
        {
            if(cur.ptrList[i]==0)
                break;
            freeNode(cur);
            cur = assignNode(cur.ptrList[i]);
            i = 0;
        }
    }
    if(equal)
    while(cur.indexList[i]==ind)
    {
        res.push_back(cur.ptrList[i++]);
        if(i>=cur.valNum)
        {
            if(cur.ptrList[i]==0)
                break;
            freeNode(cur);
            cur = assignNode(cur.ptrList[i]);
            i = 0;
        }
    }
    freeNode(cur);
    return res;
}

std::vector<unsigned int> bTree::findGreater(Index ind, bool equal){
    unsigned int i;
    std::vector<unsigned int> res;
    bTreeNode cur = findFirstNode(ind);

    for (i = 0; i < cur.valNum; ++i)
    {
       if(cur.indexList[i]>=ind) 
           break;
    }
    if(i>=cur.valNum)
    {
        freeNode(cur);
        if(cur.ptrList.back()==0)
            return res;
        cur = assignNode(cur.ptrList.back());
        i = 0;
    }
    while(true)
    {
        if(equal || cur.indexList[i]!=ind)
            res.push_back(cur.ptrList[i]);
        i++;
        if(i>=cur.valNum)
        {
            if(cur.ptrList[i]==0)
                break;
            freeNode(cur);
            cur = assignNode(cur.ptrList[i]);
            i = 0;
        }
    }
    freeNode(cur);
    return res;
}

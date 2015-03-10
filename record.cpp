#include "record.h"
#include "buffer.h"
//#include "API.h"
#include"CatalogManager.h"
#include <iostream>

//宏定义，块大小，逻辑符
#define BLOCK_LEN 4096                  
#define AND 11
#define OR 12
 

extern buffer bm;
extern CatalogManager catalogM;

//extern API ap;

int record::deleteValue(string tableName)
{
	fileInfo  *fp = bm.getFile(tableName.c_str());       //get file information
  
	if (fp != NULL)
	{
		int  bp = fp->firstBlock; //first block position
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName);
			fp->firstBlock = bp;
		}
		while (bp != -1)
		{

			bm.block[bp].lrutime++;
			memset(bm.block[bp].address, 0, BLOCK_LEN);
			bm.block[bp].usage = 0;
		
			if (bm.block[bp].nextBlock == -1)
			{
				bp = bm.findnextblock(tableName);
			}
			else
			{
				bp = bm.block[bp].nextBlock;
			}
		}
	}
	return catalogM.getRecordNum(tableName);
	//return 0;
}


int record::deleteValue(string tableName, vector<string> colName, vector<string> cond, vector<string> operater)
{
	int deleteNum = 0;
	for (int i = 0; i < colName.size(); i++)
	{
		string colName1 = colName[i];
		string cond1 = cond[i];
		string operater1 = operater[i];

		fileInfo * fp = bm.getFile(tableName.c_str());
		

		if (fp != NULL)
		{
			vector<string> collName;
			vector<string> collType;

			//得到表的属性列表和属性类型列表
			collName = catalogM.getCollName(tableName);
			collType = catalogM.getCollType(tableName);
			char * p;
			char * tmpp;
			char value[255];
			string type;
			int typeLen;
			int recordLen;
			//计算这个表中的记录长度
			recordLen = catalogM.getrecordLength(tableName);


			int  bp = fp->firstBlock;
			if (bp == NULL)
			{
				bp = bm.findnextblock(tableName.c_str());
				fp->firstBlock = bp;
			}

			while (bp != -1)
			{

				bm.block[bp].lrutime++;
				p = bm.block[bp].address;

				//if p is empty then it is empty
				while (p - bm.block[bp].address < bm.block[bp].usage)
				{
					tmpp = p;    //用tmpp记录每条记录的开始位置
					//对记录中的每个字段进行遍历
					for (unsigned int j = 0; j < collName.size(); j++)
					{
						type = collType.at(j);
						typeLen = catalogM.getLength(type);
						//找到对应的比较字段，提取其值
						if (collName.at(j) == colName1)
						{
							memcpy(value, p, typeLen);
							break;
						}
						p = p + typeLen;
					}
					//如果字段的值满足比较条件，则将这个字段删除,并将块的usage减去记录长度,同时将p指针指到下一条记录，
					//也就是删除前原来记录的位置
					if (fullFillCond(type, value, cond1, operater1))
					{
						bm.block[bp].usage = bm.block[bp].usage - recordLen;
						//move back to front
						char * ip;
						for (ip = tmpp; ip != bm.block[bp].address + bm.block[bp].usage; ip++)
							*ip = *(ip + recordLen);
						for (int i = 0; i < recordLen; i++, ip++)
							*ip = 0;
						//relocated
						p = tmpp;
						deleteNum++;
						bm.block[bp].dirty = 1;
					}
					//move to next len
					else
						p = tmpp + recordLen;
				}

				if (bm.block[bp].nextBlock == -1)
					bp = bm.findnextblock(tableName.c_str());
				else
					bp = bm.block[bp].nextBlock;
			}
		}
	}
	return deleteNum;
}




int record::selectRecord(string tableName)
{
	fileInfo * fp = bm.getFile(tableName.c_str());

	if (fp != NULL)
	{
		int recordLen = catalogM.getrecordLength(tableName);
		vector<string> collName = catalogM.getCollName(tableName);
		vector<string> collType = catalogM.getCollType(tableName);
		char * p;
		char value[255];
		memset(value, 0, 255);  //set zero of value[]
		int valueLen;
		string type;

		for (unsigned int i = 0; i<collName.size(); i++)
			cout << collName.at(i) << " ";
		cout << endl;

		//if no block under the file
		int bp = fp->firstBlock;
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName.c_str());
			fp->firstBlock = bp;
		}
		while (1)
		{
			if (bp == -1)
				break;
			bm.block[bp].lrutime++;
			p = bm.block[bp].address;

			while (p - bm.block[bp].address<	bm.block[bp].usage)
			{
				for (unsigned int j = 0; j<collName.size(); j++)
				{
					type = collType.at(j);
					valueLen = catalogM.getLength(type);
					memcpy(value, p, valueLen);
					p = p + valueLen;

					//按照对应的类型，将字段的值输出
					if (type == "int")
					{
						int * x;
						x = (int *)value;
						cout << (*x) << " ";
					}
					else if (type == "float")
					{
						float * x;
						x = (float *)value;
						cout << (*x) << " ";
					}
					else
						cout << value << " ";
				}
				cout << endl;
			}

			if (bm.block[bp].nextBlock == -1)
			{
				bp = bm.findnextblock(tableName.c_str());
			}
			else
				bp = bm.block[bp].nextBlock;
		}
		return catalogM.getRecordNum(tableName);
		return 5;
	}
	else
		return 0;
}


//通过一个给定的文件名和一个where条件来查找表中的记录。
int record::selectRecord(string tableName, vector<string> colName, vector<string> cond, vector<string> operater)
{
	int selectNum = 0;
	for (int i = 0; i < colName.size(); i++)
	{
		string colName1 = colName[i];
		string cond1 = cond[i];
		string operater1 = operater[i];
		fileInfo * fp = bm.getFile(tableName.c_str());

		if (fp != NULL)
		{

			int recordLen = catalogM.getrecordLength(tableName);
			vector<string> collName = catalogM.getCollName(tableName);
			vector<string> collType = catalogM.getCollType(tableName);
			char * p;
			char * tmp;
			int typeLen = 0;
			string type;
			char value[255];
			memset(value, 0, 255);

			//print out name
			for (unsigned int i = 0; i < collName.size(); i++)
				cout << collName.at(i) << " ";
			cout << endl;

			int  bp = fp->firstBlock;
			if (bp == -1)
			{
				bp = bm.findnextblock(tableName.c_str());
				fp->firstBlock = bp;
			}
			while (1)
			{
				if (bp == -1)
					break;
				bm.block[bp].lrutime++;  //块被使用一次

				p = bm.block[bp].address;

				while ((int)(*p) != 0)
				{
					tmp = p;

					//在记录字节中找到对应字段的值
					for (unsigned int j = 0; j < collName.size(); j++)
					{
						type = collType.at(j);
						typeLen = catalogM.getLength(type);
						if (collName.at(j) == colName1)
						{
							memcpy(value, p, typeLen);
							break;
						}
						p = p + typeLen;
					}

					//如果不满足条件，则将指针指向下一条记录
					if (!fullFillCond(type, value, cond1, operater1))
					{
						p = tmp + recordLen;
						continue;
					}
					//如果满足条件，则将这条件记录输出				
					selectNum++;                              //将块的lru加一
					p = tmp;                                    //重新指回记录的起始位置
					for (unsigned int j = 0; j < collName.size(); j++)
					{
						type = collType.at(j);
						typeLen = catalogM.getLength(type);
						memcpy(value, p, typeLen);
						p = p + typeLen;

						//将字节转化为对应的类型值进行输出
						if (collType.at(j) == "int")
						{
							int * x;
							x = (int *)value;
							cout << (*x) << " ";
						}
						else if (collType.at(j) == "float")
						{
							float * x;
							x = (float *)value;
							cout << (*x) << " ";
						}
						else
							cout << value << " ";
					}
					cout << endl;
				}
				if (bm.block[bp].nextBlock == -1)
					bp = bm.findnextblock(tableName.c_str());
				else
					bp = bm.block[bp].nextBlock;
			}
			return selectNum;
		}
	}
	return 0;
}



int record::insertRecord(string tableName, const char * s)
{

	//get file information
	fileInfo * fp = bm.getFile(tableName.c_str());

	int recordLen = catalogM.getrecordLength(tableName);
	if (fp != NULL)
	{
		int  bp = fp->firstBlock;

		//得到文件对应的第一个块

		//如果文件节点中没有块链表，则读取第一个块，同时将这个块挂到文件节点下面
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName.c_str());
			bm.block[bp]=bm.block[bp];
			fp->firstBlock = bp;
		}

		//如果是非空文件
		if (bp != -1)
		{
			while (1)
			{
				//对使用的块的lru增加1
				bm.block[bp].lrutime++;
				//如果文件块对应的块中还没有写满，则将记录写入这个块尾，即插入到文件中间，而不是末尾
				if (bm.block[bp].usage <= BLOCK_LEN - recordLen)
				{
					char * p;
					p = bm.block[bp].address;
					p = p + bm.block[bp].usage;
					memcpy(p, s, recordLen);
					bm.block[bp].usage = bm.block[bp].usage + recordLen;
					bm.block[bp].dirty = 1;
					bm.block[bp].lrutime++;
					cout << "Add a record to table" << endl;
					return 1;
				}

				//如果链表中的块遍历完，则要得到替换新的块放入链表,这个所谓新的块是在文件中有对就段的
				//如果链表中的块还没遍历完，直接指向链表中的下一个块
				if (bm.block[bp].nextBlock == -1)
				{
					int tmp;
					if ((tmp = bm.findnextblock(tableName.c_str())) != -1)
						bp = tmp;
					else
					{
						bp = bm.addNewBlock(tableName.c_str());
					}
						
				}
				else
					bp = bm.block[bp].nextBlock;
			}
		}

		//如果是个空文件，刚添加一个新的空块，并将它挂到文件节点下面
		if (bp == -1)
		{
			bp = bm.addNewBlock(tableName.c_str());
			fp->firstBlock = bp;
		}
		//还没有将记录插入到各个文件段对应的块中，则查找一个空块，并将记录放入空块，也就是将记录插入文件末尾

			//bp = bm.findnextblock(tableName.c_str());
			memcpy(bm.block[bp].address, s, recordLen);
			bm.block[bp].usage = bm.block[bp].usage + recordLen;
			bm.block[bp].lrutime++;
			bm.block[bp].dirty = 1;
		//	bm.freeBlock("testfile2", bp, 1);
			cout << "Add a record to table" << endl;
			return 1;
	}
	else
	{
		cout << "Fail to find file block of " << tableName << endl;
		return 0;
	}
}



//判断某个值在比较符operater下是否符合条件cond,符合返回1，不符合返回0
int record::fullFillCond(string type, char * value, string scond, string operater)
{
	int tmp;
	//比较value 和 cond，将value 和 cond的差记为tmp
	const char * cond;
	cond = scond.c_str();
	if (type == "int")
	{
		int * v;
		v = (int *)value;
		int cv;
		cv = atoi(cond);
		tmp = (*v) - cv;
	}

	else if (type == "float")
	{
		float * v;
		v = (float *)value;
		float cv;
		cv = (float)atof(cond);


		if ((*v)<cv)
			tmp = -1;
		else if ((*v) == cv)
			tmp = 0;
		else
			tmp = 1;
	}
	else
	{
		tmp = strcmp(value, cond);
	}
	//cout << operater<<endl;
	//通过不同的operater和两者的差，来最后确定是否满足条件
	if (operater == "<")
	{
		if (tmp<0)
			return 1;
		else
			return 0;
	}
	else if (operater == "<=")
	{
		if (tmp <= 0)
			return 1;
		else
			return 0;
	}
	else if (operater == ">")
	{
		if (tmp>0)
			return 1;
		else
			return 0;
	}
	else if (operater == ">=")
	{
		if (tmp >= 0)
			return 1;
		else
			return 0;
	}
	else if (operater == "=")
	{
		if (tmp == 0)
			return 1;
		else
			return 0;
	}

	//提示比较出错
	else
	{
		cout << "Fail to compare values" << endl;
		return -1;
	}
}
vector<int> record::selectInt(string tableName, string colName)
{
	vector<int> aim;

	fileInfo * fp = bm.getFile(tableName.c_str());

	if (fp != NULL)
	{
		int recordLen = catalogM.getrecordLength(tableName);
		vector<string> collName = catalogM.getCollName(tableName);
		vector<string> collType = catalogM.getCollType(tableName);
		char * p;
		char value[255];
		memset(value, 0, 255);  //set zero of value[]
		int valueLen;
		string type;


		//if no block under the file
		int bp = fp->firstBlock;
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName.c_str());
			fp->firstBlock = bp;
		}
		while (1)
		{
			if (bp == -1)
				break;
			bm.block[bp].lrutime++;
			p = bm.block[bp].address;

			while (p - bm.block[bp].address<	bm.block[bp].usage)
			{
				for (unsigned int j = 0; j<collName.size(); j++)
				{

					type = collType.at(j);
					valueLen = catalogM.getLength(type);
					memcpy(value, p, valueLen);
					p = p + valueLen;
					int * x;
					x = (int *)value;
					if (collName[j] != colName) continue;
					aim.push_back(*x);
				}
			}

			if (bm.block[bp].nextBlock == -1)
			{
				bp = bm.findnextblock(tableName.c_str());
			}
			else
				bp = bm.block[bp].nextBlock;
		}
		return aim;
	}
	else
		return aim;
}


vector<float> record::selectFloat(string tableName, string colName)
{
	vector<float> aim;

	fileInfo * fp = bm.getFile(tableName.c_str());

	if (fp != NULL)
	{
		int recordLen = catalogM.getrecordLength(tableName);
		vector<string> collName = catalogM.getCollName(tableName);
		vector<string> collType = catalogM.getCollType(tableName);
		char * p;
		char value[255];
		memset(value, 0, 255);  //set zero of value[]
		int valueLen;
		string type;


		//if no block under the file
		int bp = fp->firstBlock;
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName.c_str());
			fp->firstBlock = bp;
		}
		while (1)
		{
			if (bp == -1)
				break;
			bm.block[bp].lrutime++;
			p = bm.block[bp].address;

			while (p - bm.block[bp].address<	bm.block[bp].usage)
			{
				for (unsigned int j = 0; j<collName.size(); j++)
				{

					type = collType.at(j);
					valueLen = catalogM.getLength(type);
					memcpy(value, p, valueLen);
					p = p + valueLen;
					if (collName[j] != colName) continue;
					float * x;
					x = (float *)value;
					aim.push_back(*x);
				}
			}

			if (bm.block[bp].nextBlock == -1)
			{
				bp = bm.findnextblock(tableName.c_str());
			}
			else
				bp = bm.block[bp].nextBlock;
		}
		return aim;
	}
	else
		return aim;
}

vector<string> record::selectString(string tableName, string colName)
{
	vector<string> aim;

	fileInfo * fp = bm.getFile(tableName.c_str());

	if (fp != NULL)
	{
		int recordLen = catalogM.getrecordLength(tableName);
		vector<string> collName = catalogM.getCollName(tableName);
		vector<string> collType = catalogM.getCollType(tableName);
		char * p;
		char value[255];
		memset(value, 0, 255);  //set zero of value[]
		int valueLen;
		string type;


		//if no block under the file
		int bp = fp->firstBlock;
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName.c_str());
			fp->firstBlock = bp;
		}
		while (1)
		{
			if (bp == -1)
				break;
			bm.block[bp].lrutime++;
			p = bm.block[bp].address;

			while (p - bm.block[bp].address<	bm.block[bp].usage)
			{
				for (unsigned int j = 0; j<collName.size(); j++)
				{

					type = collType.at(j);
					valueLen = catalogM.getLength(type);
					memcpy(value, p, valueLen);
					p = p + valueLen;
					if (collName.at(j) != colName) continue;
					aim.push_back(value);
				}
			}

			if (bm.block[bp].nextBlock == -1)
			{
				bp = bm.findnextblock(tableName.c_str());
			}
			else
				bp = bm.block[bp].nextBlock;
		}
		return aim;
	}
	else
		return aim;
}








/*
record rm;
int main()
{
	bm.createNewFile("student3");
	
	bm.block[1] = bm.block[1];
	for (int i = 1; i <= 10;i++)
	rm.insertRecord("student3", "asdfsfdsdfasdfsaf");

	rm.selectRecord("student3");

	cout << bm.getNumberOfBlocks("student3");

	bm.deleteFile("student3");

	system("pause");
}*/







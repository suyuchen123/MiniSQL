#include "record.h"
#include "buffer.h"
//#include "API.h"
#include"CatalogManager.h"
#include <iostream>

//�궨�壬���С���߼���
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

			//�õ���������б�����������б�
			collName = catalogM.getCollName(tableName);
			collType = catalogM.getCollType(tableName);
			char * p;
			char * tmpp;
			char value[255];
			string type;
			int typeLen;
			int recordLen;
			//����������еļ�¼����
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
					tmpp = p;    //��tmpp��¼ÿ����¼�Ŀ�ʼλ��
					//�Լ�¼�е�ÿ���ֶν��б���
					for (unsigned int j = 0; j < collName.size(); j++)
					{
						type = collType.at(j);
						typeLen = catalogM.getLength(type);
						//�ҵ���Ӧ�ıȽ��ֶΣ���ȡ��ֵ
						if (collName.at(j) == colName1)
						{
							memcpy(value, p, typeLen);
							break;
						}
						p = p + typeLen;
					}
					//����ֶε�ֵ����Ƚ�������������ֶ�ɾ��,�������usage��ȥ��¼����,ͬʱ��pָ��ָ����һ����¼��
					//Ҳ����ɾ��ǰԭ����¼��λ��
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

					//���ն�Ӧ�����ͣ����ֶε�ֵ���
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


//ͨ��һ���������ļ�����һ��where���������ұ��еļ�¼��
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
				bm.block[bp].lrutime++;  //�鱻ʹ��һ��

				p = bm.block[bp].address;

				while ((int)(*p) != 0)
				{
					tmp = p;

					//�ڼ�¼�ֽ����ҵ���Ӧ�ֶε�ֵ
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

					//�����������������ָ��ָ����һ����¼
					if (!fullFillCond(type, value, cond1, operater1))
					{
						p = tmp + recordLen;
						continue;
					}
					//�����������������������¼���				
					selectNum++;                              //�����lru��һ
					p = tmp;                                    //����ָ�ؼ�¼����ʼλ��
					for (unsigned int j = 0; j < collName.size(); j++)
					{
						type = collType.at(j);
						typeLen = catalogM.getLength(type);
						memcpy(value, p, typeLen);
						p = p + typeLen;

						//���ֽ�ת��Ϊ��Ӧ������ֵ�������
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

		//�õ��ļ���Ӧ�ĵ�һ����

		//����ļ��ڵ���û�п��������ȡ��һ���飬ͬʱ�������ҵ��ļ��ڵ�����
		if (bp == -1)
		{
			bp = bm.findnextblock(tableName.c_str());
			bm.block[bp]=bm.block[bp];
			fp->firstBlock = bp;
		}

		//����Ƿǿ��ļ�
		if (bp != -1)
		{
			while (1)
			{
				//��ʹ�õĿ��lru����1
				bm.block[bp].lrutime++;
				//����ļ����Ӧ�Ŀ��л�û��д�����򽫼�¼д�������β�������뵽�ļ��м䣬������ĩβ
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

				//��������еĿ�����꣬��Ҫ�õ��滻�µĿ��������,�����ν�µĿ������ļ����жԾͶε�
				//��������еĿ黹û�����ֱ꣬��ָ�������е���һ����
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

		//����Ǹ����ļ��������һ���µĿտ飬�������ҵ��ļ��ڵ�����
		if (bp == -1)
		{
			bp = bm.addNewBlock(tableName.c_str());
			fp->firstBlock = bp;
		}
		//��û�н���¼���뵽�����ļ��ζ�Ӧ�Ŀ��У������һ���տ飬������¼����տ飬Ҳ���ǽ���¼�����ļ�ĩβ

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



//�ж�ĳ��ֵ�ڱȽϷ�operater���Ƿ��������cond,���Ϸ���1�������Ϸ���0
int record::fullFillCond(string type, char * value, string scond, string operater)
{
	int tmp;
	//�Ƚ�value �� cond����value �� cond�Ĳ��Ϊtmp
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
	//ͨ����ͬ��operater�����ߵĲ�����ȷ���Ƿ���������
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

	//��ʾ�Ƚϳ���
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







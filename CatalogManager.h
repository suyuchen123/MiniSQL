#ifndef _CatalogManager_H_
#define _CatalogManager_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class index
{
public:
	string indexName;              //���������
	string tableName;            //���������Ӧ�ı���
	string colName;              //���������Ӧ������
};

class col
{
public:
	string colName;        //�洢����
	string colType;        //�洢�е�����
	int colLength;         //�洢�еĳ���
	string colUnique;         //�������Ƿ�unique
	string colIndex;          //�����е�����
};

class table
{
public:
	string tableName;               //��ű���
	int recordNum;                  //��ż�¼���� 
	int colNum;                    //�����е���Ŀ==colnum.size()
	string primaryKey;                 //�������
	vector<col> column;            //��Ÿ����е���Ϣ
};

class CatalogManager
{
public:
	vector<index> Index_c;
	vector<table> Table;
	int IndexNum;
	int TableNum;
	vector<int> rei;
	vector<float> ref;
	vector<string> res;
public:
	CatalogManager();
	~CatalogManager();
	int getLength(string type);  //���ã�����ĳ���������ͣ���������������͵ĳ���
	int gettableLength(string tableName); //���ݱ����������ļ��еļ�¼����
	int getrecordLength(string tableName);//���ݱ����������ļ��еĵ�����¼����
	bool check_colName(int i, string c);
	bool CreateTable(string tableName, vector<string> col_name, vector<string> col_type, vector<int> col_unique, string primaryKey);
	bool CreateIndex(string indexName, string tableName, string colName);//�ж��Ƿ���Խ�������
	bool DropTable(string tableName);
	bool DropIndex(string indeNname);
	bool SelectValue(string tableName);
	bool SelectValue(string tableName, vector<string> colName, vector<string> op, vector<string> value);
	bool DeleteValue(string tableName);
	bool DeleteValue(string tableName, vector<string> colName, vector<string> op, vector<string> value, int deleteNum);
	bool InsertValue(string tableName, vector<string> value);
	void getfileinfo(string fileName, int *n);
	void selectRecord(string tableName, string colName);
	int getRecordNum(string tableName); //���ã������ļ�������������ļ��ļ�¼��Ŀ��
	vector<string> getCollName(string tableName); //���ã������ļ������б�
	vector<string> getCollType(string tableName); //���ã������ļ����������б�
};
#endif
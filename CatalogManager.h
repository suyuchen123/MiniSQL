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
	string indexName;              //存放索引名
	string tableName;            //存放索引对应的表名
	string colName;              //存放索引对应的列名
};

class col
{
public:
	string colName;        //存储列名
	string colType;        //存储列的类型
	int colLength;         //存储列的长度
	string colUnique;         //存数列是否unique
	string colIndex;          //存数列的索引
};

class table
{
public:
	string tableName;               //存放表名
	int recordNum;                  //存放记录条数 
	int colNum;                    //存入列的数目==colnum.size()
	string primaryKey;                 //存放主键
	vector<col> column;            //存放各各列的信息
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
	int getLength(string type);  //作用：根据某个属性类型，返回这个属性类型的长度
	int gettableLength(string tableName); //根据表名，返回文件中的记录长度
	int getrecordLength(string tableName);//根据表名，返回文件中的单条记录长度
	bool check_colName(int i, string c);
	bool CreateTable(string tableName, vector<string> col_name, vector<string> col_type, vector<int> col_unique, string primaryKey);
	bool CreateIndex(string indexName, string tableName, string colName);//判断是否可以建立索引
	bool DropTable(string tableName);
	bool DropIndex(string indeNname);
	bool SelectValue(string tableName);
	bool SelectValue(string tableName, vector<string> colName, vector<string> op, vector<string> value);
	bool DeleteValue(string tableName);
	bool DeleteValue(string tableName, vector<string> colName, vector<string> op, vector<string> value, int deleteNum);
	bool InsertValue(string tableName, vector<string> value);
	void getfileinfo(string fileName, int *n);
	void selectRecord(string tableName, string colName);
	int getRecordNum(string tableName); //作用：查找文件，返回在这个文件的记录数目。
	vector<string> getCollName(string tableName); //作用：返回文件属性列表
	vector<string> getCollType(string tableName); //作用：返回文件属性类型列表
};
#endif
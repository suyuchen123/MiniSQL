#include "CatalogManager.h"
#include "record.h"
extern record recordM;
CatalogManager::CatalogManager()
{
	fstream  ftable("table.catlog", ios::in);
	fstream  findex("index.catlog", ios::in);
	int i;
	if (!(ftable&&findex))
	{
		cout << "Fail to open the catlog file when read dictionary information" << endl;
		exit(0);
	}
	ftable >> TableNum;
	findex >> IndexNum;

	index exam_index;
	col exam_col;
	for (i = 0; i < TableNum; i++)
	{
		table exam_table;
		ftable >> exam_table.tableName; //读取表名
		ftable >> exam_table.colNum;//读取列的数目
		ftable >> exam_table.recordNum;
		ftable >> exam_table.primaryKey;//读取主键
		for (int j = 0; j < exam_table.colNum; j++)
		{
			col exam_col;
			ftable >> exam_col.colName;//读取列名
			ftable >> exam_col.colType;//读取列数据类型
			ftable >> exam_col.colLength;//读取列数据长度
			ftable >> exam_col.colUnique;//读取是否unique
			ftable >> exam_col.colIndex;//读取是否unique
			exam_table.column.push_back(exam_col);
		}
		Table.push_back(exam_table);
	}
	for (int i = 0; i < IndexNum; i++)
	{//fill in the vector of Index
		index exam_index;
		findex >> exam_index.indexName;
		findex >> exam_index.tableName;
		findex >> exam_index.colName;
		Index_c.push_back(exam_index);
	}
	ftable.close();
	findex.close();
}

CatalogManager::~CatalogManager()
{
	fstream  ftable("table.catlog", ios::out);
	fstream  findex("index.catlog", ios::out);
	int i;
	if (!(ftable&&findex))
	{
		cout << "Fail to open the catlog file when read dictionary information" << endl;
		exit(0);
	}
	ftable << TableNum << endl << endl;
	findex << IndexNum << endl << endl;
	for (i = 0; i < TableNum; i++)
	{
		ftable << Table[i].tableName << " ";
		ftable << Table[i].colNum << " ";
		ftable << Table[i].recordNum << " ";
		ftable << Table[i].primaryKey << endl;

		for (int j = 0; j < Table[i].colNum; j++)
		{
			ftable << Table[i].column[j].colName << " ";
			ftable << Table[i].column[j].colType << " ";
			ftable << Table[i].column[j].colLength << " ";
			ftable << Table[i].column[j].colUnique << " ";
			ftable << Table[i].column[j].colIndex << " " << endl;
		}
		ftable << endl;
	}
	for (int i = 0; i < IndexNum; i++)
	{
		findex << Index_c[i].indexName << " ";
		findex << Index_c[i].tableName << " ";
		findex << Index_c[i].colName << " "<<endl;
	}
	ftable.close();
	findex.close();
}

int CatalogManager::getLength(string type)
{
	int tmp;
	string s;
	if (type=="int")           //如果是整型的字段类型
	{
		tmp = sizeof(int);
	}
	else if (type[0] == 'c' && type[1] == 'h' &&type[2] == 'a' &&type[3] == 'r')     //如果是char型的字段类型,则要返回()中的char型长度
	{
		char p[5];
		int n;
		if (type[7] == ')')
		{
			p[0] = type[5];
			p[1] = type[6];
			p[2] = 0;
		}
		else if (type[8] == ')')
		{
			p[0] = type[5];
			p[1] = type[6];
			p[2] = type[7];
			p[3] = 0;
		}
		n = atoi(p);
		tmp = n*sizeof(char);
	}
	else if (type == "float")      //如果是float型字段类型
	{
		tmp = sizeof(float);
	}
	else
	{
		cout << "Fail to calcute record length,undefined type found" << endl;
		return -1;
	}
	return tmp;
}

int CatalogManager::gettableLength(string tableName)
{
	unsigned int j;
	int i;
	int n;
	int onerecord=0;
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tableName)
			break;
	}
	if (i == TableNum) return -1;

	for (j = 0; j < Table[i].column.size(); j++)
	{
		onerecord = onerecord + Table[i].column[j].colLength;
	}

	n = Table[i].recordNum * onerecord;
	return n;
}

int CatalogManager::getrecordLength(string tableName)
{
	unsigned int j;
	int i;
	int onerecord = 0;
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tableName)
			break;
	}
	if (i == TableNum) return -1;

	for (j = 0; j < Table[i].column.size(); j++)
	{
		onerecord = onerecord + Table[i].column[j].colLength;
	}
	return onerecord;
}

bool CatalogManager::check_colName(int i, string c)
{
	int j;
	bool flag = true;
	for (j = 0; j < Table[i].colNum; j++)
	{
		if (Table[i].column[j].colName == c)
		{
			break;
		}
	}
	if (j == Table[i].colNum)
		flag = false;
	return flag;

}

void CatalogManager::getfileinfo(string fileName, int *n)
{
	int i = 0;
	n[0] = 8;
	for (i = 0; i<TableNum; i++)
	{
		if (Table[i].tableName == fileName)
			n[0] = 9;
	}
	for (i = 0; i<IndexNum; i++)
	{
		if (Index_c[i].indexName == fileName)
			n[0] = 10;
	}
	for (unsigned int i = 0; i<Table.size(); i++)
	{
		if (Table[i].tableName == fileName)
		{
			n[0] = 8;
			n[1] = Table[i].recordNum;
			n[2] = getrecordLength(Table[i].tableName);
		}
	}
}

bool CatalogManager::CreateTable(string tableName, vector<string> col_name, vector<string> col_type, vector<int> col_unique, string primaryKey)
{	
	unsigned int i;
	if (primaryKey == "")
		primaryKey = "no_primaryKey";
	for (i = 0; i < Table.size(); i++)
	if (Table[i].tableName == tableName)
	{
		cout << "Error: The table has already exist! Please check again!" << endl;
		return false;
	}
	TableNum++;
	table t;       //创建一个表的struct
	t.tableName = tableName;         //把提供的表信息放入这个struct
	t.recordNum = 0;
	t.colNum = col_name.size();
	t.primaryKey = primaryKey;
	for (i = 0; i < col_name.size(); i++)
	{
		col Col;
		Col.colName = col_name[i];
		Col.colType = col_type[i];
		Col.colLength = getLength(col_type[i]);
		if (col_unique[i] == 1)
			Col.colUnique = "unique";
		else
			Col.colUnique = "not_unique";
		Col.colIndex = "no_index";
		t.column.push_back(Col);
	}
	Table.push_back(t);

	//创建表文件
	ofstream outfile(tableName);
	if (!outfile)
	{
		cout << "Fail to create file" << endl;
		return false;
	}
	outfile.close();

	return true;
}

bool CatalogManager::CreateIndex(string indexName, string tableName, string colName)
{
	unsigned int i;
	int j;
	bool flag = true;
	for (i = 0; i < Index_c.size(); i++)
	if (Index_c[i].indexName == indexName)
	{
		cout << "Error: The index has already exist! Please check again!" << endl;
		flag= false;
	}
	for (i = 0; i < Table.size(); i++)
	{
		if (Table[i].tableName == tableName)
		{
			if (Table[i].recordNum == 0)
			{
				cout << "Fail to create index on a empty table!" << endl;
				return false;
			}
			for (j = 0; j<Table[i].colNum; j++)
			{
				if (Table[i].column[j].colName == colName && Table[i].column[j].colIndex=="no_index")
				{
					index ii;
					Table[i].column[j].colIndex = indexName;
					ii.indexName = indexName;
					ii.tableName = tableName;
					ii.colName = colName;
					Index_c.push_back(ii);
					IndexNum++;
					break;
				}
				else if (Table[i].column[j].colName == colName)
				{
					cout << "Error: There already have index on the col!" << endl;
					flag = false;
				}
			}
			if (j == Table[i].colNum)
			{
				cout << "Fail to create index, coll " << colName << " does not exist" << endl;
				flag=false;
			}
			break;
		}
	}
	if (i==Table.size())
	{
		cout << "Fail to create index, table " << tableName << " does not exist" << endl;
		flag=false;
	}

	//创建索引文件
	ofstream outfile(indexName);
	if (!outfile)
	{
		cout << "Fail to create file" << endl;
		return 0;
	}
	outfile.close();
	return flag;
}

bool CatalogManager::DropTable(string tableName)
{                                //将表数目减一
	int i, j;
	
	for (i = TableNum - 1; i >=0; i--)
	{
		if (Table[i].tableName == tableName)
		{
			Table.erase(Table.begin() + i);
			TableNum--;
			break;
		}
	}
	if (i == -1)                //如果文件不存在，则提示出错信息
	{
		cout << "Table " << tableName << " does not exists" << endl;
		return false;
	}
	for (j = IndexNum - 1; j >= 0; j--)
	{
		if (Index_c[j].tableName == tableName)
		{
			Index_c.erase(Index_c.begin() + j);
			IndexNum--;
			//删除该表上的索引
		}
	}
	remove(tableName.c_str());
	return true;

}

bool CatalogManager::DropIndex(string indexName)
{
	int i,j;
	string tName, cName;
	for (i = 0; i < IndexNum; i++)
	{
		if (Index_c[i].indexName == indexName)
		{
			tName = Index_c[i].tableName;
			cName = Index_c[i].colName;
			break;
		} 
	}
	if (i == IndexNum)                //如果文件不存在，则提示出错信息
	{
		cout << "Index " << indexName << " does not exists" << endl;
		return false;
	}
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tName)
		{
			for (j = 0; j < Table[i].colNum; j++)
			{
				if (Table[i].column[j].colName == cName)
				{
					Table[i].column[j].colIndex = "no_index";
					break;
				}
			}
			break;
		}
	}

	for (i = IndexNum - 1; i >= 0; i--)
	{
		if (Index_c[i].indexName == indexName)
		{
			Index_c.erase(Index_c.begin() + i);
			IndexNum--;
			break;
		}
	}
	
	remove(indexName.c_str());
	return true;
}

bool CatalogManager::SelectValue(string tableName)
{
	unsigned int i;
	bool flag = true;
	for (i = 0; i < Table.size(); i++)
	{
		if (Table[i].tableName == tableName)
		{
			break;
		}
	}
	if (i == Table.size())
	{
		cout << "Table " << tableName << " does not exists" << endl;
		flag= false;
	}
	return flag;
}

bool CatalogManager::SelectValue(string tableName, vector<string> colName, vector<string> op, vector<string> value)
{
	unsigned int i, j;
	int flag = 0;
	for (i = 0; i < Table.size(); i++)
	{
		if (Table[i].tableName == tableName)
		{
			for (j = 0; j < colName.size(); j++)
			{
				if (check_colName(i, colName[j]) == false)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 1)
			{
				cout << "Error: The " << colName[j] << "is not exist in the table " << tableName <<" !"<< endl;
				return false;
			}
		}
	}
	if (i == Table.size())
	{
		cout << "Table " << tableName << " does not exists" << endl;
		return false;
	}
	return true;
}

bool CatalogManager::DeleteValue(string tableName)
{
	unsigned int i;
	for (i = 0; i < Table.size(); i++)
	{
		if (Table[i].tableName == tableName)
		{
			Table[i].recordNum = 0;
			break;
		}
	}
	if (i == Table.size())
	{
		cout << "Table " << tableName << " does not exists" << endl;
		return false;
	}
	return true;
}

bool CatalogManager::DeleteValue(string tableName, vector<string> colName, vector<string> op, vector<string> value, int deleteNum)
{
	unsigned int i, j;
	int flag = 0;
	for (i = 0; i < Table.size(); i++)
	{
		if (Table[i].tableName == tableName)
		{
			for (j = 0; j < colName.size(); j++)
			{
				if (check_colName(i, colName[j]) == false)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 1)
			{
				cout << "Error: The " << colName[j] << "is not exist in the table " << tableName << " !" << endl;
				return false;
			}
			Table[i].recordNum = Table[i].recordNum - deleteNum;
			break;
		}
	}
	if (i == Table.size())
	{
		cout << "Table " << tableName << " does not exists" << endl;
		return false;
	}
	return true;
}

bool CatalogManager::InsertValue(string tableName, vector<string> value)
{
	unsigned int i;
	for (i = 0; i < Table.size(); i++)
	{
		if (Table[i].tableName == tableName && value.size()==Table[i].colNum)
		{
			Table[i].recordNum++;
			break;
		}
		else if (Table[i].tableName == tableName)
		{
			cout << "The number of data input does not equal to the col number" << endl;
			return false;
		}
	}
	if (i == Table.size())
	{
		cout << "Table " << tableName << " does not exists" << endl;
		return false;
	}
	return true;
}

void CatalogManager::selectRecord(string tableName, string colName)
{
	int i,j;
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tableName)
			break;
	}
	if (i == TableNum)
	{
		cout << "Error: Fail to create index, table " << tableName << " does not exist" << endl;
		return;
	}
	for (j = 0; j < Table[i].colNum; j++)
	{
		if (Table[i].column[j].colName == colName)
			break;
	}
	if (j == Table[i].colNum)
	{
		cout << "Error: Fail to create index, col " << colName << " does not exist" << endl;
		return;
	}

	if (Table[i].column[j].colType == "int")
	{
		rei = recordM.selectInt(tableName, colName);
	}
	else if (Table[i].column[j].colType == "float")
	{
		ref = recordM.selectFloat(tableName, colName);
	}
	else
	{
		res=recordM.selectString(tableName, colName);
	}
}

int CatalogManager::getRecordNum(string tableName)
{
	int i;
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tableName)
			break;
	}
	if (i == TableNum)
	{
		cout << "Error: Fail to create index, table " << tableName << " does not exist" << endl;
		return -1;
	}
	return Table[i].colNum;
}

vector<string> CatalogManager::getCollName(string tableName)
{
	vector<string> s;
	int i,j;
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tableName)
			break;
	}
	if (i == TableNum)
	{
		cout << "Error: Fail to create index, table " << tableName << " does not exist" << endl;
	}
	else
	{
		for (j = 0; j < Table[i].colNum; j++)
			s.push_back(Table[i].column[j].colName);
	}
	return s;
}

vector<string> CatalogManager::getCollType(string tableName)
{
	vector<string> s;
	int i, j;
	for (i = 0; i < TableNum; i++)
	{
		if (Table[i].tableName == tableName)
			break;
	}
	if (i == TableNum)
	{
		cout << "Error: Fail to create index, table " << tableName << " does not exist" << endl;
	}
	else
	{
		for (j = 0; j < Table[i].colNum; j++)
			s.push_back(Table[i].column[j].colType);
	}
	return s;
}
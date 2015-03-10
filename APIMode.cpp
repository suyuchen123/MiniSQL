//
//  APIMode.cpp
//  MiniSQL
//
//  Created by xiangcy123a on 14/11/17.
//  Copyright (c) 2014年 Teain. All rights reserved.
//

#include "APIMode.h"

using namespace std;
CatalogManager catalogM;
record recordM;
buffer bm;
IndexManager indexM(bm);
void APIMode::DropTable(string tableName)
{

	if (catalogM.DropTable(tableName))
	{
		cout << "The table " + tableName + " has been deleted!" << endl;
	}
	else
		return;
            
}

void APIMode::DropIndex(string indexName)
{
	if (catalogM.DropIndex(indexName))
	{
		cout << "The index " + indexName + " has been deleted!" << endl;
	}
	else
		return;
	
}

void APIMode::CreateIndex(string indexName,string tableName,string colName)
{
	if (catalogM.CreateIndex(indexName, tableName, colName))
	{}
	else
	{
		return;
	}
	int i;
    catalogM.selectRecord(tableName,colName);
	cout << catalogM.res.size() << endl;
    if (catalogM.rei.size()!=0) 
	{
		for (i = 0; i < catalogM.rei.size(); i++) 
		{
			Index index;
			index.init(catalogM.rei[i]);
            index.setTuple(i+1);
			indexM.setIndex(INT, 0);
            indexM.insert(indexName,index);
        }
    }
	else if (catalogM.ref.size() != 0)
	{
		for (i = 0; i < catalogM.ref.size(); i++) 
		{
			Index index;
			index.init(catalogM.ref[i]);
            index.setTuple(i+1);
			indexM.setIndex(FLOAT, 0);
            indexM.insert(indexName,index);
		}
	}
	else if (catalogM.res.size() != 0)
	{
		for (i = 0; i < catalogM.res.size(); i++)
		{
			Index index;
			string type;
			index.init(catalogM.res[i]);
			index.setTuple(i + 1);
			for (int j = 0; j < catalogM.TableNum; j++)
			{
				if (catalogM.Table[j].tableName == tableName)
				{
					for (int k = 0; k < catalogM.Table[j].colNum; k++)
					{
						if (catalogM.Table[j].column[k].colName == colName)
							type = catalogM.Table[j].column[k].colType;
					}
				
				}
			}
			
			int l = catalogM.getLength(type);
			indexM.setIndex(STRING, l);
			indexM.insert(indexName, index);
		}
	}
    else
        cout << "Error in creating index" << endl;
}

void APIMode::CreateTable(string tableName,vector<string> col,vector<string> type,vector<int> unique,string primKey)
{
	if (catalogM.CreateTable(tableName, col, type, unique, primKey))
	{
		cout << "The table " + tableName + " has been created!" << endl;
	}
	else
		return;
}

void APIMode::SelectValue(string tableName)
{
	if (recordM.selectRecord(tableName))
		cout << "Select Successful!" << endl;
	else
		cout << "gg,no data!" << endl;
}

void APIMode::SelectValue(string tableName, vector<string> colName, vector<string> condition, vector<string> operater)
{
    if (recordM.selectRecord(tableName,colName,condition,operater)) {
        cout << "Select Successful!" << endl;
    }
    else
        cout << "Get data fail, or no data!" << endl;
}

void APIMode::InsertValue(string tableName,vector<string> value)
{
	char s[2000];
	memset(s, 0, 2000);
	char *p;
	int pos = 0;
	vector<string> type;
	type = catalogM.getCollType(tableName);
	for (unsigned i = 0; i<value.size(); i++)
	{

		if (catalogM.getLength(type.at(i)) + pos>2000)
		{
			cout << "Failed to insert. The record is too long" << endl;
			break;
		}

		if (type.at(i) == "int")
		{
			int_t t;
			t.value = atoi(value.at(i).c_str());
			p = (char *)&t;
			for (int j = 0; j<sizeof(int); j++, pos++)
			{
				s[pos] = *p;
				p++;
			}
		}

		else if (type.at(i) == "float")
		{
			float_tt tt;
			tt.value = (float)atof(value.at(i).c_str());
			p = (char *)&tt;
			for (int j = 0; j<sizeof(float); j++, pos++)
			{
				s[pos] = *p;
				p++;
			}
		}

		else
		{

			if (value.at(i).length()>(unsigned int)catalogM.getLength(type.at(i)))
			{
				cout << "Insert Failed. The string of " << value.at(i) << " is too long" << endl;
				break;
			}
			const char * cp;
			cp = value.at(i).c_str();
			for (unsigned int j = 0; j<value.at(i).length(); j++, pos++)
			{
				s[pos] = *cp;
				cp++;
			}
			for (unsigned int j = value.at(i).length(); j<(unsigned int)catalogM.getLength(type.at(i)); j++, pos++)
				;

		}
	}
		
	if (catalogM.InsertValue(tableName, value))
	{

		if (recordM.insertRecord(tableName, s)) {
			cout << "Insert Successful!" << endl;
		}
		else
			cout << "Insert Fail!" << endl;
	}
	else
		return;
}

void APIMode::DeleteValue(string tableName)
{
	if (catalogM.DeleteValue(tableName))
	{

		if (recordM.deleteValue(tableName)) {
			cout << "Delete Successful!" << endl;
		}
		else
			cout << "Delete Fail!" << endl;
	}
	else
		return;
   
}

void APIMode::DeleteValue(string tableName, vector<string> colName1, vector<string> condition1, vector<string> operater1)
{
	int i;
	i = recordM.deleteValue(tableName, colName1, condition1, operater1);
	if (catalogM.DeleteValue(tableName, colName1, condition1, operater1, i))
	{
		if (i >= 0) {
			cout << "Delete Successful!" << endl;
		}
		else
			cout << "Delete Fail!" << endl;
	}
	else
		return;
}












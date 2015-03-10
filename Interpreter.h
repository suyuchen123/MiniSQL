#ifndef _Interpreter_H_
#define _Interpreter_H_

#include "APIMode.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class Interpreter
{
    public:
		string in_SQL;
		string Key;
		string firstKey;
		string subKey;

		string tableName;
		string indexName;
		int temp;
		APIMode api;
	public:
		void start();
		void read_input();
		string get_Key(int *temp);
		string check_nKey(int temp);
		void drop();
		void create();
		void select();
		void insert();
		void sql_delete();
		void execfile();
		void error(int num, string index);  
		int check_sign(int temp);
		int check_havea(string s);
		char get_sign(int temp);         //获取特定位置的符号
		string exe(string key1,int *tmp);
		void exeSQL();
};

#endif
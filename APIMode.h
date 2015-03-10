// 
 //  APIMode.h 
 //  MiniSQL 
 // 
 //  Created by xiangcy123a on 14/11/17. 
 //  Copyright (c) 2014年 Teain. All rights reserved. 
 // 
  
 #ifndef _APIMode_h_ 
 #define _APIMode_h_ 
  
 #include <vector> 
 #include <iostream> 
 #include <string> 
 #include "Record.h" 
 #include "CatalogManager.h" 
 #include "indexManager.h" 
#include "buffer.h"
 using namespace std; 

 struct int_t{
	 int value;
 };
 struct float_tt{
	 float value;
 };

 class APIMode { 
 public: 

 	// 
 	/* 
 	 *  Interpreter 和 CatalogManger模块需要的API接口实现函数 
 	 */ 
 public: 
 	void DropTable(string tableName);  //删除表 
 	void DropIndex(string indexName);  //删除索引 
 	void CreateIndex(string indexName, string tableName, string colName); //创建索引 
 	void CreateTable(string tableName, vector<string> col, vector<string> type, vector<int> unique, string primKey);  //创建表 
 	void SelectValue(string tableName);  //查找值 
 	void SelectValue(string tableName, vector<string> colName, vector<string> op, vector<string> value); //查找值(重载) 
 	void InsertValue(string tableName, vector<string> value);  //插入值 
 	void DeleteValue(string tableName);  //删除值 
 	void DeleteValue(string tableName, vector<string> colName1, vector<string> op, vector<string> value); //删除值(重载) 
 }; 
  
 #endif 
 
#ifndef _record_H_
#define _record_H_

#include <string>
#include <vector>
using namespace std;
class record{
public:
	record(){}
	int selectRecord(string tableName);
	int selectRecord(string tableName, vector<string> colName, vector<string> cond, vector<string> operater);

	int insertRecord(string tableName, const char * s);

	int deleteValue(string tableName);
	int deleteValue(string tableName, vector<string> colName, vector<string> cond, vector<string> operater);

	int fullFillCond(string type, char * value, string scond, string operater);
	vector<int> selectInt(string tableName, string colName);
	vector<float> selectFloat(string tableName, string colName);
	vector<string> selectString(string tableName, string colName);
};






#endif
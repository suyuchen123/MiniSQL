#include "Interpreter.h"
#include <sstream>
std::stringstream ss;


void Interpreter::read_input()
{
	string tmp;
	temp = 0;
	in_SQL = "";
	char str[100];
	bool finish = false;
	cout << "MiniSQL>>" ;
	while (!finish)
	{
		cin >> str;
		tmp = str;
		in_SQL = in_SQL + " " + tmp;
		if (in_SQL.at(in_SQL.length() - 1) == ';')
		{
			finish = true;
		}
	}
	while (in_SQL.at(0) == ' ' || in_SQL.at(0) == '\n')
	in_SQL.erase(0, 1);

}
string Interpreter::exe(string SQL, int *tmp)
{
	string s;
	int start = 0, end = 0;
	while ((SQL[*tmp] == ' ' || SQL[*tmp] == 10 || SQL[*tmp] == '(' || SQL[*tmp] == ')' || SQL[*tmp] == 39 || SQL[*tmp] == ',' || SQL[*tmp] == ';') && *tmp < SQL.size())	//ȥ��ǰ�治�ǹؼ��ʵ������ַ�
	{
		(*tmp)++;
	}
	start = *tmp;

	if (*tmp == SQL.size())   //�ַ�����ȡ���
	{
		s = "END READ";
	}
	else
	{
		while (SQL[*tmp] != ';')
			(*tmp)++;
		end = *tmp;
		s = SQL.substr(start, end - start);
		s = s + ';';
		(*tmp)++;
	}
	//cout << s << endl;
	return s;

}
void Interpreter::exeSQL()
{
	temp = 0;
	Key = get_Key(&temp);
	if (Key == "END READ")    //�ж��Ƿ�Ϊֻ��һ���ֺõĿ���䣬����ǣ���������
	{
		cout << "Error: Not statement input, please try again!" << endl;
		return;
	}
	if (Key != "quit")  //�жϵ�һ���ؼ��ʣ�ѡ��ִ�еĺ�����quit���˳�
	{
		if (Key == "create")
		{
			create();
		}
		else if (Key == "drop")
		{
			drop();
		}
		else if (Key == "select")
		{
			select();
		}
		else if (Key == "insert")
		{
			insert();
		}
		else if (Key == "delete")
		{
			sql_delete();
		}
		else
		{
			error(0, Key);
			return;
		}
	}
}
string Interpreter::get_Key(int *temp)  
{
	int start=0, end=0;
	while ((in_SQL[*temp] == ' ' || in_SQL[*temp] == 10 || in_SQL[*temp] == '(' || in_SQL[*temp] == ')' || in_SQL[*temp] == 39 || in_SQL[*temp] == ',') && in_SQL[*temp] != ';')	//ȥ��ǰ�治�ǹؼ��ʵ������ַ�
	{
		(*temp)++;
	}
	start = *temp;

	if (in_SQL[*temp] == ';')   //�ַ�����ȡ���
	{
		Key = "END READ";
	}
	else
	{
		while (in_SQL[*temp] != ' ' && in_SQL[*temp] != 10 && in_SQL[*temp] != '(' && in_SQL[*temp] != ')' && in_SQL[*temp] != 39 && in_SQL[*temp] != ',' && in_SQL[*temp] != ';')
			(*temp)++;
		end = *temp;
		Key = in_SQL.substr(start, end - start);
	}
	
	return Key;
}
string Interpreter::check_nKey(int temp)
{
	string check;
	int start = 0, end = 0;
	while ((in_SQL[temp] == ' ' || in_SQL[temp] == 10 || in_SQL[temp] == '(' || in_SQL[temp] == ')' || in_SQL[temp] == 39 || in_SQL[temp] == ',') && in_SQL[temp] != ';')	//ȥ��ǰ�治�ǹؼ��ʵ������ַ�
	{
		temp++;
	}
	start = temp;

	if (in_SQL[temp] == ';')   //�ַ�����ȡ���
	{
		check = "END READ";
	}
	else
	{
		while (in_SQL[temp] != ' ' && in_SQL[temp] != 10 && in_SQL[temp] != '(' && in_SQL[temp] != ')' && in_SQL[temp] != 39 && in_SQL[temp] != ',' && in_SQL[temp] != ';')
			temp++;
		end = temp;
		check = in_SQL.substr(start, end - start);
	}

	return check;
}
int Interpreter::check_havea(string s)
{
	int i;
	int flag = 0;
	s = s + ';';
	for (i = 0; s[i] != ';'; i++)
	{
		if (s[i]<48 || s[i]>57)
			flag = 1;
	}
	return flag;

}
int Interpreter::check_sign(int temp)
{
	if (get_sign(temp) != ',' && get_sign(temp) != '(' && get_sign(temp) != ')' && get_sign(temp) != 39 && get_sign(temp) != ' ')
		return 1;
	else
		return 0;
}
char Interpreter::get_sign(int temp)
{
	char sign;
	sign = in_SQL[temp];
	return sign;
}
void Interpreter::drop()
{
	if (!(get_sign(temp) == ' ' && check_sign(temp+1) == 1))    //����drop����ֻ�ܸ��ո������ж��Ż������ָ���ţ��򷵻ش���
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	if (Key != "END READ")
	{
		if (Key == "table")
		{
			if (!(get_sign(temp) == ' ' && check_sign(temp+1) == 1))
			{//�ж��������
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			if (Key == "END READ")
			{
				error(4, Key);
				return;
			}

			tableName = Key;
			if (get_sign(temp) == ';' || (get_sign(temp) == ' '&& get_sign(temp+1) == ';'))
			{
				Key = get_Key(&temp);
				if (Key == "END READ")
				{ //cout << "drop table"<<tableName << endl; 
					api.DropTable(tableName);
				}
				else
					error(2, Key);
			}
			else
			{
				error(1, Key);
				return;
			}
		}// end if (Key == "table")
		else if(Key == "index")
		{
			if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			if (Key == "END READ")
			{
				error(4, Key);
				return;
			}

			indexName = Key;
			if (get_sign(temp) == ';' || (get_sign(temp) == ' '&& get_sign(temp+1) == ';'))
			{

				Key = get_Key(&temp);
				if (Key == "END READ")
				{ //cout << "drop index" << indexName << endl; 
					api.DropIndex(indexName);
				}
				else
					error(2, Key);
			}
			else
			{
				error(1, Key);
				return;
			}
		}// end else if(Key == "index")
		else // û�����ָ��
		{
			error(3, Key);
		}

	}// end if (Key != "END READ")
	else   //Key == END READ �����������
	{
		error(4, Key);
	}
}
void Interpreter::create()
{
	string colName; //�ڴ�������ʱ�洢����
	string type1;   //�洢��Ϻ��char���ͣ���char(10)
	string primary_key; //�洢����
	vector<string> col;  //create tableʱ�洢����
	vector<string> type;  //�洢����
	vector<int> unique;  //�洢�Ƿ�unique
	int NumOfChar;

	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))    //����create����ֻ�ܸ��ո������ж��Ż������ָ���ţ��򷵻ش���
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	if (Key != "END READ")
	{
		if (Key == "table")
		{
			if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�ж�table������쳣����
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			if (Key == "END READ")    //�жϺ����Ƿ����ַ������ޣ������������
			{
				error(4, Key);
				return;
			}

			tableName = Key;
			if ( !((get_sign(temp) == '(' && check_sign(temp + 1) == 1) ||                                    //�����ʽ tablename(col
				   (get_sign(temp) == '(' && get_sign(temp + 1) == ' ' && check_sign(temp + 2) == 1) ||       //tablename( col
				   (get_sign(temp) == ' ' && get_sign(temp + 1) == '(' && check_sign(temp + 2) == 1) ||       //tablename (col
				   (get_sign(temp) == ' ' && get_sign(temp + 1) == '(' && get_sign(temp + 2) == ' ' && check_sign(temp + 3) == 1)   //tablename ( col
				  )
			   )  //�жϱ�������쳣����
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			if (Key == "END READ")
			{
				error(4, Key);
				return;
			}
			while (Key != "END READ")
			{
				if (Key != "primary")
				{
					col.push_back(Key);
					if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
					{
						error(1, Key);
						return;
					}
					Key = get_Key(&temp);
					if (Key == "END READ")
					{
						error(4, Key);
						return;
					}
					if (Key == "int" || Key == "float")//  type =int, float
					{
						type.push_back(Key);
						if (get_sign(temp) == ' ' && check_sign(temp + 1) == 1 && check_nKey(temp) != "END READ")   //�жϺ�����Ƿ���uniqueָ��
						{
							Key = get_Key(&temp);
							if (!(Key == "unique")) //�жϺ����Ƿ���uniqueָ��
							{
								error(0, Key);
								return;
							}
							if ((get_sign(temp) == ',' && get_sign(temp + 1) == ' ' && check_sign(temp + 2) == 1) ||
								(get_sign(temp) == ' ' && get_sign(temp+1) == ',' && get_sign(temp + 2) == ' ' && check_sign(temp + 3) == 1))  //���ݿ�ָ��ʾ��������һ�е���Ϣ����ʽΪ int/float, colname
							{
								unique.push_back(1);
								Key = get_Key(&temp);
							}
							else if ((get_sign(temp) == ')' && get_sign(temp + 1) == ';') ||       //��β��ʽ int);
								(get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';')    //int );
								)
							{
								unique.push_back(1);
								Key = get_Key(&temp);								
							}
							else
							{
								error(1, Key);
								return;
							}
							
						}
						else if ((get_sign(temp) == ',' && get_sign(temp + 1) == ' ' && check_sign(temp + 2) == 1)||
							(get_sign(temp) == ' ' && get_sign(temp + 1) == ',' && get_sign(temp + 2) == ' ' && check_sign(temp + 3) == 1))//���ݿ�ָ��ʾ��������һ�е���Ϣ����ʽΪ int/float, colname
						{
							unique.push_back(0);
							Key = get_Key(&temp);
						}
						else if (check_nKey(temp) == "END READ")
						{
							if (!((get_sign(temp) == ')' && get_sign(temp + 1) == ';') ||       //��β��ʽ int);
								(get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';')    //int );
								)
								)
							{
								error(1, Key);
								return;
							}
							unique.push_back(0);
							Key = get_Key(&temp);
						}
						else
						{
							error(1, Key);
							return;
						}
					}
					else if (Key == "char")   //type = char
					{
						if (!((get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && check_sign(temp+2) == 1) ||    //��ʽ char (10)
							  (get_sign(temp) == '(' && check_sign(temp+1) == 1)                                    // char(10)
							  )
							)
						{
							error(1, Key);
							return;
						}
						Key = get_Key(&temp);
						if (Key == "END READ")    //�жϺ����Ƿ����ַ������ޣ������������
						{
							error(4, Key);
							return;
						}
						if (check_havea(Key) == 1)  //�ж�char�ĳ����Ƿ��������
						{
							error(6, Key);
							return;
						}
						ss.clear();
						ss.str("");
						ss << Key;
						ss >> NumOfChar;
						if (NumOfChar > 255 || NumOfChar < 1)   //�ж�char�ĳ����Ƿ񳬳���Χ
						{
							error(5, Key);
							return;
						}
						type1 = "char(" + Key + ")";
						type.push_back(type1);  //��char���ʹ���type����
						if (get_sign(temp) == ')' && get_sign(temp+1) == ' ' && check_sign(temp + 2) == 1 && check_nKey(temp) != "END READ")   //�жϺ�����Ƿ���uniqueָ��
						{
							Key = get_Key(&temp);
							if (!(Key == "unique")) //�жϺ����Ƿ���uniqueָ��
							{
								error(0, Key);
								return;
							}							
							if ((get_sign(temp) == ',' && get_sign(temp + 1) == ' ' && check_sign(temp + 2) == 1) ||
								(get_sign(temp) == ' ' && get_sign(temp + 1) == ',' && get_sign(temp + 2) == ' ' && check_sign(temp + 3) == 1))  //���ݿ�ָ��ʾ��������һ�е���Ϣ����ʽΪ int/float, colname
							{
								unique.push_back(1);
								Key = get_Key(&temp);
							}
							else if ((get_sign(temp) == ')' && get_sign(temp + 1) == ';') ||       //��β��ʽ int);
								(get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';')    //int );
								)
							{
								unique.push_back(1);
								Key = get_Key(&temp);
							}
							else
							{
								error(1, Key);
								return;
							}
						}
						else if ((get_sign(temp) == ')' && get_sign(temp + 1) == ',' && get_sign(temp + 2) == ' ' && check_sign(temp + 3) == 1) ||
							(get_sign(temp) == ')' && get_sign(temp + 1) == ' ' && get_sign(temp + 2) == ',' && get_sign(temp+3)==' ' && check_sign(temp + 4) == 1))//���ݿ�ָ��ʾ��������һ�е���Ϣ����ʽΪ int/float, colname
						{
							unique.push_back(0);
							Key = get_Key(&temp);
						}
						else if (check_nKey(temp) == "END READ")
						{
							if (!((get_sign(temp) == ')' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';') ||       //��β��ʽ char(10));
								(get_sign(temp) == ')' && get_sign(temp+1) == ' ' && get_sign(temp + 2) == ')' && get_sign(temp + 3) == ';')    //char(10) );
								)
								)
							{
								error(1, Key);
								return;
							}
							unique.push_back(0);
							Key = get_Key(&temp);
						}
						else
						{
							error(1, Key);
							return;
						}
					}// end type=char
				}//end Key!=primary
				else  //Key == primary
				{
					if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
					{
						error(1, Key);
						return;
					}
					Key = get_Key(&temp);
					if (Key != "key")  //�ж��Ƿ�Ϊprimary key
					{
						error(7, Key);
						return;
					}
					if (!(get_sign(temp) == ' '&& get_sign(temp + 1) == '('&& check_sign(temp + 2) == 1 ||
						(get_sign(temp) == ' '&& get_sign(temp + 1) == '('&& get_sign(temp + 2) == ' '&& check_sign(temp + 3) == 1)
						)
						)// �жϺ�����쳣����
					{
						error(1, Key);
						return;
					}
					Key = get_Key(&temp);
					bool flag = false;
					for (unsigned int index = 0; index < (col.size()); index++)
					{
						if (Key == col[index])
							flag = true;
					}
					if (flag == false)
					{
						error(8, Key);
						return;
					}
					if (check_nKey(temp) != "END READ")
					{
						error(9, Key);
						return;
					}
					if (!((get_sign(temp) == ')' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';') ||       //��β��ʽ primary key (col));
						(get_sign(temp) == ')' && get_sign(temp + 1) == ' ' && get_sign(temp + 2) == ')' && get_sign(temp + 3) == ';') ||   //primary key (col) );
						(get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ')' && get_sign(temp + 3) == ';') ||   //primary key ( col ));
						(get_sign(temp) == ' ' && get_sign(temp+1) == ')' && get_sign(temp + 2) == ' ' && get_sign(temp + 3) == ')' && get_sign(temp + 4) == ';')    //primary key ( col ) );
						)
						)
					{
						error(1, Key);
						return;
					}
					primary_key = Key;
					Key = get_Key(&temp);
				}//end Key==primary
			}//end while(Key != END READ)

			//���Դ���
			/*cout << "create table" << endl;
			cout << "tablename =" << tableName << endl;
			cout << "col" << " " << "type" << " " << "unique" << endl;
			for (int ii = 0; ii<(col.size()); ii++)
				cout << col[ii] << " " << type[ii] << " " << unique[ii] << endl;		
			if (primary_key != "")
				cout << "primary key =" << primary_key << endl;
			cout << "END show" << endl;*/

			api.CreateTable(tableName,col,type,unique,primary_key);
		}
		else if (Key == "index")
		{
			if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�ж�table������쳣����
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			indexName = Key;  //����������
			if (Key == "END READ")    //�жϺ����Ƿ����ַ������ޣ������������
			{
				error(4, Key);
				return;
			}
			if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			if (Key == "END READ")    //�жϺ����Ƿ����ַ������ޣ������������
			{
				error(4, Key);
				return;
			}
			if (Key != "on")
			{
				error(1, Key);
				return;
			}
			if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			tableName = Key;
			if (Key == "END READ")    //�жϺ����Ƿ����ַ������ޣ������������
			{
				error(4, Key);
				return;
			}
			if (!(get_sign(temp) == ' '&& get_sign(temp + 1) == '('&& check_sign(temp + 2) == 1 ||
				(get_sign(temp) == ' '&& get_sign(temp + 1) == '('&& get_sign(temp + 2) == ' '&& check_sign(temp + 3) == 1)
				)
				)// �жϺ�����쳣����
			{
				error(1, Key);
				return;
			}
			Key = get_Key(&temp);
			colName = Key;
			if (Key == "END READ")    //�жϺ����Ƿ����ַ������ޣ������������
			{
				error(4, Key);
				return;
			}
			if (check_nKey(temp) != "END READ")
			{
				error(9, Key);
				return;
			}
			if (!((get_sign(temp) == ')' && get_sign(temp + 1) == ';') ||       //��β��ʽ  (colName);
				(get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';')   //primary key ( colName );
				)
				)
			{
				error(1, Key);
				return;
			}

			api.CreateIndex(indexName,tableName,colName);
			//cout << "CreateIndex(" << indexName << "," << tableName << "," << colName << ");" << endl;
		}
		else //Key!=table && Key != index
		{
			error(13, Key);
			return;
		}
	}// end if (Key != "END READ")
	else
	{
		error(4, Key);
		return;
	}
}
void Interpreter::select()
{
	vector<string> colName;
	vector<string> value;
	vector<string> op;
	if (!(get_sign(temp) == ' ' && get_sign(temp + 1) == '*' && get_sign(temp + 2) == ' ' && check_sign(temp + 3) == 1))  // ���select����������ַ�
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	Key = get_Key(&temp);
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
	{
		error(1, Key);
		return;
	}
	if (Key != "from")
	{
		error(10, Key);
		return;
	}
	Key = get_Key(&temp);
	tableName = Key;
	if (check_nKey(temp) == "END READ")  // ������������
	{
		if (get_sign(temp) == ';' || (get_sign(temp) == ' ' && get_sign(temp + 1) == ';'))
		{
			api.SelectValue(tableName);
			//cout << "SelectValue (" << tableName << ");" << endl;
		}
	} // end ������������
	else  //������������
	{
		Key = get_Key(&temp);
		if (Key == "END READ")  //���벻����
		{
			error(4, Key);
			return;
		}
		if (Key != "where")
		{
			error(0, Key);
			return;
		}
		if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
		{
			error(1, Key);
			return;
		}
		while (Key !="END READ")
		{
			if (Key == "where" || Key == "and")
			{
				Key = get_Key(&temp);
				colName.push_back(Key);
				if (Key == "END READ")  //���벻����
				{
					error(4, Key);
					return;
				}
				if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
				{
					error(1, Key);
					return;
				}
				Key = get_Key(&temp);
				if (Key == "END READ")  //���벻����
				{
					error(4, Key);
					return;
				}
				if (Key != "=" && Key != "<>" && Key != "<" && Key != ">" && Key != "<=" && Key != ">=")  //�жϷ���
				{
					error(0, Key);
					return;
				}
				op.push_back(Key);
				if (get_sign(temp) == ' ' && check_sign(temp + 1) == 1)   //���û���ţ�����ֵ��
				{
					Key = get_Key(&temp);
					value.push_back(Key);
					if (Key == "END READ")  //���벻����
					{
						error(4, Key);
						return;
					}					
					if (check_nKey(temp) == "END READ")  //�жϽ����ķ���
					{
						if (!(get_sign(temp) == ';' || (get_sign(temp) == ' ' && get_sign(temp + 1) == ';')))
						{
							error(1, Key);
							return;
						}
						Key = get_Key(&temp);
					}
					else
					{
						if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
						{
							error(1, Key);
							return;
						}
						if (check_nKey(temp) == "where")
						{
							error(0, check_nKey(temp));
							return;
						}
						Key = get_Key(&temp);
					}
				} //end if ��ֵ
				else if (get_sign(temp) == ' ' && get_sign(temp + 1) == 39 && check_sign(temp + 2) == 1) //�����ţ��ַ���
				{
					Key = get_Key(&temp);
					value.push_back(Key);
					if (Key == "END READ")  //���벻����
					{
						error(4, Key);
						return;
					}					
					if (check_nKey(temp) == "END READ")  //�жϽ����ķ���
					{
						if (!((get_sign(temp) == 39 && get_sign(temp + 1) == ';') || (get_sign(temp) == 39 &&  get_sign(temp+1) == ' ' && get_sign(temp + 2) == ';')))
						{
							error(1, Key);
							return;
						}
						Key = get_Key(&temp);
					}
					else
					{
						if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
						{
							error(1, Key);
							return;
						}
						if (check_nKey(temp) == "where")
						{
							error(0, check_nKey(temp));
							return;
						}
						Key = get_Key(&temp);
					}
				}// end �ַ���
				else
				{
					error(1, Key);
					return;
				}
			}// end if(Key=where||key=and)
			else//  �Է���һ
			{
				error(0, Key);
				return;
			}
		}// end while
		api.SelectValue(tableName,colName,value,op);

		//���Դ���
		/*cout << "select" << endl;
		cout << "tablename =" << tableName << endl;
		cout << "col" << " " << "op" << " " << "value" << endl;
		for (int ii = 0; ii<(colName.size()); ii++)
		cout << colName[ii] << " " << op[ii] << " " << value[ii] << endl;

		cout << "END show" << endl;*/

	} //end ������������
}
void Interpreter::sql_delete()
{
	vector<string> colName;
	vector<string> value;
	vector<string> op;
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  // ���delete����������ַ�
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
	{
		error(1, Key);
		return;
	}
	if (Key != "from")
	{
		error(11, Key);
		return;
	}
	Key = get_Key(&temp);
	tableName = Key;
	if (check_nKey(temp) == "END READ")  // ��������ɾ��,������������ɾ��
	{
		if (get_sign(temp) == ';' || (get_sign(temp) == ' ' && get_sign(temp + 1) == ';'))
		{
			api.DeleteValue(tableName);
			//cout << "DeleteValue (" << tableName << ");" << endl;
		}
	} // end ��������ɾ��
	else  //��������ɾ��
	{
		Key = get_Key(&temp);
		if (Key == "END READ")  //���벻����
		{
			error(4, Key);
			return;
		}
		if (Key != "where")
		{
			error(0, Key);
			return;
		}
		if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
		{
			error(1, Key);
			return;
		}
		while (Key != "END READ")
		{
			if (Key == "where" || Key == "and")
			{
				Key = get_Key(&temp);
				colName.push_back(Key);
				if (Key == "END READ")  //���벻����
				{
					error(4, Key);
					return;
				}
				if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
				{
					error(1, Key);
					return;
				}
				Key = get_Key(&temp);
				if (Key == "END READ")  //���벻����
				{
					error(4, Key);
					return;
				}
				if (Key != "=" && Key != "<>" && Key != "<" && Key != ">" && Key != "<=" && Key != ">=")  //�жϷ���
				{
					error(0, Key);
					return;
				}
				op.push_back(Key);
				if (get_sign(temp) == ' ' && check_sign(temp + 1) == 1)   //���û���ţ�����ֵ��
				{
					Key = get_Key(&temp);
					value.push_back(Key);
					if (Key == "END READ")  //���벻����
					{
						error(4, Key);
						return;
					}
					if (check_nKey(temp) == "END READ")  //�жϽ����ķ���
					{
						if (!(get_sign(temp) == ';' || (get_sign(temp) == ' ' && get_sign(temp + 1) == ';')))
						{
							error(1, Key);
							return;
						}
						Key = get_Key(&temp);
					}
					else
					{
						if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
						{
							error(1, Key);
							return;
						}
						if (check_nKey(temp) == "where")
						{
							error(0, check_nKey(temp));
							return;
						}
						Key = get_Key(&temp);
					}
				} //end if ��ֵ
				else if (get_sign(temp) == ' ' && get_sign(temp + 1) == 39 && check_sign(temp + 2) == 1) //�����ţ��ַ���
				{
					Key = get_Key(&temp);
					value.push_back(Key);
					if (Key == "END READ")  //���벻����
					{
						error(4, Key);
						return;
					}
					if (check_nKey(temp) == "END READ")  //�жϽ����ķ���
					{
						if (!((get_sign(temp) == 39 && get_sign(temp + 1) == ';') || (get_sign(temp) == 39 && get_sign(temp + 1) == ' ' && get_sign(temp + 2) == ';')))
						{
							error(1, Key);
							return;
						}
						Key = get_Key(&temp);
					}
					else
					{
						if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
						{
							error(1, Key);
							return;
						}
						if (check_nKey(temp) == "where")
						{
							error(0, check_nKey(temp));
							return;
						}
						Key = get_Key(&temp);
					}
				}// end �ַ���
				else
				{
					error(1, Key);
					return;
				}
			}// end if(Key=where||key=and)
			else//  �Է���һ
			{
				error(0, Key);
				return;
			}
		}// end while
		api.DeleteValue(tableName,colName,value,op);

		//���Դ���
		/*cout << "END READ" << endl;
		cout << "tablename =" << tableName << endl;
		cout << "col" << " " << "op" << " " << "value" << endl;
		for (int ii = 0; ii<(colName.size()); ii++)
			cout << colName[ii] << " " << op[ii] << " " << value[ii] << endl;

		cout << "END show" << endl;*/

	} //end ��������ɾ��
}
void Interpreter::insert()
{
	vector<string> value;
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  // ������������ַ�
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
	{
		error(1, Key);
		return;
	}
	if (Key != "into")
	{
		error(12, Key);
		return;
	}
	Key = get_Key(&temp);
	tableName = Key;
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  //�жϺ�����쳣����
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	if (Key != "values")
	{
		error(0, Key);
		return;
	}
	if (get_sign(temp) == '(' && (get_sign(temp + 1) == ' ' || get_sign(temp + 1) == 39 || check_sign(temp+1)==1))
		 //char
	{
		temp++;
	}
	else if (get_sign(temp) == ' ' && get_sign(temp + 1) == '(')
	{
		temp++;
		temp++;
	}
	if (check_nKey(temp) == "END READ")
	{
		error(4, Key);
		return;
	}
	while (check_nKey(temp) != "END READ")
	{
		if ((get_sign(temp) == 39 && check_sign(temp + 1) == 1) ||
			(get_sign(temp) == ' ' && get_sign(temp + 2) == 39 && check_sign(temp + 3) == 1)
			) //char
		{
			Key = get_Key(&temp);
			value.push_back(Key);
			if (check_nKey(temp) == "END READ")
			{
				if (!((get_sign(temp) == 39 && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';') ||        //�涨��ʽ'b');
					(get_sign(temp) == 39 && get_sign(temp + 1) == ' ' && get_sign(temp + 2) == ')' && get_sign(temp + 3) == ';')   //'a' );
					))
				{
					error(1, Key);
					return;
				}
			}
			else
			{
				if ((get_sign(temp) == 39 && get_sign(temp + 1) == ',' ) ||     //�涨��ʽ'a','b'
					(get_sign(temp) == 39 && get_sign(temp + 1) == ',' && get_sign(temp + 2) == ' ')    //'a', 'b'	
					)
				{
					temp++;
					temp++;
				}
				else if ((get_sign(temp) == 39 && get_sign(temp + 1) == ' ' && get_sign(temp + 2) == ',') ||    //'a' ,'b'
					(get_sign(temp) == 39 && get_sign(temp + 1) == ' ' && get_sign(temp + 2) == ',' && get_sign(temp + 3) == ' ') //'a' , 'b'
					)
				{
					temp++;
					temp++;
					temp++;
					temp++;
				}
				else
				{
					error(1, Key);
					return;
				}
			}
		}
		else if ((check_sign(temp) == 1) ||
			(get_sign(temp) == ' ' && check_sign(temp + 1) == 1)
			) //int,float
		{
			Key = get_Key(&temp);
			value.push_back(Key);
			if (check_nKey(temp) == "END READ")
			{
				if (!((get_sign(temp) == ')' && get_sign(temp + 1) == ';') ||        //�涨��ʽ b);
					(get_sign(temp) == ' ' && get_sign(temp + 1) == ')' && get_sign(temp + 2) == ';')   //a );
					))
				{
					error(1, Key);
					return;
				}
			}
			else
			{
				if ((get_sign(temp) == ',')||     //�涨��ʽ1,2					
					(get_sign(temp) == ',' && get_sign(temp + 1) == ' ' )//1, 2
					)
				{
					temp++;
				}
				else if ((get_sign(temp) == ' ' && get_sign(temp + 1) == ',')||    //'a' ,'b'
					(get_sign(temp) == ' ' && get_sign(temp + 1) == ',' && get_sign(temp + 3) == ' ')//'a' , 'b'
					)
				{
					temp++;
					temp++;
				}
				else
				{
					error(1, Key);
					return;
				}
			}
		}
		else
		{
			error(1, Key);
			return;
		}

	}
	api.InsertValue(tableName, value);
	//���Դ���
	/*cout << "insert" << endl;
	cout << "tablename =" << tableName << endl;
	cout << "value" << endl;
	for (int ii = 0; ii<(value.size()); ii++)
	cout << value[ii] << endl;

	cout << "END show" << endl;*/
}
void Interpreter::execfile()
{
	int tmp = 0;
	unsigned int i = 0;
	ifstream file;
	string key;
	string key1, sql;
	if (!(get_sign(temp) == ' ' && check_sign(temp + 1) == 1))  // ������������ַ�
	{
		error(1, Key);
		return;
	}
	Key = get_Key(&temp);
	if (!((get_sign(temp) == ';') || (get_sign(temp) == ' '&&get_sign(temp + 1) == ';')))
	{
		error(1, Key);
		return;
	}
	key =  Key + ".txt";
	file.open(key);

	if (!file.is_open())
	{
		cout << "Fail to open " << key << endl;
	}
	else
	{
		while (!file.eof())
		{
			
			key1 = key1 + ' ' + sql;
			file >> sql;
		}
		for (i = 0; i<key1.size(); i++)
		{
		if (key1[i] == ' ' && key1[i + 1] == ' ')
		key1.erase(i + 1, 1);
		}
		//cout << key1 << endl;
		while (tmp < key1.size())
		{
			in_SQL = "";
			in_SQL=exe(key1, &tmp);
			//cout << in_SQL << endl;
			exeSQL();
		}

	}
	file.close();
}

void Interpreter::error(int num, string index)
{
	switch (num)
	{
	case 0:cout << "Error: The '" << index << "' is not a legal statement, please check again!"<<endl; break;
	case 1:cout << "Error: There are some illegal sign around '" << index << "', please check again!" << endl; break;
	case 2:cout << "Error: There are some redundancy around '" << index << "', please check again!" << endl; break;
	case 3:cout << "Error: The 'drop " << index << "' is not a legal statement, please check again!" << endl; break;
	case 4:cout << "Error: Incomplete SQL statment, please check again!" << endl; break;
	case 5:cout << "Error: The longth of the type 'char' is out of range!" << endl; break;
	case 6:cout << "Error: Wrong type of the longth of char!" << endl; break;
	case 7:cout << "Error: The 'primary " << index << "' is not a legal statement, please check again!" << endl; break;
	case 8:cout << "Error: The '" << index << "' is not a legal colname has been definded, please check again!" << endl; break;
	case 9:cout << "Error: Some unknown word around '" << index << "' , please check again!" << endl; break;
	case 10:cout << "Error: The 'select * " << index << "' is not a legal statement, please check again!" << endl; break;
	case 11:cout << "Error: The 'delete " << index << "' is not a legal statement, please check again!" << endl; break;
	case 12:cout << "Error: The 'insert " << index << "' is not a legal statement, please check again!" << endl; break;
	case 13:cout << "Error: The 'create" << index << "' is not a legal statement, please check again!" << endl; break;
	default:
		break;
	}
}

void Interpreter::start()
{
	
	read_input();        //��ȡ�����SQL���
	Key = get_Key(&temp);    //��ȡ��һ���ؼ���
	/*�������в����жϡ��ָ���š����жϹؼ��ʣ����Ǳ��ո��������ţ��س��������ŷָ�Ķ���Ϊ�ǹؼ��ʣ�
	�����ڹؼ����п��ܻ�����»��ߵ�����������ţ���Щ���Ų��ᱻʶ��Ϊ���ָ���š����Ի����ڹؼ����С�*/
	while (Key == "END READ")    //�ж��Ƿ�Ϊֻ��һ���ֺõĿ���䣬����ǣ���������
	{
		cout << "Error: Not statement input, please try again!" << endl;
		read_input();
		Key = get_Key(&temp);
	}
	while (Key != "quit")  //�жϵ�һ���ؼ��ʣ�ѡ��ִ�еĺ�����quit���˳�
	{
		if (Key == "create")
		{
			create();
		}
		else if (Key == "drop")
		{
			drop();
		}
		else if (Key == "select")
		{
			select();
		}
		else if (Key == "insert")
		{
			insert();
		}
		else if (Key == "delete")
		{
			sql_delete();
		}
		else if (Key == "execfile")
		{
			execfile();
		}
		else
		{
			error(0, Key);
		}
		read_input();
		Key = get_Key(&temp);
	}
	cout << "Thank you to using our MiniSQL!" << endl;
}
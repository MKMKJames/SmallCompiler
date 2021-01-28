#pragma once
#include<fstream>
#include<iostream>
#include<stack>
#include<queue>
#include"global.h"

class Compiler {
private:
	char getch();

	void getsym();

	void getToken();

	void do_valDeclare();

	void do_valDefine();

	void do_multiSentence();

	void do_identTable(vector<token*>&);

	void do_statementTable();

	Meta do_statement();

	Meta do_sign();

	Meta do_if();

	Meta do_while();

	Meta do_repeat();

	Meta do_algriExp();

	Meta do_boolExp();

	Meta do_term();

	Meta do_factor();

	Meta do_algriVal();

	Meta do_boolTerm();

	Meta do_boolFactor();

	Meta do_boolVal();

	string genTemp();

	void genTAC(string, string, string, string);

	void backPatch(vector<int>, string);

	void backPatch(vector<int>, int);

	vector<int> makeList(int);

	vector<int> merge(vector<int>,vector<int>);

	string filename = {};
	vector<string> inputLines = {};
	int linepos = 0;
	int lineNum = 0;
	int ptokens = 0;
	int nextaddr = 0;
	int temp = 1;
	int minus = 0;//0表示正，1表示无括号负，2表示有括号负
	int Not= 0;//0表示无非，1表示无括号非，2表示有括号非
	bool lexical_error = false;
	ifstream myfile;
	vector<token> tokens;
	vector<TAC*> tacs;
	stack<Type> nameStk;
	token* pt = nullptr;
	char ch = ' ';

public:
	Compiler();

	Compiler(string);

	~Compiler();

	//词法分析主函数
	void lexical();

	void print_lex();

	//递归下降语法分析&&四元式生成
	void parse();

	void print_tac();
};
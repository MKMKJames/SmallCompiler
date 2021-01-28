#pragma once
#include<string>
#include<vector>
#include<string>
#include<map>

using namespace std;

struct token {
	string name;
	string content;
	int code;

	token(string name, string content, int code) :
		name(name), content(content), code(code) {}
};

struct Type {
	string name;
	string type;
	Type(string name, string type) :
		name(name), type(type) {}
	Type(){}
};

struct TAC {
	string op;
	string operand1;
	string operand2;
	string result;
	TAC(string op, string operand1, string operand2, string result) :
		op(op), operand1(operand1), operand2(operand2), result(result) {}
};

struct Meta {
	vector<int> trueList = {};
	vector<int> falseList = {};
	vector<int> nextList = {};
	int beginCode = {};
};
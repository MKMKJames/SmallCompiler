#include "Compiler.h"

map<string, int> keywords = map<string, int>({
	{ "and", 1 }, { "array", 2 }, { "begin", 3 }, { "bool", 4 }, { "call", 5 },
	{ "case", 6 }, { "char", 7 }, { "constant", 8 }, { "dim", 9 }, { "do", 10 },
	{ "else", 11 }, { "end", 12 }, { "false", 13 }, { "for", 14 }, { "if", 15 },
	{ "input", 16 }, { "integer", 17 }, { "not", 18 }, { "of", 19 }, { "or", 20 },
	{ "output", 21 }, { "procedure", 22 }, { "program", 23 }, { "read", 24 }, { "real", 25 },
	{ "repeat", 26 }, { "set", 27 }, { "stop", 28 }, { "then", 29 }, { "to", 30 },
	{ "true", 31 }, { "until", 32 }, { "var", 33 }, { "while", 34 }, { "write", 35 },
	{ "(", 39 }, { ")", 40 }, { "*", 41 }, { "*/", 42 }, { "+", 43 }, { ",", 44 },
	{ "-", 45 }, { ".", 46 }, { "..", 47 }, { "/", 48 }, { "/*", 49 }, { ":", 50 },
	{ ":=", 51 }, { ";", 52 }, { "<", 53 }, { "<=", 54 }, { "<>", 55 }, { "=", 56 },
	{ ">", 57 }, { ">=", 58 }, { "[", 59 }, { "]", 60 }
	});

map<string, string> checkType;

Compiler::Compiler() {
	cout << "请输入文件名：";
	cin >> filename;
	myfile.open(filename);
	if (!myfile.is_open()) {
		cout << "不存在该文件！" << endl;
		exit(-1);
	}
	string tmp;
	while (getline(myfile, tmp)) {
		inputLines.push_back(tmp + "\n");
	}
}

Compiler::Compiler(string filename) {
	myfile.open(filename);
	if (!myfile.is_open()) {
		cout << "不存在该文件！" << endl;
		exit(-1);
	}
	string tmp;
	while (getline(myfile, tmp)) {
		inputLines.push_back(tmp + "\n");
	}
}

Compiler::~Compiler() {
	for (auto tac : tacs) {
		delete tac;
	}
}

char Compiler::getch() {
	if (linepos >= inputLines[lineNum].length()) {
		linepos = 0;
		lineNum++;
	}
	if (lineNum >= inputLines.size()) {
		return 0;
	}
	char res = inputLines[lineNum][linepos++];
	return res;
}

void Compiler::genTAC(string op, string op1, string op2, string res) {
	tacs.push_back(new TAC(op, op1, op2, res));
	nextaddr++;
}

void Compiler::backPatch(vector<int> List, string addr) {
	for (auto i : List)
		tacs[i]->result = addr;
}

vector<int> Compiler::merge(vector<int> p1, vector<int> p2) {
	p1.insert(p1.end(), p2.begin(), p2.end());
	return p1;
}

vector<int> Compiler::makeList(int i) {
	return { i };
}

void Compiler::backPatch(vector<int> List, int addr) {
	for (auto i : List)
		tacs[i]->result = to_string(addr);
}

void Compiler::print_tac() {
	int count = 0;
	for (auto i : tacs) {
		printf("(%d) (%s, %s, %s, %s)\n", count++, i->op.c_str(),
			i->operand1.c_str(), i->operand2.c_str(), i->result.c_str());
	}
	cout << endl;
}

string Compiler::genTemp() {
	return "T" + to_string(temp++);
}

//一次识别一个符号
void Compiler::getsym() {
	string word;
	while (ch == ' ' || ch == '\t' || ch == '\n') {
		ch = getch();
	}
	if (isalpha(ch)) {
		do {
			word.push_back(ch);
			ch = getch();
		} while (isalpha(ch) || isdigit(ch));
		if (keywords.count(word)) {
			tokens.push_back({ word,"",keywords[word] });
			//是关键字
		}
		else {
			//是标识符
			tokens.push_back({ word,"",36 });
		}
	}
	else {
		if (isdigit(ch)) {
			do {
				word.push_back(ch);
				ch = getch();
			} while (isdigit(ch));
			if (isalpha(ch)) {
				printf("无法识别的单词：第 %d 行\n", lineNum + 1);
				lexical_error = true;
				lineNum++;
			}
			else
				tokens.push_back({ "",word,37 });
		}
		else {
			if (ch == ':') {
				word.push_back(ch);
				ch = getch();
				if (ch == '=') {
					//赋值符号
					word.push_back(ch);
					tokens.push_back({ word,"",keywords[word] });
					ch = getch();
				}
				else {
					tokens.push_back({ word,"",keywords[word] });
				}
			}
			else {
				if (ch == '<') {
					word.push_back(ch);
					ch = getch();
					if (ch == '=') {
						//<=
						word.push_back(ch);
						tokens.push_back({ word,"",keywords[word] });
						ch = getch();
					}
					else if (ch == '>') {
						//<>
						word.push_back(ch);
						tokens.push_back({ word,"",keywords[word] });
						ch = getch();
					}
					else {
						//<
						tokens.push_back({ word,"",keywords[word] });
					}
				}
				else {
					word.push_back(ch);
					if (ch == '>') {
						ch = getch();
						if (ch == '=') {
							//>=
							word.push_back(ch);
							tokens.push_back({ word,"",keywords[word] });
							ch = getch();
						}
						else {
							//>
							tokens.push_back({ word,"",keywords[word] });
						}
					}
					else {
						if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '=' || ch == ',' ||
							ch == ';' || ch == '-' || ch == '+' || ch == '*') {
							tokens.push_back({ word,"",keywords[word] });
							ch = getch();
						}
						else if (ch == '/') {
							ch = getch();
							if (ch == '*') {
								bool wrong = true;
								while ((ch = getch()) != '\n') {
									if (ch == '*' && (ch = getch() == '/')) {
										wrong = false;
										ch = getch();
										break;
									}
								}
								if (wrong) {
									printf("注释错误：第 %d 行\n", lineNum + 1);
									lexical_error = true;
								}
							}
							else {
								tokens.push_back({ word,"",keywords[word] });
							}
						}
						else if (ch == '\'') {
							word.pop_back();
							int first_pos = linepos;
							int count = 0;
							int lastpos;
							while ((ch = getch()) != '\n') {
								if (ch == '\'') {
									lastpos = linepos;
									count++;
								}
							}
							if (count != 1) {
								printf("字符常量错误：第 %d 行\n", lineNum + 1);
								lexical_error = true;
							}
							else {
								tokens.push_back({ "", inputLines[lineNum].substr(first_pos,lastpos - first_pos - 1),38 });
								linepos = lastpos;
							}
						}
						else if (ch == '.') {
							ch = getch();
							if (ch == '.') {
								word.push_back(ch);
								tokens.push_back({ word,"",keywords[word] });
							}
							else
								tokens.push_back({ word,"",keywords[word] });
						}
					}
				}
			}
		}
	}
}

void Compiler::lexical() {
	vector<char> illegal;
	for (auto s : inputLines) {
		for (auto ch : s) {
			if (ch == '\n' || ch == '\t' || ch == ' ')
				continue;
			if (!isalpha(ch) && !isdigit(ch) && !(ch == '(' || ch == ')' || ch == '[' ||
				ch == ']' || ch == '=' || ch == ',' || ch == ':' || ch == '.' || ch == '\'' ||
				ch == '/' || ch == '<' || ch == '>' || ch == ';' || ch == '-' || ch == '+' || ch == '*')) {
				illegal.push_back(ch);
			}
		}
	}
	if (!illegal.empty()) {
		cerr << "源程序中含有非法字符：" << endl;
		for (auto c : illegal)
			cout << c << endl;
		exit(-1);
	}
	while (lineNum != inputLines.size()) {
		getsym();
	}
	if (lexical_error) {
		exit(-1);
	}
}

void Compiler::print_lex() {
	int count = 0;
	int iden = 1;
	map<string, int> check;
	for (auto i : tokens) {
		if (i.code != 36 && i.code != 37 && i.code != 38) {
			printf("( %d , - )\t", i.code);
		}
		else {
			if (check.count(i.name) == 0) {
				printf("( %d , %d )\t", i.code, iden);
				check[i.name] = iden++;
			}
			else {
				printf("( %d , %d )\t", i.code, check[i.name]);
			}
		}
		count++;
		if (count % 5 == 0) {
			cout << endl;
		}
	}
	cout << endl;
}

/**************************************************************/
/*
  递归下降语法分析
  在子程序中生成四元式
*/
/**************************************************************/

void Compiler::getToken() {
	if (ptokens == tokens.size()) {
		pt = nullptr;
		return;
	}
	pt = &tokens[ptokens++];
}

void Compiler::parse() {
	getToken();
	if (pt->code != keywords["program"]) {
		cout << "panic: missing keyword \"program\"" << endl;
		exit(-1);
	}
	getToken();
	string name = pt->name;
	if (pt->code != 36) {
		cout << "panic: missing identifier" << endl;
		exit(-1);
	}
	getToken();
	if (pt->code != keywords[";"]) {
		cout << "panic: missing \";\"" << endl;
		exit(-1);
	}
	genTAC("program", name, "-", "-");
	do_valDeclare();

	do_multiSentence();
	getToken();
	if (pt->code != keywords["."]) {
		cout << "panic: missing keyword \".\"" << endl;
		exit(-1);
	}
	genTAC("sys", "-", "-", "-");
}

void Compiler::do_valDeclare() {
	getToken();
	if (pt->code == keywords["var"]) {
		do_valDefine();
	}
}

void Compiler::do_valDefine() {
	vector<token*> temp;
	do_identTable(temp);
	if (pt->code != keywords[":"]) {
		cout << "panic: missing keyword \":\"" << endl;
		exit(-1);
	}
	getToken();
	if (pt->name != "integer" && pt->name != "bool" && pt->name != "char") {
		cout << "panic: type" << endl;
		exit(-1);
	}

	for (auto i : temp) {
		checkType[i->name] = pt->name;
	}

	getToken();
	if (pt->code != keywords[";"]) {
		cout << "panic: missing keyword \";\"" << endl;
		exit(-1);
	}
	getToken();
	if (pt->code == 36) {
		ptokens--;
		do_valDefine();
	}
	else
		ptokens--;
}

void Compiler::do_identTable(vector<token*>& res) {
	getToken();
	if (pt->code != 36) {
		cout << "panic: missing keyword" << endl;
		exit(-1);
	}
	res.push_back(pt);
	getToken();
	if (pt->code == keywords[","]) {
		do_identTable(res);
	}
}

void Compiler::do_multiSentence() {
	getToken();
	if (pt->code != keywords["begin"]) {
		cout << "panic: missing keyword \"begin\"" << endl;
		exit(-1);
	}
	do_statementTable();
	//cout << pt->content << endl;
	if (pt->code != keywords["end"]) {
		cout << "panic: missing keyword \"end\"" << endl;
		exit(-1);
	}
}

void Compiler::do_statementTable() {
	Meta temp = do_statement();
	backPatch(temp.nextList, nextaddr);
	if (pt->code == keywords[";"]) {
		do_statementTable();
	}
}

Meta Compiler::do_statement() {
	getToken();
	Meta res;
	if (pt->code == 36) {
		return do_sign();
	}
	else if (pt->code == keywords["while"]) {
		return do_while();
	}
	else if (pt->code == keywords["if"]) {
		return do_if();
	}
	else if (pt->code == keywords["repeat"]) {
		return do_repeat();
	}
	else if (pt->code == keywords["begin"]) {
		do_multiSentence();
	}
	return res;
}

Meta Compiler::do_while() {
	Meta B, S, S1;
	S.beginCode = nextaddr;
	B = do_boolExp();
	if (pt->code != keywords["do"]) {
		cout << "panic: missing keyword \"do\"" << endl;
		exit(-1);
	}
	S1 = do_statement();
	backPatch(S1.nextList, B.beginCode);
	backPatch(B.trueList, S1.beginCode);
	S.nextList = B.falseList;
	genTAC("j", "-", "-", to_string(B.beginCode));
	return S;
}

Meta Compiler::do_if() {
	Meta B, S, S1, S2, N;
	S.beginCode = nextaddr;
	B = do_boolExp();
	if (pt->code != keywords["then"]) {
		cout << "do_if: missing keyword \"then\"" << endl;
		exit(-1);
	}
	S1 = do_statement();

	N.nextList = makeList(nextaddr);
	genTAC("j", "-", "-", "-");

	if (pt->code == keywords["else"]) {
		S2 = do_statement();
		backPatch(B.trueList, S1.beginCode);
		backPatch(B.falseList, S2.beginCode);
		auto tmp = merge(S1.nextList, N.nextList);
		S.nextList = merge(S2.nextList, tmp);
	}
	else {
		backPatch(B.trueList, S1.beginCode);
		S.nextList = merge(B.falseList, S1.nextList);
	}
	return S;
}

Meta Compiler::do_repeat() {
	Meta S, S1, B;
	S.beginCode = nextaddr;
	S1 = do_statement();
	if (pt->code != keywords["until"]) {
		cout << "do_repeat: missing keyword \"until\"" << endl;
		exit(-1);
	}
	B = do_boolExp();
	backPatch(B.falseList, S1.beginCode);
	backPatch(S1.nextList, B.beginCode);
	S.nextList = B.trueList;
	return S;
}

Meta Compiler::do_sign() {
	//赋值语句的beginCode就是第一次进入do_sign时候的nextaddr
	Meta temp;
	temp.beginCode = nextaddr;
	string name = pt->name;
	if (checkType.count(name) == 0) {
		cerr << "赋值错误：未定义类型的标识符 " << name << endl;
		exit(-1);
	}
	string _type = checkType[name];
	getToken();
	if (pt->code != keywords[":="]) {
		cout << "do_sign: missing keyword \":=\"" << endl;
		exit(-1);
	}
	do_algriExp();
	if (nameStk.size() >= 1) {
		Type tmp = nameStk.top();
		nameStk.pop();
		if (_type != tmp.type) {
			cerr << "类型错误：请检查赋值语句左右是否是同一类型" << endl;
			exit(-1);
		}
		genTAC(":=", tmp.name, "-", name);
	}
	return temp;
}

Meta Compiler::do_algriExp() {
	Meta temp;
	temp.beginCode = nextaddr;
	do_term();
	while (pt->code == 45 || pt->code == 43) {//- +
		string op = pt->code == 43 ? "+" : "-";
		do_term();
		if (nameStk.size() >= 2) {
			Type op2 = nameStk.top();
			nameStk.pop();
			Type op1 = nameStk.top();
			nameStk.pop();
			if (op1.type != op2.type) {
				cerr << "类型错误：请检查运算符左右是否是同一类型" << endl;
				exit(-1);
			}
			Type tmp = { genTemp() ,op1.type };
			genTAC(op, op1.name, op2.name, tmp.name);
			nameStk.push(tmp);
			if (minus == 2) {
				genTAC(":=", "-" + tmp.name, "-", tmp.name);
				minus = 0;
			}
		}
	}
	return temp;
}


Meta Compiler::do_term() {
	Meta temp;
	temp.beginCode = nextaddr;
	do_factor();
	getToken();
	if (pt->code == 41 || pt->code == 48) {// * /
		string op = pt->code == 41 ? "*" : "/";
		do_term();
		if (nameStk.size() >= 2) {
			Type op2 = nameStk.top();
			nameStk.pop();
			Type op1 = nameStk.top();
			nameStk.pop();
			if (op1.type != op2.type) {
				cerr << "类型错误：请检查运算符左右是否是同一类型" << endl;
				exit(-1);
			}
			Type tmp = { genTemp() ,op1.type };
			genTAC(op, op1.name, op2.name, tmp.name);
			nameStk.push(tmp);
		}
	}
	return temp;
}

Meta Compiler::do_factor() {
	Meta temp;
	temp.beginCode = nextaddr;
	getToken();
	if (pt->code == keywords["-"]) {
		minus = 1;
		temp = do_factor();
	}
	else {
		ptokens--;
		temp = do_algriVal();
	}
	return temp;
}

Meta Compiler::do_algriVal() {
	Meta temp;
	temp.beginCode = nextaddr;
	getToken();
	if (pt->code == 37) {
		//get integer
		string num = minus == 1 ? "-" + pt->content : pt->content;
		nameStk.push({ num ,"integer" });
		if (minus == 1)
			minus = 0;
	}
	else if (pt->code == 36) {
		//get indentifier
		string num = minus == 1 ? "-" + pt->name : pt->name;
		nameStk.push({ num ,checkType[pt->name] });
		if (minus == 1)
			minus = 0;
	}
	else if (pt->code == keywords["("]) {
		if (minus == 1)
			minus = 2;
		do_algriExp();
		if (pt->code != keywords[")"]) {
			cerr << "do_algriVal: missing \")\"" << endl;
			exit(-1);
		}
	}
	else {
		cerr << "do_algriVal: Error" << endl;
		exit(-1);
	}

	return temp;
}

Meta Compiler::do_boolExp() {
	Meta temp1, temp2, B;
	temp1.beginCode = nextaddr;
	temp1 = do_boolTerm();
	B = temp1;
	while (pt->code == keywords["or"]) {
		temp2 = do_boolTerm();
		backPatch(temp1.falseList, temp2.beginCode);
		B.falseList = temp2.falseList;
		B.trueList = merge(temp1.trueList, temp2.trueList);
	}
	return B;
}

Meta Compiler::do_boolTerm() {
	Meta temp1, temp2, B;
	temp1.beginCode = nextaddr;
	temp1 = do_boolFactor();
	B = temp1;
	getToken();
	if (pt->code == keywords["and"]) {
		temp2 = do_boolTerm();
		backPatch(temp1.trueList, temp2.beginCode);
		B.trueList = temp2.trueList;
		B.falseList = merge(temp1.falseList, temp2.falseList);
	}
	return B;
}

Meta Compiler::do_boolFactor() {
	Meta temp;
	temp.beginCode = nextaddr;
	getToken();
	if (pt->code == keywords["not"]) {
		Not = 1;
		temp = do_boolFactor();
	}
	else {
		ptokens--;
		temp = do_boolVal();
	}
	return temp;
}

Meta Compiler::do_boolVal() {
	Meta B;
	B.beginCode = nextaddr;
	getToken();
	if (pt->code == 36) {
		string name = pt->name;
		string _type1 = checkType[pt->name];
		getToken();
		string relop = pt->name;
		if (pt->code == keywords["<"] || pt->code == keywords["<>"] || pt->code == keywords["<="]
			|| pt->code == keywords[">"] || pt->code == keywords[">="] || pt->code == keywords["="]) {
			getToken();
			string _type2 = checkType[pt->name];
			if (pt->code == 36) {
				//ok
				if (_type1 != "integer" && _type2 != "integer") {
					cerr << "布尔表达式错误: 关系运算符两端的标识符应该均为integer类型" << endl;
					exit(-1);
				}
				B.trueList = makeList(nextaddr);
				B.falseList = makeList(nextaddr + 1);
				if (Not == 1) {
					swap(B.trueList, B.falseList);
					Not = 0;
				}
				genTAC("j" + relop, name, pt->name, "-");
				genTAC("j", "-", "-", "-");
				return B;
			}
			else
				ptokens -= 3;
		}
		else {
			ptokens--;
			B.trueList = makeList(nextaddr);
			B.falseList = makeList(nextaddr + 1);
			if (Not == 1) {
				swap(B.trueList, B.falseList);
				Not = 0;
			}
			genTAC("jnz", name, "-", "-");
			genTAC("j", "-", "-", "-");
			return B;
		}
	}

	else if (pt->code == keywords["true"]) {
		B.trueList = makeList(nextaddr);
		if (Not == 1) {
			swap(B.trueList, B.falseList);
			Not = 0;
		}
		genTAC("j", "-", "-", "-");
		return B;
	}

	else if (pt->code == keywords["false"]) {
		B.falseList = makeList(nextaddr);
		if (Not == 1) {
			swap(B.trueList, B.falseList);
			Not = 0;
		}
		genTAC("j", "-", "-", "-");
		return B;
	}

	else if (pt->code == keywords["("]) {
		if (Not == 1)
			Not = 2;
		Meta temp;
		temp = do_boolExp();
		if (pt->code != keywords[")"]) {
			cerr << "do_boolVal: missing \")\"" << endl;
			exit(-1);
		}
		if (Not == 2) {
			swap(temp.trueList, temp.falseList);
			Not = 0;
		}
		B = temp;
		return B;
	}

	if (pt->code == 37) {
		do_algriExp();
		string relop = pt->name;
		if (pt->code == keywords["<"] || pt->code == keywords["<>"] || pt->code == keywords["<="]
			|| pt->code == keywords[">"] || pt->code == keywords[">="] || pt->code == keywords["="]) {

		}
		else {
			cerr << "do_boolVal: missing relop" << endl;
			exit(-1);
		}
		do_algriExp();
		if (nameStk.size() >= 2) {
			Type op1, op2;
			op2 = nameStk.top();
			nameStk.pop();
			op1 = nameStk.top();
			nameStk.pop();
			if (op1.type != "integer" && op2.type != "integer") {
				cerr << "出现类型错误，请检查关系运算符两端是否都是integer" << endl;
				exit(-1);
			}
			B.trueList = makeList(nextaddr);
			B.falseList = makeList(nextaddr + 1);
			if (Not == 1) {
				auto tmp = B.trueList;
				B.trueList = B.falseList;
				B.falseList = tmp;
				Not = 0;
			}
			genTAC("j" + relop, op1.name, op2.name, "-");
			genTAC("j", "-", "-", "-");
		}
		ptokens--;
		return B;
	}

	else {
		cerr << "do_boolVal: Error" << endl;
		exit(-1);
	}

	return B;
}

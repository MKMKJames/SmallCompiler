#include "Compiler.h"

int main(void) {
	cout << "凌子尧" << "\n" << "19计科1班" << "\n" << "201930362095" << endl;
	Compiler compiler;
	compiler.lexical();//词法分析
	compiler.print_lex();//打印词法分析结果
	compiler.parse();//语法分析&语义分析&四元式生成
	compiler.print_tac();//打印四元式
}
#include "Compiler.h"

int main(void) {
	cout << "����Ң" << "\n" << "19�ƿ�1��" << "\n" << "201930362095" << endl;
	Compiler compiler;
	compiler.lexical();//�ʷ�����
	compiler.print_lex();//��ӡ�ʷ��������
	compiler.parse();//�﷨����&�������&��Ԫʽ����
	compiler.print_tac();//��ӡ��Ԫʽ
}
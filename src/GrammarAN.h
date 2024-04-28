#ifndef _GRAMMAR_AN_H
#define _GRAMMAR_AN_H

#include "common.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <cstdlib>
#include <stack>
#include <stdint.h>

using namespace std;


struct Token
{
	int lineShow;		//行号
	LexType lex;		//词法信息
	string sem;		//语义信息
};

TreeNode* parseLL();
void CreateLL1Table();
void Predict(int pnum);
int Priosoty(LexType op);
LexType getLexType(const std::string& str);
string enumkindToString(StmtType value);
void BuildLL1tree(TreeNode* Prok);
string getDeckEnumString(DeclareType deckEnum);
int compareIgnoreCase(string str1, string str2);
void process1();
void process2();
void process3();
void process4();
void process5();
void process6();
void process7();
void process8();
void process9();
void process10();
void process11();
void process12();
void process13();
void process14();
void process15();
void process16();
void process17();
void process18();
void process19();
void process20();
void process21();
void process22();
void process23();
void process24();
void process25();
void process26();
void process27();
void process28();
void process29();
void process30();
void process31();
void process32();
void process33();
void process34();
void process35();
void process36();
void process37();
void process38();
void process39();
void process40();
void process41();
void process42();
void process43();
void process44();
void process45();
void process46();
void process47();
void process48();
void process49();
void process50();
void process51();
void process52();
void process53();
void process54();
void process55();
void process56();
void process57();
void process58();
void process59();
void process60();
void process61();
void process62();
void process63();
void process64();
void process65();
void process66();
void process67();
void process68();
void process69();
void process70();
void process71();
void process72();
void process73();
void process74();
void process75();
void process76();
void process77();
void process78();
void process79();
void process80();
void process81();
void process82();
void process83();
void process84();
void process85();
void process86();
void process87();
void process88();
void process89();
void process90();
void process91();
void process92();
void process93();
void process94();
void process95();
void process96();
void process97();
void process98();
void process99();
void process100();
void process101();
void process102();
void process103();
void process104();


#endif
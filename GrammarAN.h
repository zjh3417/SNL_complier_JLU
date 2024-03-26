#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<set>
#include<cstdlib>
#include<stack>
using namespace std;

enum NodeKindEnum {	//语法树节点类型法枚举
	ProK,				//根标志节点
	PheadK,			//程序头标志节点
	TypeK,				//类型声明标志节点
	VarK,				//	变量声明标志节点
	ProcDecK,		//	函数声明标志节点
	StmLK,				//语句序列标志节点
	DecK,				//声明节点
	StmtK,				//语句节点
	ExpK					//表达式节点
};
enum DeckEnum {	//声明节点类型枚举
	ArrayK,				//数组类型
	CharK,				//字符类型
	IntegerK,			//整数类型
	RecordK,			//记录类型
	IdK					//类类型标志符作为类型
};
enum StmtKEnum {	//语句节点类型枚举
	IfK,					//判断语句类型
	WhileK,				//循环语句类型
	AssignK,			//赋值语句类型
	ReadK,				//读语句类型
	WriteK,				//写语句类型
	CallK,				//函数调用语句类型
	ReturnK			//返回语句类型
};
enum ExpKEnum {	//	表达式节点类型枚举
	OpK,					//	操作符类型
	ConstK,			//常整数类型
	IdEK					//标识符类型
};
enum ParamtEnum {	//参数是值参还是变参
	Valparamtype,
	Varparamtype
};
enum VarKindEnum {	//变量的类别,分别表示变量是标志符变量，数组成员变量还是域成员变量
	IdV,
	ArrayMembV,
	FieldMembV
};
enum LexType {
	ID, IF, BEGIN, INTC, END, PLUS, MINUS, TIMES, OVER, EQ, LT, LMIDPAREN, RMIDPAREN, DOT
	, TYPE, VAR, PROCEDURE, PROGRAM, SEMI, INTEGER, CHAR, ARRAY, RECORD, UNDERANGE
	, OF, COMMA, LPAREN, RPAREN, ENDWH, WHILE, RETURN, READ, WRITE, ASSIGN, THEN, FI
	, ELSE, DO
};
enum ExpType {
	Void, Integer, Boolean
};
/*******************共用体声明************************/
union Kind {
	DeckEnum dec;		//记录语法树节点的声明类型,当nodekind=DecK时有效
	StmtKEnum stmt;		//记录语法树节点的语句类型,当nodekind=StmtK时有效
	ExpKEnum exp;		//记录语法树节点的表达式类型,当nodekind=ExpK时有效
};
/*******************结构体声明************************/
struct ArrayAttr {	//记录数组类型的属性。
	int low;
	int up;
	DeckEnum childType;
};
struct ProcAttr {	//记录过程的属性。
	ParamtEnum paramt;
};
struct ExpAttr {	//记录表达式的属性。
	LexType op;	//记录语法树节点的运算符单词，为单词类型
	int val;		//记录语法树节点的数值,
	VarKindEnum varkind;	//记录变量的类别
	ExpType type;	//记录语法树节点的检查类型，取值Void, Integer,Boolean,为类型检查ExpType类型
};
struct Attr {		//记录语法树节点其他属性
	ArrayAttr arrayAttr;
	ProcAttr procAttr;
	ExpAttr expAttr;
};
struct TreeNode
{
	TreeNode* child[3];	//指向子语法树节点指针,0是程序头,1是声明，2是程序体
	TreeNode* sibling;	//指向兄弟语法树节点指针
	int lineno;					//记录源程序行号
	NodeKindEnum nodekind;//	记录语法树节点类型
	Kind kind;					//记录语法树节点的具体类型
	int idnum = 0;				//记录一个节点中标志符个数
	vector<string> name;	//容器成员是节点中的标志符的名字
	vector<int*> table;	//	容器成员是节点中的各个标志符在符号表中的入口
	string type_name;	//记录类型名，当节点为声明类型且类型是由类型标志符表示是有效
	Attr attr;				//记录语法树节点其他属性
};
struct Token
{
	int linsShow;		//记录单词行号
	LexType lex;		//记录该单词的词法信息，其中LexType为单词的类型枚举
	string sem;		//记录该单词的语义信息
};

//**********语法分析所要的变量声明*****************
stack<string> signStack;	//符号栈
stack<TreeNode*> opSignStack;	//操作符栈
stack<TreeNode*> opNumStack;	//操作数栈
stack<TreeNode**>	syntaxTreeStack;		//语法树栈
vector<vector<string>> productSet;	//存储所有的产生式
vector<vector<string>> predictSet;	//存储所有产生式的predict集
vector<vector<int>> LL1Table;	//LL1矩阵
set<string> VT;	//存储所有终极符
set<string> VN;	//存储所有非终极符
string S;	//开始符
TreeNode* currentP;	//存储当前节点
TreeNode* saveP;		//保存当前指向记录类型声明节点的指针
TreeNode* saveFuncP;	//保存指向某一节点的指针
DeckEnum* temp;
bool getExpResult = true;
bool getExpResult2;	//用于数组
int expflag = 0;	//用于计算括号
extern Token token;
string arrayLexType_hyf[] = { "ID","IF","BEGIN","INTC","END","PLUS","MINUS","TIMES","OVER","EQ","LT","LMIDPAREN","RMIDPAREN" ,"DOT"
	,"TYPE","VAR", "PROCEDURE","PROGRAM","SEMI","INTEGER","CHAR","ARRAY","RECORD","UNDERANGE"
	,"OF","COMMA","LPAREN","RPAREN","ENDWH","WHILE","RETURN","READ","WRITE","ASSIGN","THEN","FI"
	,"ELSE","DO"
};

TreeNode* parseLL();
void CreateLL1Table();
int GetIndex(set<string>& A, string s);
void InputError(string errorInfo, string path);
void Predict(int pnum);
int Priosoty(LexType op);

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

#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdint.h>

using std::vector;
using std::string;


enum NodeTypeEnum {
	program_node,
	program_head_node,
	
	type_declare_node,
	var_declare_node,
	procedure_declare_node,

	declare_node,

	stms_node,
	
	stm_node,
	expr_node
};

enum DeclareType {
	array_declare,
	char_declare,
	int_declare,
	record_declare,
	type_declare
};
enum StmtType {
	IfK,
	WhileK,
	AssignK,
	ReadK,
	WriteK,
	CallK,
	ReturnK
};

enum ExpKEnum {
	OpK,					//表达式是操作符
	ConstK,					//表达式是常量
	CharC,					//字符
	IdEK					//表达式是变量
};
enum ParamtEnum {
	Valparamtype,
	Varparamtype
};
enum VarKindEnum {
	IdV,
	ArrayMembV,
	FieldMembV
};
enum LexType {
	ID, IF, BEGIN, INTC, END, PLUS, MINUS, TIMES, OVER, EQ, LT, LMIDPAREN, RMIDPAREN, DOT
	, TYPE, VAR, PROCEDURE, PROGRAM, SEMI, INTEGER, CHAR, ARRAY, RECORD, UNDERANGE
	, OF, COMMA, LPAREN, RPAREN, ENDWH, WHILE, RETURN, READ, WRITE, ASSIGN, THEN, FI
	, ELSE, DO,CHARC
};
enum ExpValueType {
	Void, Integer, Boolean
};


union Kind {
	DeclareType dec_type;			//如果是声明的话,这里面存放的是声明类型
	StmtType    stmt_type;			//如果是语句节点的话,这里面存放的是语句类型.
	ExpKEnum    exp_type;			//如果是表达式的话,这里面存放的是表达式类型
};

enum  StoreType
{
	local_var,
	param_var
};

struct VarAttr{
	uint32_t level;						//在第几层,这东西实际没啥用.
	uint32_t off;						//在当前函数内local var 的偏移.
	uint32_t declare_proc_identity;		//在哪个函数中声明
	uint32_t store_type;				//local 还是参数
	uint32_t size;						//变量大小.
	DeclareType type;
};

struct ArrayAttr{						//记录数组类型的属性。
	int low;							//数组下届
	int up;								//数组上届
	uint32_t     element_size;				//数组元素大小
	DeclareType  element_type;				//元素类型.

	ArrayAttr(){
		low = 0;
		up = 0;
		element_size = 0;
	}
};


struct ProcAttr {	//记录过程的属性。
	ParamtEnum paramt;
	uint32_t   proc_identity;			//过程的唯一标识
};

struct ExpAttr {
	LexType      op;
	int          val;
	char		 valc;
	VarKindEnum  varkind;
	ExpValueType type;
};


struct Attr{					//记录语法树节点其他属性
	VarAttr    varAttr;			//变量基本属性
	ArrayAttr  arrayAttr;		//

	ProcAttr   procAttr;
	ExpAttr    expAttr;
};

struct TreeNode
{
	TreeNode* child[3];			
	/*	
			program
				0 : 没用.
				1 : 类型声明 -> 变量声明 -> 过程声明
				2 : 过程体

			if 
				child[0] = condition
				child[1] = then
				child[2] = else

			proc:
				0: 形参
				1: 所有声明
				2: 过程体

			while:
				0: 条件表达式
				1: 一系列语句.

			普通标识符:
				没有child

			数组标识符:
				0:
				1:

			record 标识符:
				0: 成员名称.

	*/

	TreeNode*	    sibling;		//指向兄弟语法树节点指针
	NodeTypeEnum    node_type;		//记录语法树节点类型

	Kind kind;						//记录语法树节点的具体类型

	int idnum;					//记录一个节点中标志符个数		????.

	vector<string> name;		//容器成员是节点中的标志符的名字

	vector<int*> table;			//容器成员是节点中的各个标志符在符号表中的入口

	string type_name;			//记录类型名，当节点为声明类型且类型是由类型标志符表示是有效

	Attr		attr;			//记录语法树节点其他属性

	int			lineno;			//记录源程序行号

	TreeNode(){
		memset(child, 0, sizeof(child));
		sibling = NULL;

		lineno = -1;
		idnum = 0;
	}
};

#define alignment_size			0x4

#endif
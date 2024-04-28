#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "common.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
//
//符号表管理

enum IdKind { typeKind, varKind, procKind };

//形参信息表
struct ParamTable {
	struct symbtable * entry;                 //形参标识符在符号表中的位置
	struct ParamTable* next;

	ParamTable(){
		entry = NULL;
		next = NULL;
	}
};

enum TypeKind { 
	intTy,
	charTy, 
	arrayTy,
	recordTy,
	boolTy 
};//intTy, charTy, arrayTy, recordTy, boolTy


//标识符类型 信息
struct typeIR
{
	int		 size;		/*类型所占空间大小*/
	TypeKind kind;		//intTy, charTy, arrayTy, recordTy, boolTy

	union {
		struct{
			struct typeIR* indexTy;					//索引类型,索引可以是立即数和变量...
			struct typeIR* elemTy;					//数组元素类型.
		}ArrayAttr;

		struct fieldChain* body; /*记录类型中的域链*/
	}More;
};


//记录的域表
struct fieldChain
{
	string      idname;				//记录域中的标识符；
	typeIR *    unitType;			//指向域类型的内部表示；                                                               //这里有错误
	int         offset;				//表示当前标识符在记录中的偏移；
	struct fieldChain* next;		//指向下一个域。
};


/**************************************************************************************/
//标识符 信息
typedef struct
{
	struct typeIR* idtype;		/*指向标识符的类型内部表示*/
	IdKind kind;				/*标识符的类型*/  //typeKind, varKind, procKind

	struct {
		VarAttr   var_attr;		//变量信息
		ArrayAttr arr_attr;		//数组信息

		struct{
			uint32_t level;
			ParamTable* param;		/*参数表*/ //形参信息表地址
			uint32_t proc_identity;

			int code;
			int size;
		}ProcAttr;					/*过程名标识符的属性*/
	}More;							/*标识符的不同类型有不同的属性*/
}AttributeIR;


//符号表定义
struct symbtable
{
	string				idname;					//符号名称
	AttributeIR			attrIR;					//
	struct symbtable*	next;					//当前level的下一个符号.

	symbtable(){
		next = NULL;
	}
};

/*******************函数声明************************/
typeIR* TypeProcess(TreeNode* t);
void	statement(TreeNode* t);
void	process(TreeNode* t);
void	BodyProcess(TreeNode* t);
void	Analyze(TreeNode* currentP);

#endif
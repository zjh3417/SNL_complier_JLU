#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "common.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
//
//���ű����

enum IdKind { typeKind, varKind, procKind };

//�β���Ϣ��
struct ParamTable {
	struct symbtable * entry;                 //�βα�ʶ���ڷ��ű��е�λ��
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


//��ʶ������ ��Ϣ
struct typeIR
{
	int		 size;		/*������ռ�ռ��С*/
	TypeKind kind;		//intTy, charTy, arrayTy, recordTy, boolTy

	union {
		struct{
			struct typeIR* indexTy;					//��������,�����������������ͱ���...
			struct typeIR* elemTy;					//����Ԫ������.
		}ArrayAttr;

		struct fieldChain* body; /*��¼�����е�����*/
	}More;
};


//��¼�����
struct fieldChain
{
	string      idname;				//��¼���еı�ʶ����
	typeIR *    unitType;			//ָ�������͵��ڲ���ʾ��                                                               //�����д���
	int         offset;				//��ʾ��ǰ��ʶ���ڼ�¼�е�ƫ�ƣ�
	struct fieldChain* next;		//ָ����һ����
};


/**************************************************************************************/
//��ʶ�� ��Ϣ
typedef struct
{
	struct typeIR* idtype;		/*ָ���ʶ���������ڲ���ʾ*/
	IdKind kind;				/*��ʶ��������*/  //typeKind, varKind, procKind

	struct {
		VarAttr   var_attr;		//������Ϣ
		ArrayAttr arr_attr;		//������Ϣ

		struct{
			uint32_t level;
			ParamTable* param;		/*������*/ //�β���Ϣ���ַ
			uint32_t proc_identity;

			int code;
			int size;
		}ProcAttr;					/*��������ʶ��������*/
	}More;							/*��ʶ���Ĳ�ͬ�����в�ͬ������*/
}AttributeIR;


//���ű���
struct symbtable
{
	string				idname;					//��������
	AttributeIR			attrIR;					//
	struct symbtable*	next;					//��ǰlevel����һ������.

	symbtable(){
		next = NULL;
	}
};

/*******************��������************************/
typeIR* TypeProcess(TreeNode* t);
void	statement(TreeNode* t);
void	process(TreeNode* t);
void	BodyProcess(TreeNode* t);
void	Analyze(TreeNode* currentP);

#endif
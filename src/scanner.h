#pragma once
#ifndef _SCANNER_H
#define _SCANNER_H
#include "list.h"
#include <stdio.h>



typedef enum _lex_type_t
{
	_NULL_TYPE = 0,

	//保留字
	_PROGRAM,				//保留字 program
	_PROCEDURE,				//保留字 procedure
	_TYPE,					//保留字 type
	_VAR,					//保留字 var
	_IF,					//保留字 if
	_THEN,					//保留字 then
	_ELSE,					//保留字 else
	_FI,					//保留字 fi
	_WHILE,					//保留字 while
	_DO,					//保留字 do
	_ENDWH,					//保留字 endwh
	_BEGIN,					//保留字 begin
	_END,					//保留字 end
	_READ,					//保留字 read
	_WRITE,					//保留字 write
	_ARRAY,					//array
	_OF,					//of
	_RECORD,				//record
	_RETURN,				//return

	//类型
	_INTEGER, _CHAR,			//保留字 类型

	//====================================================

	//多字符单词符号
	_ID,					//普通标识符
	_INTC,					//无符号数值
	_CHARC,					//字符

	//特殊符号
	_ASSIGN,				// :=
	_UNDERANGE,				// ..
	_COMMENT,				// 注释 {}

	_EQ,					//  =
	_LT,					// <
	_PLUS,					// +
	_MINUS,					// -
	_TIMES,					// * 
	_OVER,					//	/
	_LPAREN,				// (
	_RPAREN,				// )
	_DOT,					// .
	_SEMI,					// ; 
	_LMIDPAREN,				// [
	_RMIDPAREN,				// ]
	_COMMA					//逗号

}lex_type_t;


static const char* lex_types[] = {
	"NULL_TYPE",

	//保留字
	"PROGRAM",				//保留字 program
	"PROCEDURE",			//保留字 procedure
	"TYPE",					//保留字 type
	"VAR",					//保留字 var
	"IF",					//保留字 if
	"THEN",					//保留字 then
	"ELSE",					//保留字 else
	"FI",					//保留字 fi
	"WHILE",					//保留字 while
	"DO",					//保留字 do
	"ENDWH",					//保留字 endwh
	"BEGIN",					//保留字 begin
	"END",					//保留字 end
	"READ",					//保留字 read
	"WRITE",					//保留字 write
	"ARRAY",					//array
	"OF",					//of
	"RECORD",				//record
	"RETURN",				//return

	//类型
	"INTEGER",
	"CHAR",			//保留字 类型

	//====================================================

	//多字符单词符号
	"ID", //ID,					//普通标识符
	"INTC", //_INTC,				//无符号数值
	"CHARC", //_CHARC,				//字符

	//特殊符号
	"ASSIGN",				// :=
	"UNDERANGE",				// ..
	"COMMENT",					//	"COMMENT",	注释 {}

	"EQ",					//  =
	"LT",					// <
	"PLUS",					// +
	"MINUS",				// -
	"TIMES",				// * 
	"OVER",					//	/
	"LPAREN",				// (
	"RPAREN",				// )
	"DOT",					// .
	"SEMI",					// ; 
	"LMIDPAREN",			// [
	"RMIDPAREN",			// ]
	"COMMA"					//逗号
};

static const char* sem_info[] = {
	NULL,

	//保留字
	"ReseverWord",				//保留字 program
	"ReseverWord",			//保留字 procedure
	"ReseverWord",					//保留字 type
	"ReseverWord",					//保留字 var
	"ReseverWord",					//保留字 if
	"ReseverWord",					//保留字 then
	"ReseverWord",					//保留字 else
	"ReseverWord",					//保留字 fi
	"ReseverWord",					//保留字 while
	"ReseverWord",					//保留字 do
	"ReseverWord",					//保留字 endwh
	"ReseverWord",					//保留字 begin
	"ReseverWord",					//保留字 end
	"ReseverWord",					//保留字 read
	"ReseverWord",					//保留字 write
	"ReseverWord",					//array
	"ReseverWord",					//of
	"ReseverWord",				//record
	"ReseverWord",				//return

	//类型
	"ReseverWord",
	"ReseverWord",			//保留字 类型

	//====================================================

	//多字符单词符号
	NULL, //ID,					//普通标识符
	NULL, //_INTC,				//无符号数值
	NULL, //_CHARC,				//字符

	//特殊符号
	"DobleSeparator",			// :=
	"ArrayIndex",				// ..
	NULL,					//	"COMMENT",	注释 {}

	"SingleSeparator",					//  =
	"SingleSeparator",					// <
	"SingleSeparator",					// +
	"SingleSeparator",				// -
	"SingleSeparator",				// * 
	"SingleSeparator",					//	/
	"SingleSeparator",				// (
	"SingleSeparator",				// )
	"Finish",					// .
	"SingleSeparator",					// ; 
	"SingleSeparator",			// [
	"SingleSeparator",			// ]
	"SingleSeparator"			//逗号
};

typedef struct _token {

	list_head list;

	int line;			//token line number..
	lex_type_t lex_type;

	char* sem;
	int sem_len;
	int sem_buf_size;
}token_t;

int get_tokens(FILE* fp, list_head** token_list);
void print_token_list(FILE* fp, list_head* token_list);
void print_token(FILE* fp, token_t* token);
void Token_Print(const char * src_code);
#endif
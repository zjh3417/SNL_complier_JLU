#pragma once

#include"GrammarAN.h"
#include <string>
struct procedure_ctx{
	int local_var_size;
	int next_temp_var_off;
	TreeNode * proc_node;
};

void genexp(TreeNode* t, procedure_ctx * ctx);
void gen(TreeNode* head);
void cgen(TreeNode* head,procedure_ctx * ctx);

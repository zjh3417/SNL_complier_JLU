#include"gencode.h"
#include "utils.h"
#include <assert.h>


int		line = 0;
string  code[0x1000];

/*
	假设基本类型最大为4个字节.... 

	由于参数不包含record 类型.所以参数最大是四个字节.
*/


/*				Stack Frame
	struct AR{
esp->		struct   	AR* prev_ar;	  //prev ar.
			uint32_t    func_identity;	  //procedure identity.
			char        local[];	      //[sp + 0x8 + offset]
			uint32      temp_var[];		  //临时变量....

ebp->		old_ebp
			ret address.

ebp + 0x8	arg0
			arg1
			arg2
	}
*/


void save_to_temp_var(procedure_ctx * ctx, int temp_var_off, string reg = "eax")
{
	code[line++] = "\tmov [esp + 0x8 + " + std::to_string(ctx->local_var_size + temp_var_off) + "], " + reg;
}

void load_from_temp_var(procedure_ctx * ctx, int temp_var_off,string reg = "eax")
{
	code[line++] = "\tmov " + reg + ", [esp + 0x8 + " + std::to_string(ctx->local_var_size + temp_var_off) + "]";
}


//获取变量的真实地址,结果存放到eax中.
void gen_get_var_address(TreeNode * t, procedure_ctx * ctx)
{
	//t->kind.exp == IdEK
	assert(t->kind.exp_type == IdEK || 
		t->kind.stmt_type == ReadK);

	int element_off = 0;

	if (t->attr.varAttr.type == record_declare){
		if (t->child[0]){
			code[line++] = "\tmov eax, " + std::to_string(t->child[0]->attr.varAttr.off);
			element_off = ctx->next_temp_var_off;
			ctx->next_temp_var_off += 0x4;
			save_to_temp_var(ctx, element_off);
		}
	}
	else if (t->attr.varAttr.type == array_declare){
		if (t->child[0]){									//如果有下标的话,再加上偏移
			genexp(t->child[0], ctx);						//计算出在数组内的偏移.
			code[line++] = "\tsub eax,1";					//snl下标从1开始....
			code[line++] = "\txor edx,edx";
			code[line++] = "\tmov ecx, " + std::to_string(t->attr.arrayAttr.element_size);
			code[line++] = "\tmul ecx";

			element_off = ctx->next_temp_var_off;
			ctx->next_temp_var_off += 0x4;
			save_to_temp_var(ctx, element_off);
		}
	}

	//压入变量偏移
	code[line++] = "\tmov eax," + std::to_string(t->attr.varAttr.off);
	code[line++] = "\tpush eax";


	//压入变量所在的函数ID
	code[line++] = "\tmov eax," + std::to_string(t->attr.varAttr.declare_proc_identity);
	code[line++] = "\tpush eax";

	//数组的话,先获取到数组的地址,在计算出元素地址.

	if (t->attr.varAttr.store_type == param_var){
		code[line++] = "\tcall find_param_var_address";

		//如果参数是数组的话，传进去的是地址.所以还要再取出数组的真实地址
		if (t->attr.varAttr.type == array_declare){
			code[line++] = "\tmov eax, [eax]";
		}
	}else{
		code[line++] = "\tcall find_local_var_address";
	}

	code[line++] = "\tadd esp,0x8";

	//如果是数组的话.
	if (t->attr.varAttr.type == array_declare){
		if (t->child[0]){
			load_from_temp_var(ctx, element_off, "ebx");
			code[line++] = "\tadd eax,ebx";
		}
	}

	if (t->attr.varAttr.type == record_declare){
		if (t->child[0]){
			load_from_temp_var(ctx, element_off, "ebx");
			code[line++] = "\tadd eax,ebx";
		}
	}
}



void genexp(TreeNode* t,procedure_ctx * ctx) {

	int temp_var_off_0 = 0;
	int temp_var_off_1 = 0;

	if (t->kind.exp_type == OpK) {

		//最后内容
		cgen(t->child[0], ctx);
		temp_var_off_0 = ctx->next_temp_var_off;
		ctx->next_temp_var_off += 0x4;
		save_to_temp_var(ctx, temp_var_off_0);
		//
		cgen(t->child[1], ctx);
		temp_var_off_1 = ctx->next_temp_var_off;
		ctx->next_temp_var_off += 0x4;
		save_to_temp_var(ctx, temp_var_off_1);

		//加载到寄存器
		load_from_temp_var(ctx, temp_var_off_0,"eax");
		load_from_temp_var(ctx, temp_var_off_1,"ebx");


		if (t->attr.expAttr.op == PLUS) {
			code[line++] = "\tadd eax, ebx";
		}
		else if (t->attr.expAttr.op == MINUS) {	//运算类型为减
			code[line++] = "\tsub eax, ebx";
		}
		else if (t->attr.expAttr.op == TIMES) {
			code[line++] = "\txor edx,edx";
			code[line++] = "\tmul ebx";
		}
		else if (t->attr.expAttr.op == OVER) {
			code[line++] = "\txor edx,edx";
			code[line++] = "\tdiv ebx";
		}
		else if (t->attr.expAttr.op == LT) {
			code[line++] = "\tcmp eax,ebx";
			code[line++] = "\tmov ebx,1";
			code[line++] = "\tmov eax,0";
			code[line++] = "\tcmovs eax,ebx";

		}else if (t->attr.expAttr.op == EQ) {
			code[line++] = "\tcmp eax,ebx";
			code[line++] = "\tmov ebx,1";
			code[line++] = "\tmov eax,0";
			code[line++] = "\tcmovz eax,ebx";
		}
		return;
	}

	if (t->kind.exp_type == ConstK) {				//常数
		code[line++] = "\tmov eax, " + std::to_string(t->attr.expAttr.val);				//
		return;
	}

	if (t->kind.exp_type == CharC){
		code[line++] = "\tmov eax, " + std::to_string(t->attr.expAttr.valc);				//
		return;
	}
	
	if (t->kind.exp_type == IdEK) {	
		int get_value = 1;
		int move_size = 4;
		
		gen_get_var_address(t, ctx);

		if (t->attr.varAttr.type == record_declare && !t->child[0]){
			get_value = 0;
		}
		else if(t->attr.varAttr.type == array_declare && !t->child[0]){
			get_value = 0;
		}
		
		if (t->attr.varAttr.type == char_declare){
			move_size = 1;
		}
		else if (t->attr.varAttr.type == record_declare &&
			t->child[0]){
			move_size = t->child[0]->attr.varAttr.size;
		}

	
		if (get_value){
			switch (move_size)
			{
			case 1:
				code[line++] = "\tmov al,byte ptr [eax]";
				code[line++] = "\tand eax,0xff";
				break;
			case 4:
				code[line++] = "\tmov eax,[eax]";
				break;
			default:
				break;
			}
		}
	}
	return;
}


//生成if 
void genif(TreeNode* t, procedure_ctx * ctx) {
	TreeNode* ift = t->child[0];
	cgen(ift,ctx);

	string else_label = string("if_") + generateUUID();
	string fi_label = string("if_") + generateUUID();
	
	code[line++] = "\ttest eax,eax";					//true != 0.
	code[line++] = "\tjz " + else_label;				//

	TreeNode* thent = t->child[1];
	cgen(thent,ctx);
	
	code[line++] = "\tjmp " + fi_label;
	code[line++] = else_label + ":";

	TreeNode* elset = t->child[2];
	cgen(elset,ctx);

	code[line++] = fi_label + ":";
	return;
}

//生成while 语句.
void genwhile(TreeNode* t, procedure_ctx * ctx) {
	TreeNode* while_condition = t->child[0];

	string loop_start_label = string("while_") + generateUUID();
	string loop_end_label = string("while_") + generateUUID();

	code[line++] = loop_start_label + ":";

	cgen(while_condition, ctx);

	code[line++] = "\ttest eax,eax";
	code[line++] = "\tjz " + loop_end_label;

	TreeNode* dot = t->child[1];
	cgen(dot, ctx);
	code[line++] = "\tjmp " + loop_start_label;
	code[line++] = loop_end_label + ":";
	return;
}


//生成赋值语句.
void genassign(TreeNode* t,procedure_ctx * ctx) {
	int temp_off_0 = 0;
	int temp_off_1 = 0;
	int move_size = 4 ;
	int temp_var_is_addr = 0;
	TreeNode * l = t->child[0];
	TreeNode * r = t->child[1];

	//左边一定是变量.
	gen_get_var_address(l , ctx);			//获取左边变量的地址.
	temp_off_0 = ctx->next_temp_var_off;
	ctx->next_temp_var_off += 0x4;
	save_to_temp_var(ctx, temp_off_0);

	//生成表达式右边的值.
	cgen(r, ctx);
	temp_off_1 = ctx->next_temp_var_off;
	ctx->next_temp_var_off += 0x4;
	save_to_temp_var(ctx, temp_off_1);

	//
	move_size = l->attr.varAttr.size;

	if (l->attr.varAttr.type == record_declare){
		if (l){
			move_size = l->child[0]->attr.varAttr.size;
		} else{
			move_size = l->attr.varAttr.size;
			temp_var_is_addr = 1;		//= 右侧的表达式返回的是地址
		}
	}

	if (l->attr.varAttr.type == array_declare){
		if (l->child[0]){
			move_size = l->attr.arrayAttr.element_size;
		} else{
			temp_var_is_addr = 1;		//= 右侧的表达式返回的是地址
			move_size = l->attr.arrayAttr.element_size * l->attr.varAttr.size;	//n * 元素大小.
		}
	}

	load_from_temp_var(ctx, temp_off_0, "edi");

	if (temp_var_is_addr){
		load_from_temp_var(ctx, temp_off_1, "esi");			//直接用move sb,	
	}
	else{
		code[line++] = "\tlea esi, [esp + 0x8 + " + std::to_string(ctx->local_var_size + temp_off_1) + "]";
	}

	code[line++] = "\tmov ecx," + std::to_string(move_size);
	code[line++] = "\trep movsb";
	return;
}



/*
	只支持读入:
		char 
		int.
*/

void genread(TreeNode* t, procedure_ctx * ctx) {
	
	//
	assert(t->attr.varAttr.type == char_declare ||
		t->attr.varAttr.type == int_declare);

	gen_get_var_address(t, ctx);
	code[line++] = "\tpush eax";
	if (t->attr.varAttr.type == char_declare){
		code[line++] = "\tcall read_char";
	} else if (t->attr.varAttr.type == int_declare){
		code[line++] = "\tcall read_integer";
	}
	code[line++] = "\tadd esp,0x4";
	return;
}

//生成write.
//write 右边的表达式.
//要实现:
// 1. 输出 char
// 2. 输出 int
// 3. 输出 array.
// 4. 输出 record不行.
// 5. 输出 address...

void genwrite(TreeNode* t, procedure_ctx * ctx) {
	
	TreeNode * expr = t->child[0];
	
	genexp(expr, ctx);

	if (expr->kind.exp_type == IdEK){
		//输出字符
		if (expr->attr.varAttr.type == char_declare){
			
			code[line++] = "push eax";
			code[line++] = "call print_char";
			code[line++] = "add esp, 0x4";
		}
		else if (expr->attr.varAttr.type == int_declare){
			
			code[line++] = "\tpush eax";
			code[line++] = "\tcall print_int";
			code[line++] = "\tadd esp, 0x4";
		}
		else if (expr->attr.varAttr.type == array_declare){
			//如果是数组名的话,就输出整个数组.
			if (!expr->child[0]){
				
				code[line++] = "\tmov ebx, " + std::to_string(expr->attr.varAttr.size / expr->attr.arrayAttr.element_size);	//数组元素个数.
				code[line++] = "\tpush ebx";
				code[line++] = "\tpush eax";

				if (expr->attr.arrayAttr.element_type == char_declare){
					code[line++] = "\tcall print_char_array";
					code[line++] = "\tadd esp,0x8";
				}
				else if (expr->attr.arrayAttr.element_type == int_declare){
					code[line++] = "\tcall print_int_array";
					code[line++] = "\tadd esp,0x8";
				}
			}else{		//就输出一个数组元素
				code[line++] = "\tpush eax";
				if (expr->attr.arrayAttr.element_type == char_declare){
					code[line++] = "\tcall print_char";
					code[line++] = "\tadd esp,0x4";
				}
				else if (expr->attr.arrayAttr.element_type == int_declare){
					code[line++] = "\tcall print_int";
					code[line++] = "\tadd esp,0x4";
				}
			}
		}
		else if (expr->attr.varAttr.type == record_declare){
			// //
			// if(expr->child[0]){
			// 	if(expr->child[0]->attr.varAttr.type == char_declare){
			// 		code[line++] = "\tpush eax";
			// 		code[line++] = "\tcall print_char";
			// 		code[line++] = "\tadd esp,0x4";
			// 	}else if(expr->child[0]->attr.varAttr.type == int_declare){
			// 		code[line++] = "\tpush eax";
			// 		code[line++] = "\tcall print_int";
			// 		code[line++] = "\tadd esp,0x4";
			// 	}
			// }
		}
	}
	else if (expr->kind.exp_type == ConstK){
		code[line++] = "\tpush eax";
		code[line++] = "\tcall print_int";
		code[line++] = "\tadd esp, 0x4";
	} else if (expr->kind.exp_type == OpK){		//运算结果都作为整数输出
		code[line++] = "\tpush eax";
		code[line++] = "\tcall print_int";
		code[line++] = "\tadd esp, 0x4";
	}

	return;
}


//生成 call

/*
	压栈. 
		1. 注意变量来源是 param还是局部变量.
		2. 传送几个字节
			参数可以是 char,int ,数组类型. 不能是record 类型


	注意传参次序.
*/


void gencall(TreeNode* t, procedure_ctx * ctx) {
	int param_size = 0;
	//0 是 符号名称.
	//code[line++] = "\tcall procedure_" + std::to_string(t->child[0]->attr.procAttr.proc_identity);

	//参数压栈,需要先计算表达式的值
	TreeNode * param_list = t->child[1];			//call 节点. 0 是 目标函数
													//1 是参数.
	TreeNode* var = t->child[1];
	stack<TreeNode* > args;
	vector<uint32_t> temp_arg_off;

	while (var != NULL) {
		param_size += 0x4;
		args.push(var);
		var = var->sibling;
	}

	while (!args.empty()){
		//计算参数表达式的值.
		TreeNode * arg_expr = args.top();
		args.pop();

		temp_arg_off.push_back(ctx->next_temp_var_off);
		ctx->next_temp_var_off += 0x4;
		genexp(arg_expr,ctx);

		save_to_temp_var(ctx, temp_arg_off.back());
	}
	
	//参数压栈.,由于local var 寻址是用到的esp,每一次push 之后esp都会变.
	//所以直接用mov操作..，然后调整esp.
	int arg_off = 0x4;
	for (int i = 0; i < temp_arg_off.size(); i++){
		load_from_temp_var(ctx, temp_arg_off[i]);							//把参数表达式最后的值放到eax
		code[line++] = "\tmov [esp - " + std::to_string(arg_off) + "], eax";
		arg_off += 0x4;
	}

	code[line++] = "\tsub esp, " + std::to_string(param_size);
	code[line++] = "\tcall procedure_" + std::to_string(t->child[0]->attr.procAttr.proc_identity);
	//清理堆栈
	code[line++] = "\tadd esp, " + std::to_string(param_size);

}

//生成返回
void genreturn(TreeNode* t, procedure_ctx * ctx) {
	code[line++] = "\tjmp " + string("procedure_") + std::to_string(ctx->proc_node->attr.procAttr.proc_identity) + "_ret";
	return;
}

//生成语句
void genstmt(TreeNode* t,procedure_ctx * ctx) {
	if (t->kind.stmt_type == IfK) {
		genif(t,ctx);
	}
	else if (t->kind.stmt_type == WhileK) {
		genwhile(t,ctx);
	}
	else if (t->kind.stmt_type == AssignK) {
		genassign(t,ctx);
	}
	else if (t->kind.stmt_type == ReadK) {
		genread(t,ctx);
	}
	else if (t->kind.stmt_type == WriteK) {
		genwrite(t,ctx);
	}
	else if (t->kind.stmt_type == CallK) {
		gencall(t,ctx);
	}
	else if (t->kind.stmt_type == ReturnK) {
		genreturn(t,ctx);
	}
	return;
}


//生成body.
void cgen(TreeNode* head, procedure_ctx * ctx) {
	
	if (head == NULL) 
		return;

	if (head->node_type == stms_node ){			//
		cgen(head->child[0],ctx);
	}else if (head->node_type == stm_node) {
		genstmt(head, ctx);
	}else if (head->node_type == expr_node) {
		genexp(head,ctx);
	}

	cgen(head->sibling,ctx);
	return;
}

//生成proc的代码.

void genproc(TreeNode* head) {			
	int current_proc_va = -1;

	procedure_ctx ctx;
	int param_size = 0;

	ctx.local_var_size = 0;
	ctx.next_temp_var_off = 0;
	ctx.proc_node = head;
	
	/*
		head:
			0: 形参,形参不需要管,采用类似cdecl的调用约定.
			1: 声明链表
			2: 过程体.
	*/

	//std::cout << "生成 proc " << head->name[0] << std::endl;
			
	TreeNode* t = head->child[1];				//所有声明
	
	//计算局部变量的空间.
	while (t){
		if (t->node_type == var_declare_node){			
			TreeNode * var = t->child[0];
			while (var){

				ctx.local_var_size += var->attr.varAttr.size;
				var = var->sibling;
			}
		}
		t = t->sibling;
	}
	//计算参数大小.

	t = head->child[0];
	while (t){
		param_size += 0x4;
		t = t->sibling;
	}

	//生成内部定义的其他procedure.
	t = head->child[1];
	while (t){
		if (t->node_type == procedure_declare_node){
			genproc(t);
		}
		t = t->sibling;
	}
	

	//先计算出声明中的占用大小.
	//然后再加上临时变量需要的空间

	current_proc_va = line;

	code[line++] = "procedure_" + std::to_string(head->attr.procAttr.proc_identity) + ":";
	code[line++] = "\t[]";
	code[line++] = "\tpush ebp";
	code[line++] = "\tmov ebp,esp";
	code[line++] = "\t[]";
	code[line++] = "\tmov eax, " + std::to_string(head->attr.procAttr.proc_identity);
	code[line++] = "\tpush eax";
	code[line++] = "\tsub esp,0x4";
	code[line++] = "\tmov [esp],ebx";

	cgen(head->child[2],&ctx);					//

	code[line++] = "procedure_" + std::to_string(head->attr.procAttr.proc_identity) + "_ret:";
	code[line++] = "\tmov esp,ebp";
	code[line++] = "\tpop ebp";
	code[line++] = "\tret";

	//填写局部变量大小和临时变量大小.
	code[current_proc_va + 4] = "\tsub esp, " + std::to_string(ctx.local_var_size + ctx.next_temp_var_off);
	code[current_proc_va + 1] = "\tlea ebx, [esp + 0x4 + " + std::to_string(param_size) + ']';

	//
	code[line++] = "\n";
	code[line++] = "\n";
	return;
}


//根节点,直接把program 当成一个procedure.

void gen(TreeNode* head) {
	head->name.push_back("program");
	head->child[0] = 0;		//没用.

	code[line++] = ".global procedure_0";
	code[line++] = ".intel_syntax noprefix";
	code[line++] = ".text";

	genproc(head);
	code[line++] = "\n";
	//输出代码

	for (int i = 0; i < line; i++){
		std::cout << code[i] << std::endl;
	}
}
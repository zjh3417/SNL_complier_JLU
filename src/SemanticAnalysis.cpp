#include "SemanticAnalysis.h"
#include "utils.h"

//符号表的实现
symbtable Scope[10000]; //符号表

int  level = 0;			//语义分析阶段当前的 level.
bool flagtree=false;	

uint32_t        local_var_off[10000];								//记录dfs中, 当前层的下一个变量声明开始的偏移. 
uint32_t		param_var_off[10000];								//参数类型的变量.所在的偏移.
uint32_t        level_proc_identity[10000];							//保存每一个level的过程id.

uint32_t		proc_identity = 0;									//未使用的id


//基本类型.
typeIR* intPtr, *charPtr, *boolPtr;


void CreatTable(uint32_t identity) {
	level++;

	local_var_off[level] = 0;							//下一个变量的偏移,初始化为0
	param_var_off[level] = 0;
	level_proc_identity[level] = identity;
}

//撤销一个符号表
void DestroyTable() {
	Scope[level].next = nullptr;
	level = level - 1;
}

//在currentLevel 对应的符号表中搜索id.
symbtable* SearchoneTable(string id, int currentLevel) {
	symbtable* t = Scope[currentLevel].next;

	while (t != nullptr) {                    
		if (t->idname == id) {
			return t;
		}
		else {
			t = t->next;
		}
	}
	return NULL;
}

//符号表中查找标识符：
symbtable* FindEntry(string id, bool search_all_level) {
	/*
		flag == false,只在当前层搜索
		flag == true,会在之前的所有层搜索
	*/
	symbtable* entry = NULL;
	if (search_all_level) {
		for (int i = level; i > 0; i--){
			entry = SearchoneTable(id, i);
			if (entry)
				return entry;
		}
		return NULL; 
	}
	
	return SearchoneTable(id, level);
}

//登记标识符和属性到符号表
symbtable* Enter(string Id, AttributeIR* AttribP) {

	symbtable * entry = NULL;

	entry = FindEntry(Id, false);

	if (entry) {
		std::cerr <<"标识符重复声明"<<endl;
		exit(1);
	}
	else {                                                
		
		entry = new symbtable;
		entry->idname = Id;
		entry->attrIR = *AttribP;
		
		if (Scope[level].next == nullptr){
			Scope[level].next = entry;
		}
		else {
			symbtable* t = Scope[level].next;
			while (t->next != nullptr){
				t = t->next;
			}
			t->next = entry;
		}
	}
	return entry;
}

//在域表中查找域名
bool FindField(string Id, fieldChain* head, fieldChain** Entry) {
	fieldChain* r = head;
	
	while (r != nullptr) {
		
		if (Id == r->idname){
			(*Entry) = r;
			return true;
		}

		r = r->next;
	}
	return false;
}


static char * szTypeKind[] = {
	"intTy",
	"charTy",
	"arrayTy",
	"recordTy",
	"boolTy"
};

//输出符号表
void OutputSymbols(string path) {
	ofstream ouput;
	ouput.open(path);

	for (int i = 1; i < level+1; i++) {

		symbtable* t = Scope[i].next;

		while (t != nullptr) {
			//有选择地输出一些符号表信息项
			if (t->attrIR.kind == typeKind){
				ouput << t->idname << " " << "typeKind" << " " << szTypeKind[t->attrIR.idtype->kind] << " " << endl;
			}
			else if (t->attrIR.kind == varKind){
				if (t->attrIR.idtype != nullptr){
					ouput <<
						t->idname << " " << "varKind" << " " <<
						szTypeKind[t->attrIR.idtype->kind] << " " <<
						t->attrIR.More.var_attr.level << " " <<
						t->attrIR.idtype->size << " " <<
						t->attrIR.More.var_attr.off << endl;
				}					
				else{
					ouput
						<< t->idname << " " << "varKind" << " "
						<< &t->attrIR << " " << endl;
				}
			}
			else {
				ouput 
					<< t->idname << " " << "procKind" << " " << "intTy" << " " 
					<< t->attrIR.More.ProcAttr.level << endl;
			}

			t = t->next;
		}
	}
	ouput.close();
}



void initializeTypeInfo() {
	intPtr = new typeIR;
	intPtr->size = 4;
	intPtr->kind = intTy;

	charPtr = new typeIR;
	charPtr->size = 1;
	charPtr->kind = charTy;
	
	boolPtr = new typeIR;
	boolPtr->size = 1;
	boolPtr->kind = boolTy;
}

typeIR* nameType(TreeNode* t) {
	symbtable* entry = NULL;
	int i = 0;
	if (flagtree)
		i++;

	entry = FindEntry(t->name[i], false);

	if (!entry) {
		cout << t->lineno << "自定义类型" + t->name[i] + "未声明" << endl;
	}else {
		return entry->attrIR.idtype;
	}
	return nullptr;
}

typeIR* arrayType(TreeNode* t) {
	if (t->attr.arrayAttr.low > t->attr.arrayAttr.up) {
		cout << "数组下表越界"<<endl;
	}
	else {
		typeIR* newarrayType = new typeIR;
		newarrayType->kind = arrayTy;
		newarrayType->size = t->attr.arrayAttr.up - t->attr.arrayAttr.low + 1;
		newarrayType->More.ArrayAttr.indexTy = intPtr;
		
		if (t->attr.arrayAttr.element_type == char_declare)
			newarrayType->More.ArrayAttr.elemTy = charPtr;
		else 
			newarrayType->More.ArrayAttr.elemTy = intPtr;
		
		newarrayType->size *= newarrayType->More.ArrayAttr.elemTy ->size;
		return newarrayType;
	}
	return nullptr;
}

void outputbody(fieldChain* bodyt) {
	if (bodyt == nullptr)
		return;
	else {
		cout <<"  "<< bodyt->idname;
		outputbody(bodyt->next);
		return;
	}
}

typeIR* recordType(TreeNode* t) {
	
	typeIR* newrecordType = new typeIR;
	newrecordType->kind = recordTy;

	fieldChain* newbody = nullptr,*dt = nullptr;
	TreeNode* nt = t->child[0];
	int recordoffset = 0;

	newrecordType->size = 0;

	while (nt != nullptr) {
		for (int i = 0; i < nt->name.size(); i++) {

			if (newbody != nullptr) {
				fieldChain* Entry= NULL;
				bool flag = FindField(nt->name[i], newbody, &Entry);
				if (flag)
					cout << t->lineno << "record重命名"<<endl;
			}

			fieldChain* newt = new fieldChain;

			if (newbody == nullptr)
				newbody = newt;
			else {
				dt->next=newt;
			}
		
			newt->unitType = TypeProcess(nt);
			newt->idname = nt->name[i];
			newt->offset = recordoffset;
			
			newrecordType->size += newt->unitType->size;

			recordoffset += newt->unitType->size;
			newt->next = nullptr;
			dt = newt;
		}

		nt = nt->sibling;
	}
	dt->next = nullptr;
	newrecordType->More.body = newbody;

	//cout << newbody->next;
	//outputbody(newbody);cout<<endl;
	return newrecordType;
}


typeIR* TypeProcess(TreeNode* t) {

	if (t->kind.dec_type == int_declare) {
		return intPtr;
	}

	if (t->kind.dec_type == char_declare) {
		return charPtr;
	}

	if (t->kind.dec_type == array_declare) {
		return arrayType(t);
	}

	if (t->kind.dec_type == record_declare) {
		return recordType(t);
	}

	if (t->kind.dec_type == type_declare) {
		return nameType(t);
	}

	return nullptr;
}


void ProgramProcess(TreeNode * t){

	/*
		program
		child[0] = PheadK
		child[1] = 类型声明 -> 变量声明 -> 过程声明
		child[2] = 程序体
	*/
	t->attr.procAttr.proc_identity = proc_identity++;

	CreatTable(t->attr.procAttr.proc_identity);

	process(t->child[0]);
	process(t->child[1]);
	process(t->child[2]);
}

//类型声明部分分析处理函数
void TypedecProcess(TreeNode* t) {
	/*
		需要做的是
			1，找是否已经定义
			2，构造其对应类型的内部表示3，加入符号表
	*/
	if (t == NULL)
		return;

	for (int i = 0; i < t->name.size(); i++) {
		string id = t->name[i];

		symbtable* entry = FindEntry(id, false);

		if (entry) {
			std::cerr << 'line' << t->lineno << ':' << "重复声明类型：" << id << endl;
			exit(1);
		} else {
			//没找到, 构造类型的内部表示
			typeIR* typeinfo = TypeProcess(t);
			
			//填入标识符属性、
			AttributeIR att;
			att.idtype = typeinfo;
			att.kind = typeKind;

			//如果是数组的话, 要给att添加内容
			//如果是结构体的话
			Enter(t->name[i], &att);
		}
	}
	
	TypedecProcess(t->sibling);
	return;
}

//变量声明部分分析处理函数
/*
	变量的声明只需要记录下大小即可.
*/

void VardecProcess(TreeNode* t) {
	//处理变量声明
	if (t == NULL)
		return;
	
	int var_size = 0;
	int var_off = local_var_off[level];

	for (int i = 0; i < t->name.size(); i++) {
		string id = t->name[i];
		symbtable*  entry = NULL;

		entry = FindEntry(id, false);

		if (entry){
			std::cerr << 'line' << t->lineno << ':' << "重复声明变量：" << id << endl;
			exit(1);
		}
		
		//还是加入符号表
		//先创建内部表示
		typeIR* typeinfo = TypeProcess(t);
			
		if (!typeinfo){
			std::cerr << 'line' << t->lineno << ':' << "未定义的类型：" << id << endl;
			exit(1);
		}

		AttributeIR att;
		att.idtype = typeinfo;
		att.kind = varKind;

		att.More.var_attr.store_type = local_var;
		att.More.var_attr.level = level;
		att.More.var_attr.declare_proc_identity = level_proc_identity[level];			//当前层的函数id.
		att.More.var_attr.off = local_var_off[level];
		att.More.var_attr.type = t->kind.dec_type;									//保存变量类型.
		att.More.var_attr.size = att.idtype->size;

		//数组类型的话,还需要保存元素大小...
		if (t->kind.dec_type == array_declare){
			t->attr.arrayAttr.element_size = typeinfo->More.ArrayAttr.elemTy->size;

			att.More.arr_attr.up = t->attr.arrayAttr.up;
			att.More.arr_attr.low = t->attr.arrayAttr.low;
			att.More.arr_attr.element_size = t->attr.arrayAttr.element_size;
			att.More.arr_attr.element_type = t->attr.arrayAttr.element_type;
		}

		//
		local_var_off[level] += align(att.idtype->size);
		var_size += align(att.idtype->size);
		//
		Enter(t->name[i], &att);			//登记符号信息.
		
	}

	//往语法树的节点中加入信息.
	t->attr.varAttr.off = var_off;
	t->attr.varAttr.declare_proc_identity = level_proc_identity[level];
	t->attr.varAttr.type = t->kind.dec_type;
	t->attr.varAttr.store_type = local_var;
	t->attr.varAttr.size = var_size;
	t->attr.varAttr.level = level;

	VardecProcess(t->sibling);
	return;
}


//形参分析处理函数
ParamTable* ProcheadProcess(TreeNode* t) {
	ParamTable* parat = nullptr;
	ParamTable* paralist = nullptr, *dt = nullptr;
	int var_size = 0;
	int var_off = param_var_off[level];

	if (t == nullptr)
		return nullptr;

	for (int i = 0; i < t->name.size(); i++) {

		symbtable* entry = NULL;
		entry = FindEntry(t->name[i], false);
		
		if (entry){
			std::cerr << t->lineno << "形参标识符" + t->name[i] + "重复声明" << endl;
			exit(1);
		} 

		typeIR* typeinfo = TypeProcess(t);

		AttributeIR att;
		att.kind = varKind;
		att.idtype = typeinfo;

		att.More.var_attr.store_type = param_var;
		att.More.var_attr.level = level;
		att.More.var_attr.off = param_var_off[level];
		att.More.var_attr.declare_proc_identity = level_proc_identity[level];
		att.More.var_attr.type = t->kind.dec_type;
		att.More.var_attr.size = att.idtype->size;

		//数组类型的话,还需要保存元素大小...
		if (t->kind.dec_type == array_declare){
			t->attr.arrayAttr.element_size = typeinfo->More.ArrayAttr.elemTy->size;

			att.More.arr_attr.up = t->attr.arrayAttr.up;
			att.More.arr_attr.low = t->attr.arrayAttr.low;
			att.More.arr_attr.element_size = t->attr.arrayAttr.element_size;
			att.More.arr_attr.element_type = t->attr.arrayAttr.element_type;
		}

		//更新偏移,四字节对齐大小.
		param_var_off[level] += align(att.idtype->size);
		var_size += align(att.idtype->size);
			
		entry = Enter(t->name[i], &att);
			
		parat = new ParamTable;
			
		if (paralist == nullptr)
			paralist = parat;
		else 
			dt->next = parat;
			
		parat->entry = entry;
		parat->next = nullptr;
		dt = parat;
		
	}

	//往语法树的节点中加入信息.
	t->attr.varAttr.off = var_off;
	t->attr.varAttr.declare_proc_identity = level_proc_identity[level];
	t->attr.varAttr.type = t->kind.dec_type;
	t->attr.varAttr.store_type = param_var;
	t->attr.varAttr.size = var_size;
	t->attr.varAttr.level = level;

	//
	dt->next= ProcheadProcess(t->sibling);
	return paralist;
}


//过程声明部分分析处理函数
void ProcdecProcess(TreeNode* t) {
	if (t == NULL)
		return;

	//处理函数头
	//将函数名加入符号表
	symbtable* entry = NULL;

	entry = FindEntry(t->name[0], false);

	if (entry) {
		std::cerr 
			<< 'line' 
			<< t->lineno 
			<< ':' 
			<< "函数名已被使用：" 
			<< t->name[0]
			<< endl;

		exit(1);

	} else {

		//处理程序头
		AttributeIR att;
		att.idtype = NULL;
		att.kind = procKind;

		ParamTable* paramtb = new ParamTable;
		
		att.More.ProcAttr.level = level;
		att.More.ProcAttr.param = paramtb;
		att.More.ProcAttr.proc_identity = proc_identity++;			//过程id

		//在语法树中加入过程id
		t->attr.procAttr.proc_identity = att.More.ProcAttr.proc_identity;

		entry = Enter(t->name[0], &att);

		//开始处理下一level的内容
		CreatTable(t->attr.procAttr.proc_identity);

		//处理形参
		entry->attrIR.More.ProcAttr.param = ProcheadProcess(t->child[0]);
		
		//处理声明
		process(t->child[1]);

		BodyProcess(t->child[2]->child[0]);

		DestroyTable();
		return;
	}
}

//表达式处理,返回表达式的类型.
//
typeIR* Expr(TreeNode* t) {

	//1. 是标识符.
	if (t->kind.exp_type == IdEK) {
		symbtable* entry = NULL;
		entry = FindEntry(t->name[0], true);
		if (!entry){
			std::cerr << t->lineno << "表达式语句中" + t->name[0] + "未声明" << endl;
			exit(1);
		}
		
		//假设 都是变量.,存储变量的起始地址.
		t->attr.varAttr.size = entry->attrIR.idtype->size;
		t->attr.varAttr.off = entry->attrIR.More.var_attr.off;
		t->attr.varAttr.declare_proc_identity = entry->attrIR.More.var_attr.declare_proc_identity;
		t->attr.varAttr.type = entry->attrIR.More.var_attr.type;
		t->attr.varAttr.store_type = entry->attrIR.More.var_attr.store_type;
		t->attr.varAttr.level = entry->attrIR.More.var_attr.level;

		if (entry->attrIR.idtype->kind == arrayTy){
			t->attr.arrayAttr.element_size = entry->attrIR.More.arr_attr.element_size;
			t->attr.arrayAttr.element_type = entry->attrIR.More.arr_attr.element_type;
			t->attr.arrayAttr.low = entry->attrIR.More.arr_attr.low;
			t->attr.arrayAttr.up = entry->attrIR.More.arr_attr.up;
		}

		//有下标,或者是record.....
		if (t->child[0] != nullptr) {

			if (entry->attrIR.idtype->kind == arrayTy){					 //数组类型返回元素的内部表示的kind
				typeIR * expr = Expr(t->child[0]);						 //多维数组吗????	
				if (expr != intPtr) {
					std::cerr << t->lineno << "数组索引不是整数" << endl;
					exit(1);
				}
				//返回元素类型.
				return entry->attrIR.idtype->More.ArrayAttr.elemTy;

			}else if(entry->attrIR.idtype->kind == recordTy) {
				fieldChain*field = entry->attrIR.idtype->More.body;
				while (field != nullptr){
					if (field->idname == t->child[0]->name[0]){

						//拷贝attr,
						t->child[0]->attr = t->attr;

						//把offset 和 类型改成相对于整个record
						t->child[0]->attr.varAttr.off = field->offset;
						t->child[0]->attr.varAttr.size = field->unitType->size;
						//t->child[0]->attr.varAttr.type = field->unitType->kind;
						return field->unitType;
					}
					
					field = field->next;
				}
				std::cerr << t->lineno << "record中没有此元素"<<endl;
				exit(1);
			}
			
		} else {
			//普通类型 : int ,char ,bool,数组类型,recordType....
			//看起来是 支持给整个recordType赋值的...
			return entry->attrIR.idtype;
		}
	}
	else if (t->kind.exp_type == ConstK) {			//2. 是常数
		return intPtr;
	}
	else if (t->kind.exp_type == CharC){
		return charPtr;
	}
	else if (t->kind.exp_type == OpK){
		//3. 是运算符.
		typeIR* a = Expr(t->child[0]);
		typeIR* b = Expr(t->child[1]);

		if (a == b && a && b) {					//proc符号不能被赋值.
			return a;
		} 

		std::cerr << t->lineno << "表达式语句类型不匹配" << endl;
		exit(1);
	}
	return nullptr;
}

//赋值语句.
void assignstatement(TreeNode* t) {

	if (t->child[0]->kind.exp_type != IdEK){
		std::cerr << t->lineno << "赋值语句左端不是标识符" << endl;			//其实数组名也不能被赋值.
		exit(1);
	} 

	typeIR* kindleft  = Expr(t->child[0]);				//获取表达式左边的类型.
	typeIR *kindright = Expr(t->child[1]);		//

	if (kindleft != kindright){
		std::cerr << t->lineno << "赋值语句左右类型不匹配" << endl;
		exit(1);
	}
}


//过程调用语句分析处理函数
void callstatement(TreeNode* t) {
	/*
		child[0] : proc name
		child[1] : param info
	*/

	symbtable* entry = FindEntry(t->child[0]->name[0], true);

	if (!entry){
		std::cerr << t->lineno << "函数" + t->child[0]->name[0] + "未声明" << endl;
		exit(1);
	} 

	if (entry->attrIR.kind != procKind){
		std::cerr << t->lineno << "标识符不是函数类型，无法调用" << endl;
		exit(1);
	}

	t->child[0]->attr.procAttr.proc_identity = entry->attrIR.More.ProcAttr.proc_identity;

	//如果没有参数的话,直接返回.
	if (t->child[1] == nullptr && entry->attrIR.More.ProcAttr.param == nullptr) {
		return;
	}

	//到过程的参数信息。
	ParamTable* a = entry->attrIR.More.ProcAttr.param;
	TreeNode  * b = t->child[1];
			
	while(a && b) {
		typeIR * bt = Expr(b);
				
		if (a->entry->attrIR.idtype != bt) {
			
			if (bt->kind == arrayTy &&  a->entry->attrIR.idtype->kind == arrayTy) {
						
				if (bt->size != a->entry->attrIR.idtype->size ||
					bt->More.ArrayAttr.elemTy != a->entry->attrIR.idtype->More.ArrayAttr.elemTy)
				{
					std::cerr << t->lineno << "参数不匹配" << endl;
					exit(1);
				}
			}
			else {
				std::cerr << t->lineno << "参数不匹配" << endl;
				exit(1);
			}
		}
	
		//下一个参数.
		b = b->sibling;
		a = a->next;
	}

	if (a || b ) {
		std::cerr << t->lineno << "参数不匹配" << endl;
		exit(1);
	}
	return;
}
                                                                     
void ifstatment(TreeNode* t) {
	//if语法树child[0]为if表达式,且为操作符
	TreeNode* ift = t->child[0];

	Expr(ift);

	//then 语句
	TreeNode* elset = t->child[1];
	while (elset) {
		statement(elset);
		elset = elset->sibling;
	}
	

	//else 语句
	TreeNode* thent = t->child[2];
	while (thent) {
		statement(thent);
		thent = thent->sibling;
	}
	return;
}

void whilestatement(TreeNode* t) {
	//while语法树child[0]为if表达式,同if
	TreeNode* whilet = t->child[0];

	Expr(whilet);

	TreeNode* dot = t->child[1];

	while (dot != nullptr) {
		statement(dot);
		dot = dot->sibling;
	}
}


//读语句分析处理函数
void readstatement(TreeNode* t) {
	symbtable* entry = FindEntry(t->name[0], true);

	if (!entry){
		std::cerr << t->lineno << "读语句标识符" + t->name[0] + "未声明" << endl;
		exit(1);
	}	

	if (entry->attrIR.More.var_attr.type == array_declare){
		std::cerr << t->lineno << "数组无法作为读语句参数" << endl;
		exit(1);
	}
	if (entry->attrIR.More.var_attr.type == record_declare){
		std::cerr << t->lineno << "record无法作为读语句参数" << endl;
		exit(1);
	}

	//符号信息.
	t->attr.varAttr = entry->attrIR.More.var_attr;
	return;
}

void writestatement(TreeNode* t) {
	Expr(t->child[0]);
	return;
}

void returnstatment(TreeNode* t) {
	return;
}


void statement(TreeNode* t) {
	switch (t->kind.stmt_type)
	{
	case IfK:
		ifstatment(t);
		break;
	case WhileK:
		whilestatement(t);
		break;
	case AssignK:
		assignstatement(t);
		break;
	case ReadK:
		readstatement(t);
		break;
	case WriteK:
		writestatement(t);
		break;
	case CallK:
		callstatement(t);
		break;
	case ReturnK:
		returnstatment(t);
		break;
	}
	return;
}


//一系列语句.
void BodyProcess(TreeNode* t) {
	if (t == NULL)
		return;

	while (t != NULL) {
		statement(t);
		t = t->sibling;
	}
}



typeIR* arrayVar(TreeNode* t) {
	return nullptr;
}

typeIR* recordVar(TreeNode* t) {
	return nullptr;
}


void process(TreeNode* t) {

	if (t == nullptr)
		return;

	switch (t->node_type)
	{
	case program_node:
		ProgramProcess(t);
		break;
	
	case program_head_node:			//do nothing....
		break;

	/* ************************* 声明   **************************/
	case type_declare_node:
		TypedecProcess(t->child[0]);
		process(t->sibling);
		break;

	case var_declare_node:
		VardecProcess(t->child[0]);
		process(t->sibling);
		break;

	case procedure_declare_node:
		ProcdecProcess(t);
		process(t->sibling);			
		break;

	/* ************************* 语句   **************************/
	case stms_node:
	case stm_node:
		BodyProcess(t->child[0]);
		break;

	default:
		break;
	}
}


//语义分析主程序
void Analyze(TreeNode* root) {

	initializeTypeInfo();
	
	process(root);

	OutputSymbols("symbtable.txt");
}

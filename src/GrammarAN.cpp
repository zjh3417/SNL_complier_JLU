
#include"GrammarAN.h"

stack<string> SymStack;
stack<TreeNode*> opSymStack;
stack<TreeNode*> OpNumStack;
stack<TreeNode**> STStack;
vector<vector<string>> productSet;
vector<vector<string>> predictSet;
vector<vector<int>> LL1Table;
set<string> VT;	//终极符
set<string> VN;	//非终极符
TreeNode* currentP;
TreeNode* saveP;
DeclareType* temp;
bool getExpResult = true;
bool getExpResult2 = false;
int expflag = 0;
Token token;       //读入的token
string lastword;     //用于记录上一条语句类型
bool isinProc = 0;      //用于在写入语法树时记录是否在函数中
string table1 = "    ";
int ExpOp = 0;   //记录Exp的table
int ExpNum = 0;     //记录操作数
TreeNode* Prok = new TreeNode();    //语法树的根节点
int expchar = 0;

string StringToken[] = {
	"ID", "IF", "BEGIN", "INTC", "END", "PLUS", "MINUS", "TIMES",
	"OVER", "EQ", "LT", "LMIDPAREN", "RMIDPAREN", "DOT"
	, "TYPE", "VAR", "PROCEDURE", "PROGRAM", "SEMI", "INTEGER",
	"CHAR", "ARRAY", "RECORD", "UNDERANGE"
	, "OF", "COMMA", "LPAREN", "RPAREN", "ENDWH", "WHILE", "RETURN",
	"READ", "WRITE", "ASSIGN", "THEN", "FI"
	, "ELSE", "DO", "CHARC"
};
TreeNode* parseLL() {
	//cout << "1" << endl;
	CreateLL1Table();
	//cout << "1" << endl;
	string start = "Program";
	SymStack.push(start);

	Prok->node_type = program_node;    //首节点只需赋值这些
	Prok->lineno = 1;

	for (int i = 2; i >= 0; i--)
		STStack.push(&(*Prok).child[i]);      //传的是指针的地址

	string ss, s, line;
	int linenumber = 0;
	ifstream tokenfile("token.txt", ifstream::in);
	if (!tokenfile.is_open())
		exit(-3);			//-3代表文件打开失败

	//cout << "1" << endl;
	getline(tokenfile, line);
	linenumber++;


	istringstream cinline(line);
	cinline >> token.lineShow;

	string tmpss;
	cinline >> tmpss;
	token.lex = getLexType(tmpss);
	
	cinline >> token.sem;

	/*cout << tmpss << endl;
	cout << token.lineShow << endl;
	cout << token.sem << endl;*/

	while (!SymStack.empty()) {
		//cout << "!SymStack.empty()" << endl;
		string stacktop = SymStack.top();
		//cout << stacktop << endl;
		//SymStack.pop();
		int con = 0;
		for (auto it : VN) {
			if (it == stacktop)
				con++;
		}

		if (con != 0) {         //符号栈顶为非终极符
			int llx = 0, lly = 0;
			for (auto it : VN) {
				if (it == stacktop) {
					//cout << "VN[llx]:" << it << endl;;
					break;
				}
				llx++;
			}
			for (auto it : VT) {
				if (it == StringToken[(int)token.lex]) {
					//cout << "VT[lly]:" << it << endl;;
					break;
				}
				lly++;
			}
			//cout << VN.size() << " " << VT.size() << endl;
			//cout << llx << " " << lly << endl;

			int pnum = LL1Table[llx][lly];
			SymStack.pop();
			//cout << "Predict:" << pnum << endl;
			Predict(pnum);
		}
		else {                 //符号栈顶为终极符
			int kk = (int)token.lex;
			//cout << "终极符匹配" << kk << " " << stacktop << " " << StringToken[kk] << endl;
			if (compareIgnoreCase(stacktop, StringToken[kk]) == 0) {       //终极符匹配
				SymStack.pop();
				line.clear();
				getline(tokenfile, line);
				if (line == "")
					break;

				//cout << line << endl;
				istringstream cinline(line);
				cinline >> token.lineShow;
				string tmpss;
				cinline >> tmpss;
				//cout << "tmpss:" << tmpss << "?" << endl;
				token.lex = getLexType(tmpss);
				cinline >> token.sem;
				//linenumber++;
				//cout << SymStack.empty() << endl;
			}
			else {
				cout << token.lineShow << ":语法错误：终极符匹配失败" << endl;
				cout << "stacktop:" << stacktop << " " << "StringToken[kk]：" << StringToken[kk] << endl;
				exit(-4);     //-4代表终极符不匹配
			}
		}
	}
	if (tokenfile.eof()) {
		BuildLL1tree(Prok);
		tokenfile.close();
		return Prok;
	}
	else {
		cout << linenumber << ":语法错误：Token不为文件结束符" << endl;
		exit(-5);    //-5代表提前结束

	}
}


int compareIgnoreCase(string str1, string str2) {
	transform(str1.begin(), str1.end(), str1.begin(), ::tolower);  //0相等
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	return str1.compare(str2);
}

void BuildLL1tree(TreeNode* root) {
	stack<TreeNode*>TreeStack;
	string table = "";
	TreeStack.push(root);
	//cout << "BuildLL1tree" << endl;
	ofstream tree;
	tree.open("LL1Tree.txt");

	while (!TreeStack.empty()) {
		TreeNode* tmp = TreeStack.top();
		TreeStack.pop();
		if (tmp->node_type == program_node) {
			tree << "ProK" << endl;
			table = table + "    ";
			lastword = "ProK";
		}
		else if (tmp->node_type == program_head_node) {
			tree << table << "program_head_node" << " " << tmp->name[0] << endl;
			lastword = "Phead";
		}
		else if (tmp->node_type == type_declare_node) {
			if (lastword != "program_head_node")table = table + "    ";
			tree << table << "Typek" << endl;

			//table = table.substr(0, table.size() - 4);
			lastword = "type_declare_node";
		}
		else if (tmp->node_type == declare_node) {

			table = table + "    ";
			tree << table << "declare_node" << " ";
			if (lastword == "ProcDeck") {
				if (tmp->attr.procAttr.paramt == Valparamtype) {    //为函数值参的情况
					tree << "Valparamtype:" << " ";
				}
				else if (tmp->attr.procAttr.paramt == Varparamtype) {   //为函数变参的情况
					tree << "Varparamtype:" << " ";
				}
			}
			tree << getDeckEnumString(tmp->kind.dec_type);
			if (tmp->kind.dec_type == array_declare) {
				tree << " " << getDeckEnumString(tmp->attr.arrayAttr.element_type) << " " << tmp->attr.arrayAttr.low << " " << tmp->attr.arrayAttr.up;
			}
			else if (tmp->kind.dec_type == type_declare) {    //自定义节点
				tree << tmp->type_name;
			}
			//else if (tmp->kind.dec_type == RecordK) {                       //record
			//	//tree<<" "<<
			//}

			for (int i = 0; i < tmp->idnum; i++)
				tree << " " << tmp->name[i];
			tree << endl;
			table = table.substr(0, table.size() - 4); //进入函数后多一个table
			//lastword = "declare_node";
		}
		else if (tmp->node_type == var_declare_node) {
			//table = table + "    ";
			tree << table << "VarK" << endl;
			//table = table.substr(0, table.size() - 4);
			lastword = "VarK";
		}
		else if (tmp->node_type == procedure_declare_node) {
			tree << "ProcDeck" << " " << tmp->name[0] << endl;
			lastword = "ProcDeck";
			isinProc = 1;
		}
		else if (tmp->node_type == stms_node) {
			if (isinProc) {
				table1 = "        ";
				isinProc = 0;
			}
			else table1 = "    ";
			table = table1;
			tree << table << "StmLK" << endl;
		}
		else if (tmp->node_type == stm_node) {
			table = table1 + "    ";
			ExpOp = ExpNum = 0;
			if (tmp->kind.stmt_type == ReadK)
				tree << table << "StmtK" << "    " << enumkindToString(tmp->kind.stmt_type) << "    " << tmp->name[0] << endl;
			else
				tree << table << "StmtK" << "    " << enumkindToString(tmp->kind.stmt_type) << endl;

		}
		else if (tmp->node_type == expr_node) {
			table = table + "    ";
			tree << table << "ExpK" << "    ";
			if (tmp->kind.exp_type == OpK) {
				if (ExpOp == 0)ExpNum = 0;
				ExpOp++;
				tree << "Op" << " " << StringToken[tmp->attr.expAttr.op] << endl;
				table = table + "    ";
			}
			else if (tmp->kind.exp_type == ConstK) {
				ExpNum++;
				tree << "Const" << "    " << tmp->attr.expAttr.val << endl;
				if (ExpNum - 1 == ExpOp&&ExpOp != 0)table = table.substr(0, table.size() - 4);
			}
			else if (tmp->kind.exp_type == CharC) {
				tree << "Charc" << "    " << tmp->attr.expAttr.valc << endl;
			}
			else if (tmp->kind.exp_type == IdEK) {
				ExpNum++;
				if (tmp->attr.expAttr.varkind == IdV) {
					tree << "IdV" << "    " << tmp->name[0] << endl;;
				}
				else if (tmp->attr.expAttr.varkind == ArrayMembV) {
					tree << "ArrayMembV" << "    " << tmp->name[0] << endl;
				}
				else if (tmp->attr.expAttr.varkind == FieldMembV) {
					tree << "FieldMembv" << "    " << tmp->name[0] << endl;;
				}
				else {
					cout << "语法错误:出现错误的表达式类型" << endl;
					exit(-10);
				}
				if (ExpNum - 1 == ExpOp && ExpOp != 0)table = table.substr(0, table.size() - 4);
			}
			else {
				cout << "语法错误:出现错误的表达式" << endl;
				exit(-11);
			}

			table = table.substr(0, table.size() - 4);

		}
		else;


		if (tmp->sibling != nullptr)TreeStack.push(tmp->sibling);
		for (int i = 2; i >= 0; i--) {
			if (tmp->child[i] != nullptr)TreeStack.push(tmp->child[i]);
		}
	}
}

string getDeckEnumString(DeclareType deckEnum) {
	switch (deckEnum) {
	case array_declare:
		return "ArrayK";
	case char_declare:
		return "CharK";
	case int_declare:
		return "IntegerK";
	case record_declare:
		return "RecordK";
	case type_declare:
		return "IdK";
	default:
	{
		cout << "变量类型错误" << endl;
		exit(-7);  //表示变量类型错误
	}
	}
}

string enumkindToString(StmtType value) {
	switch (value) {
	case IfK: return "IfK";
	case WhileK: return "WhileK";
	case AssignK: return "AssignK";
	case ReadK: return "ReadK";
	case WriteK: return "WriteK";
	case CallK: return "CallK";
	case ReturnK: return "ReturnK";
	default: {
		cout << "语句错误" << endl;
		exit(-8);  //表示语句错误
	}
	}
}

LexType getLexType(const string& str) {
	unordered_map<string, LexType> lexMap = {
		{ "ID", ID }, { "IF", IF }, { "BEGIN", BEGIN }, { "INTC", INTC }, { "END", END },
		{ "PLUS", PLUS }, { "MINUS", MINUS }, { "TIMES", TIMES }, { "OVER", OVER }, { "EQ", EQ },
		{ "LT", LT }, { "LMIDPAREN", LMIDPAREN }, { "RMIDPAREN", RMIDPAREN }, { "DOT", DOT },
		{ "TYPE", TYPE }, { "VAR", VAR }, { "PROCEDURE", PROCEDURE }, { "PROGRAM", PROGRAM },
		{ "SEMI", SEMI }, { "INTEGER", INTEGER }, { "CHAR", CHAR }, { "ARRAY", ARRAY },
		{ "RECORD", RECORD }, { "UNDERANGE", UNDERANGE }, { "OF", OF }, { "COMMA", COMMA },
		{ "LPAREN", LPAREN }, { "RPAREN", RPAREN }, { "ENDWH", ENDWH }, { "WHILE", WHILE },
		{ "RETURN", RETURN }, { "READ", READ }, { "WRITE", WRITE }, { "ASSIGN", ASSIGN },
		{ "THEN", THEN }, { "FI", FI }, { "ELSE", ELSE }, { "DO", DO }, { "CHARC", CHARC }
	};

	auto it = lexMap.find(str);
	if (it != lexMap.end()) {
		return it->second;
	}
	cout << "终极符匹配出错" << endl;
	exit(-9);  //表示终极符匹配出错
	//cout << "ERROR_STR:" << str << endl;
	return ID;
}



void CreateLL1Table() {
	string s, line;         //第一步：构建VN和productSet
	//int linenumber = 0;
	string sigle;
	vector<string>total;
	ifstream productfile("Product.txt", ifstream::in);
	if (!productfile.is_open()) exit(-3);
	//cout << 10 << endl;
	while (getline(productfile, line)) {
		//linenumber++;
		istringstream Line(line);
		int a = 0;
		while (Line >> sigle) {
			a++;
			if (a == 1) {
				VN.insert(sigle);
				//cout << "VN:" << sigle << endl;
			}
			else if (a == 2)continue;
			else;

			total.push_back(sigle);
		}
		productSet.push_back(total);
		total.clear();
	}
	productfile.close();


	total.clear();
	ifstream predictfile("Predict.txt", ifstream::in);    //第二步：构建VT和predictSet
	if (!predictfile.is_open()) exit(-3);
	//cout << 10 << endl;
	while (getline(predictfile, line)) {
		istringstream Line(line);
		int a = 0;
		while (Line >> sigle) {

			a++;
			if (a != 1) {
				VT.insert(sigle);
				//cout << "VT:" << sigle << endl;
			}

			total.push_back(sigle);
		}
		predictSet.push_back(total);
		total.clear();
	}
	predictfile.close();
	total.clear();
	//cout << "----------" << endl;
	for (auto it : VT)
	{
		//cout << it << endl;
	}
	//cout << "----------" << endl;
	int x = predictSet.size();
	LL1Table = vector<vector<int>>(VN.size(), vector<int>(VT.size(), 0));			//第三步：构建LL1表
	for (int i = 0; i < x; i++)
	{
		int key = stoi(predictSet[i][0]);

		string s = productSet[key - 1][0];
		int llx = 0;          //找到非终极符的距离
		for (auto it : VN) {
			if (it == s)break;
			llx++;
		}
		for (int j = 1; j < predictSet[i].size(); j++) {       //找到该产生式所有对应的终极符
			int lly = 0;
			for (auto it : VT) {
				if (it == predictSet[i][j])break;
				lly++;
			}
			LL1Table[llx][lly] = key;
		}

	}
}



int Priosoty(LexType op) {
	if (op == END) {
		return 0;
	}
	else if (op == LT || op == EQ) {
		return 1;
	}
	else if (op == PLUS || op == MINUS) {
		return 2;
	}
	else if (op == TIMES || op == OVER) {
		return 3;
	}
	else {
		cout << ":语法错误：没有该类型的操作符" << endl;
		exit(-6);      //-6代表操作符错误
	}
}

void PushSymStack(int a) {
	for (int i = productSet[a].size() - 1; i >= 1; i--) {
		SymStack.push(productSet[a][i]);
	}
}

void Predict(int pnum) {
	int a = 0;
	if (pnum == 5 || pnum == 28 || pnum == 30 || pnum == 37 || pnum == 39 || pnum == 42 || pnum == 53
		|| pnum == 79 || pnum == 87) {//3
		a = 0;
	}
	else if (pnum == 1 || pnum == 4 || pnum == 6 || pnum == 10 || pnum == 17 || pnum == 26 || pnum == 18
		|| pnum == 29 || pnum == 31 || pnum == 35 || pnum == 38 || pnum == 40 || pnum == 43 || pnum == 46
		|| pnum == 47 || pnum == 49 || pnum == 54 || pnum == 55 || pnum == 56 || pnum == 58 || pnum == 60
		|| pnum == 72 || pnum == 75 || pnum == 83 || pnum == 86 || pnum == 91 || pnum == 99 || pnum == 100
		|| pnum == 101 || pnum == 102 || pnum == 103 || pnum == 104) {//PushSymStack
		a = -1;
	}
	else if (pnum == 9 || pnum == 34 || pnum == 45 || pnum == 59 || pnum == 77) {//pop
		a = -2;
	}
	else if (pnum == 3 || pnum == 11 || pnum == 27 || pnum == 36 || pnum == 44 || pnum == 52 || pnum == 73) {//ID
		a = -3;
	}
	else a = pnum;

	switch (a)
	{
	case 0:
		break;
	case -1://
		PushSymStack(pnum - 1);
		break;
	case -2://
		STStack.pop();
		break;
	case -3://ID
	{
		PushSymStack(pnum - 1);
		currentP->idnum++;
		currentP->name.push_back(token.sem);
	}
	break;
	case 2://<ProgramHead>:: = PROGRAM ProgramName
	{
		PushSymStack(1);
		currentP = new TreeNode();
		currentP->lineno = token.lineShow;
		currentP->node_type = program_head_node;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
	}
	break;
	case 7: //  <TypeDeclaration>::=TYPE TypeDecList 
	{
		PushSymStack(6);
		currentP = new TreeNode();
		currentP->node_type = type_declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
		STStack.push(&currentP->child[0]);
	}
	break;
	case 8:// <TypeDecList>::=TypeId=TypeDef;TypeDecMore
	{
		PushSymStack(7);
		currentP = new TreeNode();
		currentP->node_type = declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 12://<TypeDef>::=BaseType
	{
		PushSymStack(11);
		temp = &(currentP->kind.dec_type);
	}
	break;
	case 13://<TypeDef>::=StructureType
	{
		PushSymStack(12);
		temp = &(currentP->kind.dec_type);
	}
	break;
	case 14: //    <TypeDef>::=ID
	{
		PushSymStack(13);
		currentP->kind.dec_type = type_declare;
		currentP->type_name = token.sem;
		currentP->idnum++;
		currentP->name.push_back(token.sem);
	}
	break;
	case 15://<BaseType>::=INTEGER
	{
		PushSymStack(14);
		(*temp) = int_declare;
	}
	break;
	case 16://<BaseType>::=CHAR
	{
		PushSymStack(15);
		(*temp) = char_declare;
	}
	break;
	case 19://<ArrayType>::=ARRAY [low..top] OF BaseType
	{
		PushSymStack(18);
		(*temp) = array_declare;
		temp = &(currentP->attr.arrayAttr.element_type);
	}
	break;
	case 20://<Low>::=INTC
	{
		PushSymStack(19);
		currentP->attr.arrayAttr.low = stoi(token.sem);
	}
	break;
	case 21://<Top>::=INTC
	{
		PushSymStack(20);
		currentP->attr.arrayAttr.up = stoi(token.sem);
	}
	break;
	case 22://<RecType>::=RECORD FieldDecList END
	{
		PushSymStack(21);
		(*temp) = record_declare;
		saveP = currentP;
		STStack.push(&currentP->child[0]);
	}
	break;
	case 23://<FieldDecList>::=BaseType Idlist;FieldDecMore
	{
		PushSymStack(22);
		currentP = new TreeNode();
		currentP->node_type = declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		temp = &(currentP->kind.dec_type);
		STStack.push(&currentP->sibling);
	}
	break;
	case 24://<FieldDecList>::=ArrayType IdList;FieldDecMore
	{
		PushSymStack(23);
		currentP = new TreeNode();
		currentP->node_type = declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		temp = &(currentP->kind.dec_type);
		(*temp) = array_declare;      //直接赋值为ArrayK
		STStack.push(&currentP->sibling);
	}
	break;
	case 25: //<FieldDecMore>::=3
	{
		STStack.pop();
		currentP = saveP;
	}
	break;
	case 32: //<VarDeclaration>::=VAR VarDecList
	{
		PushSymStack(31);
		currentP = new TreeNode();
		currentP->node_type = var_declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
		STStack.push(&currentP->child[0]);
	}
	break;
	case 33: //<VarDeclist>::=TypeDef VarList;VarDecMore
	{
		PushSymStack(32);
		currentP = new TreeNode();
		currentP->node_type = declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 41: //<ProcDec::=PROCEDURE 
		//ProcName(ParamList);
		// ProcDecPart
		//ProcBody	ProcDecMore
	{
		PushSymStack(40);
		currentP = new TreeNode();
		currentP->node_type = procedure_declare_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		saveP = currentP;       //保存节点，以防声明时丢失
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
		for (int i = 2; i >= 0; i--)
			STStack.push(&currentP->child[i]);
	}
	break;
	case 48://<ParamMore>::=3
	{
		STStack.pop();
		currentP = saveP;
		temp = &(currentP->kind.dec_type);
	}
	break;
	case 50:  //<Param>::=TypeDef FormList
	{
		PushSymStack(49);
		currentP = new TreeNode();
		currentP->node_type = declare_node;
		currentP->lineno = token.lineShow;
		currentP->attr.procAttr.paramt = Valparamtype;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 51://<Param>::=VAR TypeDef FormList
	{
		PushSymStack(50);
		currentP = new TreeNode();
		currentP->node_type = declare_node;
		currentP->lineno = token.lineShow;
		currentP->attr.procAttr.paramt = Varparamtype;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 57: //<ProgramBody>::=BEGIN StmList END
	{
		PushSymStack(56);
		STStack.pop();
		currentP = new TreeNode();
		currentP->node_type = stms_node;
		currentP->lineno = token.lineShow;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->child[0]);
	}
	break;
	case 61://<Stm>::=ConditionalStm
	{
		PushSymStack(60);
		currentP = new TreeNode();
		currentP->node_type = stm_node;
		currentP->lineno = token.lineShow;
		currentP->kind.stmt_type = IfK;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 62://<Stm>::=LoopStm
	{
		PushSymStack(61);
		currentP = new TreeNode();
		currentP->node_type = stm_node;
		currentP->lineno = token.lineShow;
		currentP->kind.stmt_type = WhileK;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 63://<Stm>::=InputStm
	{
		PushSymStack(62);
		currentP = new TreeNode();
		currentP->node_type = stm_node;
		currentP->lineno = token.lineShow;
		currentP->kind.stmt_type = ReadK;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
		//cout << Prok << endl;
	}
	break;
	case 64://<Stm>::=OutputStm
	{
		PushSymStack(63);
		currentP = new TreeNode();
		currentP->node_type = stm_node;
		currentP->lineno = token.lineShow;
		currentP->kind.stmt_type = WriteK;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 65://<Stm>::=ReturnStm
	{
		PushSymStack(64);
		currentP = new TreeNode();
		currentP->node_type = stm_node;
		currentP->lineno = token.lineShow;
		currentP->kind.stmt_type = ReturnK;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
		STStack.push(&currentP->sibling);
	}
	break;
	case 66://<Stm>::=id AssCall       赋值和函数调用复杂些
	{
		PushSymStack(65);
		currentP = new TreeNode();
		currentP->node_type = stm_node;
		currentP->lineno = token.lineShow;
		TreeNode* tt = new TreeNode();
		tt->node_type = expr_node;
		tt->kind.exp_type = IdEK;
		tt->attr.expAttr.varkind = IdV;
		tt->lineno = token.lineShow;
		tt->idnum++;
		tt->name.push_back(token.sem);
		currentP->child[0] = tt;
		TreeNode** t1 = STStack.top();
		STStack.pop();
		*t1 = currentP;                        //节点处理完毕
		STStack.push(&currentP->sibling);
	}
	break;
	case 67://<AssCall>::=AssignmentRest
	{
		PushSymStack(66);
		currentP->kind.stmt_type = AssignK;
	}
	break;
	case 68://<AssCall>::=CallStmRest
	{
		PushSymStack(67);
		(*(currentP->child[0])).attr.expAttr.varkind = IdV;
		currentP->kind.stmt_type = CallK;
	}
	break;
	case 69: //<AssignmentRest>::=VariMore:=Exp
	{
		PushSymStack(68);
		STStack.push(&currentP->child[1]);      //压入赋值右部表达式指针，表达式压入
		currentP = currentP->child[0];   //取当前currentP为赋值左侧值
		TreeNode* tt = new TreeNode();
		tt->node_type = expr_node;
		tt->lineno = token.lineShow;
		tt->kind.exp_type = OpK;
		tt->attr.expAttr.op = END;      //压入特殊栈底标志，以便后续进行表达式处理
		opSymStack.push(tt);
	}
	break;
	case 70: //<ConditionalStm>::=IF RelExp THEN StmList ELSE StmList FI
	{
		PushSymStack(69);
		for (int i = 2; i >= 0; i--)
			STStack.push(&currentP->child[i]);      //ELSE StmList     ;      THEN StmList      ;		RelExp
	}
	break;
	case 71: //<LoopStm>::=WHILE RelExp DO StmList ENDWH
	{
		PushSymStack(70);
		STStack.push(&currentP->child[1]);
		STStack.push(&currentP->child[0]);
	}
	break;
	case 74://<OutputStm>::=WRITE(Exp)
	{
		PushSymStack(73);
		STStack.push(&currentP->child[0]);
		TreeNode* tt = new TreeNode;
		tt->node_type = expr_node;
		tt->lineno = token.lineShow;
		tt->kind.exp_type = OpK;
		tt->attr.expAttr.op = END;      //对所有表达式处理应压入END操作符
		opSymStack.push(tt);
	}
	break;
	case 76://<CallStmRest>::=(ActParamList)
	{
		PushSymStack(75);
		STStack.push(&currentP->child[1]);
	}
	break;
	case 78: //<ActParamList>::=Exp ActParamMore
	{
		PushSymStack(77);
		TreeNode* tt = new TreeNode;
		tt->node_type = expr_node;
		tt->lineno = token.lineShow;
		tt->kind.exp_type = OpK;
		tt->attr.expAttr.op = END;
		opSymStack.push(tt);
	}
	break;
	case 80: //<ActParamMore>::=,ActParamList
	{
		PushSymStack(79);
		STStack.push(&currentP->sibling);
	}
	break;
	case 81: //<RelExp>::=Exp OtherRelE
	{
		PushSymStack(80);
		TreeNode* tt = new TreeNode;
		tt->kind.exp_type = OpK;
		tt->lineno = token.lineShow;
		tt->node_type = expr_node;
		tt->attr.expAttr.op = END;
		opSymStack.push(tt);
		getExpResult = false;
	}
	break;
	case 82://<OtherRelE>::=CmpOp Exp
	{
		PushSymStack(81);
		currentP = new TreeNode();
		currentP->kind.exp_type = OpK;
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		currentP->attr.expAttr.op = token.lex;
		LexType Numstacktop = opSymStack.top()->attr.expAttr.op;
		while (Priosoty(Numstacktop) >= Priosoty(token.lex)) {
			TreeNode* t = opSymStack.top();
			opSymStack.pop();
			TreeNode* Rnum = OpNumStack.top();
			OpNumStack.pop();
			TreeNode* Lnum = OpNumStack.top();
			OpNumStack.pop();
			t->child[0] = Lnum;
			t->child[1] = Rnum;
			OpNumStack.push(t);
			Numstacktop = opSymStack.top()->attr.expAttr.op;
		}
		opSymStack.push(currentP);
		getExpResult = true;
	}
	break;
	case 84:  //<OtherTerm>::=3        语法的括号匹配也在这里
	{
		if (expchar == 1) {
			opSymStack.pop();
			TreeNode** t = STStack.top();
			STStack.pop();
			*t = currentP;
			expchar = 0;
			break;
		}
		if (token.lex == RPAREN && expflag > 0) {      //当前token为右括号且未匹配左括号数不为0
			LexType Numstacktop = opSymStack.top()->attr.expAttr.op;
			while (Numstacktop != LPAREN) {   //找到匹配括号
				TreeNode* t = opSymStack.top();
				opSymStack.pop();
				TreeNode* Rnum = OpNumStack.top();
				OpNumStack.pop();
				TreeNode* Lnum = OpNumStack.top();
				OpNumStack.pop();
				t->child[0] = Lnum;
				t->child[1] = Rnum;
				OpNumStack.push(t);
				Numstacktop = opSymStack.top()->attr.expAttr.op;
			}
			opSymStack.pop();
			expflag--;
		}
		else {
			if (getExpResult || getExpResult2) {     //当比较操作或数组计算完成时，表示当前表达式处理结束
				LexType Numstacktop = opSymStack.top()->attr.expAttr.op;
				while (Numstacktop != END) {
					TreeNode* t = opSymStack.top();
					opSymStack.pop();
					TreeNode* Rnum = OpNumStack.top();
					OpNumStack.pop();
					TreeNode* Lnum = OpNumStack.top();
					OpNumStack.pop();
					t->child[0] = Lnum;
					t->child[1] = Rnum;
					OpNumStack.push(t);
					Numstacktop = opSymStack.top()->attr.expAttr.op;
				}
				opSymStack.pop();
				currentP = OpNumStack.top();
				OpNumStack.pop();
				TreeNode** t = STStack.top();
				STStack.pop();
				*t = currentP;
				getExpResult2 = false;
			}
		}
	}
	break;
	case 85://<OtherTerm>::=AddOp Exp
	{
		PushSymStack(84);
		currentP = new TreeNode();
		currentP->lineno = token.lineShow;
		currentP->kind.exp_type = OpK;
		currentP->node_type = expr_node;
		currentP->attr.expAttr.op = token.lex;
		LexType Numstacktop = opSymStack.top()->attr.expAttr.op;
		while (Numstacktop != LPAREN && Priosoty(Numstacktop) >= Priosoty(token.lex)) {
			TreeNode* t = opSymStack.top();
			opSymStack.pop();
			TreeNode* Rnum = OpNumStack.top();
			OpNumStack.pop();
			TreeNode* Lnum = OpNumStack.top();
			OpNumStack.pop();
			t->child[0] = Lnum;
			t->child[1] = Rnum;
			OpNumStack.push(t);
			Numstacktop = opSymStack.top()->attr.expAttr.op;
		}
		opSymStack.push(currentP);
	}
	break;
	case 88://<OtherFactor>::=MultOp Term
	{
		PushSymStack(87);
		currentP = new TreeNode();
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		currentP->kind.exp_type = OpK;
		currentP->attr.expAttr.op = token.lex;
		LexType top = opSymStack.top()->attr.expAttr.op;
		while (top != LPAREN && Priosoty(top) >= Priosoty(token.lex)) {
			TreeNode* t = opSymStack.top();
			opSymStack.pop();
			TreeNode* Rnum = OpNumStack.top();
			OpNumStack.pop();
			TreeNode* Lnum = OpNumStack.top();
			OpNumStack.pop();
			t->child[0] = Lnum;
			t->child[1] = Rnum;
			OpNumStack.push(t);
			top = opSymStack.top()->attr.expAttr.op;
			opSymStack.push(currentP);
		}
	}
	break;
	case 89: //<Factor>::=(Exp)
	{
		PushSymStack(88);
		currentP = new TreeNode();
		currentP->attr.expAttr.op = LPAREN;
		currentP->kind.exp_type = OpK;
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		opSymStack.push(currentP);
		expflag++;    //未匹配符号数加1
	}
	break;
	case 90://<Factor>::=INTC
	{
		PushSymStack(89);
		currentP = new TreeNode();
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		currentP->kind.exp_type = ConstK;
		currentP->attr.expAttr.val = stoi(token.sem);
		OpNumStack.push(currentP);
	}
	break;
	case 92://<Factor>::=ID VariMore
	{
		PushSymStack(91);
		currentP = new TreeNode();
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		currentP->kind.exp_type = IdEK;
		currentP->idnum++;
		currentP->name.push_back(token.sem);
		OpNumStack.push(currentP);
	}
	break;
	case 93: //<VariMore>::=3
		currentP->attr.expAttr.varkind = IdV;
		break;
	case 94://<VariMore>::=[Exp]
	{
		PushSymStack(93);
		currentP->attr.expAttr.varkind = ArrayMembV;
		STStack.push(&currentP->child[0]);
		TreeNode* tt = new TreeNode();
		tt->node_type = expr_node;
		tt->lineno = token.lineShow;
		tt->kind.exp_type = OpK;
		tt->attr.expAttr.op = END;
		opSymStack.push(tt);
		getExpResult2 = true;
	}
	break;
	case 95: //<VariMore>::=.FieldVar
	{
		PushSymStack(94);
		currentP->attr.expAttr.varkind = FieldMembV;
		STStack.push(&currentP->child[0]);
	}
	break;
	case 96: //<FieldVar>::=ID FieldVarMore
	{
		PushSymStack(95);
		currentP = new TreeNode();
		currentP->idnum++;
		currentP->name.push_back(token.sem);
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		currentP->kind.exp_type = IdEK;
		TreeNode** t = STStack.top();
		STStack.pop();
		*t = currentP;
	}
	break;
	case 97: //<FieIdVarMore>::=3
		currentP->attr.expAttr.varkind = IdV;
		break;
	case 98://<FieldVarMore>::=[Exp]
	{
		PushSymStack(97);
		currentP->attr.expAttr.varkind = ArrayMembV;
		STStack.push(&currentP->child[0]);
		TreeNode* tt = new TreeNode();
		tt->node_type = expr_node;
		tt->lineno = token.lineShow;
		tt->kind.exp_type = OpK;
		tt->attr.expAttr.op = END;
		opSymStack.push(tt);
		getExpResult2 = true;
	}
	break;
	case 105://Factor :: = CHARC
	{
		PushSymStack(104);
		currentP = new TreeNode();
		currentP->node_type = expr_node;
		currentP->lineno = token.lineShow;
		currentP->kind.exp_type = CharC;
		currentP->attr.expAttr.valc = atoi(token.sem.c_str());
		expchar = 1;
	}
	break;
	default:
		break;
	}
}


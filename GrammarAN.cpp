// GrammarAN.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include"GrammarAN.h"
TreeNode* parseLL() {
    CreateLL1Table();
    S = productSet[0][0];
    TreeNode* proK = new TreeNode();
    (*proK).nodekind = ProK;
    (*proK).lineno = 1;
    syntaxTreeStack.push(&(*proK).child[2]);
    syntaxTreeStack.push(&(*proK).child[1]);
    syntaxTreeStack.push(&(*proK).child[0]);

    //读入一个token，用lineno记录行号
    ifstream in("token.txt");
    string line;
    getline(in, line);
    istringstream inword(line);
    inword >> token.linsShow;
    int lex;
    inword >> lex;
    token.lex = (LexType)lex;
    inword >> token.sem;
    int lineno = token.linsShow;
    while (!signStack.empty()) {
        string top = signStack.top();
        int num = GetIndex(VN, top);
        if (num != -1) {              //如果符号栈顶不为终极符
            int x = GetIndex(VN, top);
            int y = GetIndex(VT, arrayLexType_hyf[(int)token.lex]);
            int pnum = LL1Table[x][y];		//栈顶非终极符做为行号，Token的词法信息为列号，取LL(1)分析表内容，得到产生式编号
            if (pnum == 0) {
                string a = to_string(token.linsShow) + ":" + arrayLexType_hyf[(int)token.lex] + "不属于" + top + "的predict集";
                InputError(a, "error.txt");
                //cout << token.linsShow << ":" << arrayLexType_hyf[(int)token.lex] << "不属于" << top << "的predict集";
                exit(0);
            }
            signStack.pop();
            Predict(pnum);
        }
    }
}
void CreateLL1Table() {
	string line;	//记录每一行字符串
	vector<string> words;	//存储每一行字符串里的所有单词
	//将product.txt文件读入到productSet，该文件内存储所有的产生式,通过该文件可以获得所有的非终极符
	ifstream in("product.txt");
	if (in) {
		while (getline(in, line)) {
			//cout << line << endl;
			istringstream inword(line);
			string word;	//记录单词
			int i = 0;		//记录单词数，方便对第二个单词舍弃，并将第一个单词加入集合VN
			while (inword >> word) {
				i++;
				if (i == 1) {
					VN.insert(word);
					words.push_back(word);
				}
				else if (i > 2) {
					words.push_back(word);
				}
			}
			productSet.push_back(words);
			words.clear();
		}
	}
	else {
		cerr << "该文件不存在";
	}
	in.close();
	//将predict.txt文件读入到predictSet，该文件内存储所有的产生式的predict集,通过该文件可以获得所有的终极符
	in.open("predict.txt");
	if (in) {
		while (getline(in, line)) {
			//cout << line << endl;
			istringstream inword(line);
			string word;	//记录单词
			int i = 0;		//记录单词数，将第一个单词加入集合VT
			while (inword >> word) {
				i++;
				words.push_back(word);
				if (i != 1) {
					VT.insert(word);
				}
			}
			predictSet.push_back(words);
			words.clear();
		}
	}
	else {
		cerr << "该文件不存在";
	}
	in.close();
	//利用上面获取到的所有非终极符和终极符构建LL1矩阵，然后通过predictSet来对矩阵进行赋值
	int m = VN.size(), n = VT.size();
	LL1Table = vector<vector<int>>(m, vector<int>(n, 0));
	for (int i = 0; i < predictSet.size(); i++) {
		int num = stoi(predictSet[i][0]);
		string s = productSet[num - 1][0];
		int x = GetIndex(VN, s);
		for (int j = 1; j < predictSet[i].size(); j++) {
			int y = GetIndex(VT, predictSet[i][j]);
			LL1Table[x][y] = num;
		}
	}
}
void InputError(string errorInfo, string path) {
	ofstream ouput;
	ouput.open(path);
	ouput << errorInfo;
	ouput.close();
}

void Predict(int pnum) {
	if (pnum == 1)process1();
	else if (pnum == 2)process2();
	else if (pnum == 3)process3();
	else if (pnum == 4)process4();
	else if (pnum == 5)process5();
	else if (pnum == 6)process6();
	else if (pnum == 7)process7();
	else if (pnum == 8)process8();
	else if (pnum == 9)process9();
	else if (pnum == 10)process10();
	else if (pnum == 11)process11();
	else if (pnum == 12)process12();
	else if (pnum == 13)process13();
	else if (pnum == 14)process14();
	else if (pnum == 15)process15();
	else if (pnum == 16)process16();
	else if (pnum == 17)process17();
	else if (pnum == 18)process18();
	else if (pnum == 19)process19();
	else if (pnum == 20)process20();
	else if (pnum == 21)process21();
	else if (pnum == 22)process22();
	else if (pnum == 23)process23();
	else if (pnum == 24)process24();
	else if (pnum == 25)process25();
	else if (pnum == 26)process26();
	else if (pnum == 27)process27();
	else if (pnum == 28)process28();
	else if (pnum == 29)process29();
	else if (pnum == 30)process30();
	else if (pnum == 31)process31();
	else if (pnum == 32)process32();
	else if (pnum == 33)process33();
	else if (pnum == 34)process34();
	else if (pnum == 35)process35();
	else if (pnum == 36)process36();
	else if (pnum == 37)process37();
	else if (pnum == 38)process38();
	else if (pnum == 39)process39();
	else if (pnum == 40)process40();
	else if (pnum == 41)process41();
	else if (pnum == 42)process42();
	else if (pnum == 43)process43();
	else if (pnum == 44)process44();
	else if (pnum == 45)process45();
	else if (pnum == 46)process46();
	else if (pnum == 47)process47();
	else if (pnum == 48)process48();
	else if (pnum == 49)process49();
	else if (pnum == 50)process50();
	else if (pnum == 51)process51();
	else if (pnum == 52)process52();
	else if (pnum == 53)process53();
	else if (pnum == 54)process54();
	else if (pnum == 55)process55();
	else if (pnum == 56)process56();
	else if (pnum == 57)process57();
	else if (pnum == 58)process58();
	else if (pnum == 59)process59();
	else if (pnum == 60)process60();
	else if (pnum == 61)process61();
	else if (pnum == 62)process62();
	else if (pnum == 63)process63();
	else if (pnum == 64)process64();
	else if (pnum == 65)process65();
	else if (pnum == 66)process66();
	else if (pnum == 67)process67();
	else if (pnum == 68)process68();
	else if (pnum == 69)process69();
	else if (pnum == 70)process70();
	else if (pnum == 71)process71();
	else if (pnum == 72)process72();
	else if (pnum == 73)process73();
	else if (pnum == 74)process74();
	else if (pnum == 75)process75();
	else if (pnum == 76)process76();
	else if (pnum == 77)process77();
	else if (pnum == 78)process78();
	else if (pnum == 79)process79();
	else if (pnum == 80)process80();
	else if (pnum == 81)process81();
	else if (pnum == 82)process82();
	else if (pnum == 83)process83();
	else if (pnum == 84)process84();
	else if (pnum == 85)process85();
	else if (pnum == 86)process86();
	else if (pnum == 87)process87();
	else if (pnum == 88)process88();
	else if (pnum == 89)process89();
	else if (pnum == 90)process90();
	else if (pnum == 91)process91();
	else if (pnum == 92)process92();
	else if (pnum == 93)process93();
	else if (pnum == 94)process94();
	else if (pnum == 95)process95();
	else if (pnum == 96)process96();
	else if (pnum == 97)process97();
	else if (pnum == 98)process98();
	else if (pnum == 99)process99();
	else if (pnum == 100)process100();
	else if (pnum == 101)process101();
	else if (pnum == 102)process102();
	else if (pnum == 103)process103();
	else if (pnum == 104)process104();
	else;
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
		string a = to_string(token.linsShow) + ":没有" + arrayLexType_hyf[(int)token.lex] + "运算符";
		InputError(a, "error.txt");
		//cout << "没有" << arrayLexType_hyf[(int)token.lex] << "运算符";
		exit(0);
	}
}


int GetIndex(set<string>& A, string s) {
	int i = 0;
	for (set<string>::iterator begin = A.begin(); begin != A.end(); begin++, i++) {
		if (*begin == s) {
			return i;
		}
	}
	return -1;	//当字符串s不在集合内时返回-1
}

void process1() {
	for (int i = productSet[0].size() - 1; i >= 1; i--) {
		signStack.push(productSet[0][i]);
	}
}
void process2() {
	for (int i = productSet[1].size() - 1; i >= 1; i--) {
		signStack.push(productSet[1][i]);
	}
	currentP = new TreeNode();
	(*currentP).nodekind = PheadK;
	(*currentP).lineno = token.linsShow;
	TreeNode** t = syntaxTreeStack.top();
	syntaxTreeStack.pop();
	*t = currentP;
	//currentP = t;
}
void process3() {
	for (int i = productSet[2].size() - 1; i >= 1; i--) {
		signStack.push(productSet[2][i]);
	}
	(*currentP).idnum++;
	(*currentP).name.push_back(token.sem);
}
void process4() {
	for (int i = productSet[3].size() - 1; i >= 1; i--) {
		signStack.push(productSet[3][i]);
	}
}
void process5() {}
void process6() {
	for (int i = productSet[5].size() - 1; i >= 1; i--) {
		signStack.push(productSet[5][i]);
	}
}
void process7() {
	for (int i = productSet[6].size() - 1; i >= 1; i--) {
		signStack.push(productSet[6][i]);
	}
	currentP = new TreeNode();
	(*currentP).nodekind = TypeK;
	(*currentP).lineno = token.linsShow;
	TreeNode** t = syntaxTreeStack.top();
	syntaxTreeStack.pop();
	*t = currentP;
	//currentP = t;
	//(*currentP).sibling = new TreeNode();
	//(*currentP).child[0] = new TreeNode();
	syntaxTreeStack.push(&(*currentP).sibling);
	syntaxTreeStack.push(&(*currentP).child[0]);

}
void process8() {
	for (int i = productSet[7].size() - 1; i >= 1; i--) {
		signStack.push(productSet[7][i]);
	}
	currentP = new TreeNode();
	(*currentP).nodekind = DecK;
	(*currentP).lineno = token.linsShow;
	TreeNode** t = syntaxTreeStack.top();
	syntaxTreeStack.pop();
	*t = currentP;
	//currentP = t;
	//(*currentP).sibling = new TreeNode();
	syntaxTreeStack.push(&(*currentP).sibling);
}
void process9() {
	syntaxTreeStack.pop();
}
void process10() {
	for (int i = productSet[9].size() - 1; i >= 1; i--) {
		signStack.push(productSet[9][i]);
	}
}
void process11() {
	for (int i = productSet[10].size() - 1; i >= 1; i--) {
		signStack.push(productSet[10][i]);
	}
	(*currentP).idnum++;
	(*currentP).name.push_back(token.sem);
}
void process12() {
	for (int i = productSet[11].size() - 1; i >= 1; i--) {
		signStack.push(productSet[11][i]);
	}
	temp = &((*currentP).kind.dec);
}
void process13() {
	for (int i = productSet[12].size() - 1; i >= 1; i--) {
		signStack.push(productSet[12][i]);
	}
	temp = &((*currentP).kind.dec);
}
void process14() {
	for (int i = productSet[13].size() - 1; i >= 1; i--) {
		signStack.push(productSet[13][i]);
	}
	(*currentP).kind.dec = IdK;
	(*currentP).type_name = token.sem;
	(*currentP).idnum++;
	(*currentP).name.push_back(token.sem);
}
void process15() {
	for (int i = productSet[14].size() - 1; i >= 1; i--) {
		signStack.push(productSet[14][i]);
	}
	(*temp) = IntegerK;
}
void process16() {
	for (int i = productSet[15].size() - 1; i >= 1; i--) {
		signStack.push(productSet[15][i]);
	}
	(*temp) = CharK;
}
void process17() {
	for (int i = productSet[16].size() - 1; i >= 1; i--) {
		signStack.push(productSet[16][i]);
	}
}
void process18() {
	for (int i = productSet[17].size() - 1; i >= 1; i--) {
		signStack.push(productSet[17][i]);
	}
}
void process19() {
	for (int i = productSet[18].size() - 1; i >= 1; i--) {
		signStack.push(productSet[18][i]);
	}
	(*temp) = ArrayK;
	temp = &((*currentP).attr.arrayAttr.childType);
}
void process20() {
	for (int i = productSet[19].size() - 1; i >= 1; i--) {
		signStack.push(productSet[19][i]);
	}
	(*currentP).attr.arrayAttr.low = stoi(token.sem);
}
void process21() {
	for (int i = productSet[20].size() - 1; i >= 1; i--) {
		signStack.push(productSet[20][i]);
	}
	(*currentP).attr.arrayAttr.up = stoi(token.sem);
}
void process22() {
	for (int i = productSet[21].size() - 1; i >= 1; i--) {
		signStack.push(productSet[21][i]);
	}
	(*temp) = RecordK;
	saveP = currentP;
	//(*currentP).child[0] = new TreeNode();
	syntaxTreeStack.push(&(*currentP).child[0]);
}
void process23() {
	for (int i = productSet[22].size() - 1; i >= 1; i--) {
		signStack.push(productSet[22][i]);
	}
	currentP = new TreeNode();
	(*currentP).nodekind = DecK;
	(*currentP).lineno = token.linsShow;
	TreeNode** t = syntaxTreeStack.top();
	syntaxTreeStack.pop();
	*t = currentP;
	//currentP = t;
	temp = &((*currentP).kind.dec);
	//(*currentP).sibling = new TreeNode();
	syntaxTreeStack.push(&(*currentP).sibling);
}
void process24() {
	for (int i = productSet[23].size() - 1; i >= 1; i--) {
		signStack.push(productSet[23][i]);
	}
	currentP = new TreeNode();
	(*currentP).nodekind = DecK;
	(*currentP).lineno = token.linsShow;
	TreeNode** t = syntaxTreeStack.top();
	syntaxTreeStack.pop();
	*t = currentP;
	//currentP = t;
	//(*currentP).sibling = new TreeNode();
	syntaxTreeStack.push(&(*currentP).sibling);
}
void process25() {
	syntaxTreeStack.pop();
	currentP = saveP;
}
void process26() {
	for (int i = productSet[25].size() - 1; i >= 1; i--) {
		signStack.push(productSet[25][i]);
	}
}
void process27() {
	for (int i = productSet[26].size() - 1; i >= 1; i--) {
		signStack.push(productSet[26][i]);
	}
	(*currentP).idnum++;
	(*currentP).name.push_back(token.sem);
}
void process28() {}
void process29() {
	for (int i = productSet[28].size() - 1; i >= 1; i--) {
		signStack.push(productSet[28][i]);
	}
}
void process30() {}
void process31() {
	for (int i = productSet[30].size() - 1; i >= 1; i--) {
		signStack.push(productSet[30][i]);
	}
}



int main()
{
    std::cout << "Hello World!\n";
}


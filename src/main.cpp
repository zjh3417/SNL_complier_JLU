#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "GrammarAN.h"
#include "SemanticAnalysis.h"
#include "gencode.h"
#include "utils.h"

int main(int argc,char * argv[],char * envp[]) {
	
	if(argc < 2){
		fprintf(stderr,"Usage : %s <snl source code>\n",argv[0]);
		exit(1);
	}
	
	Token_Print(argv[1]);
	
	TreeNode* Prok=parseLL();
	
	Analyze(Prok);

	gen(Prok);

	//getchar();
	return 0;
}
#ifndef _checker5_H
#define _checker5_H
#include "framework/BasicChecker.h"
#include "framework/Common.h"

#include <vector>
#include <clang/AST/StmtVisitor.h>
#include <clang/AST/Expr.h>
#include <clang/Analysis/CFG.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <sstream>
#include <string>
#include <iostream>
#include <map>
#include <time.h>
#include <utility>
#include <unordered_set>
#include <set>
#include <stack>

using namespace std;


class Checker5 : public BasicChecker {
private:

	clock_t run_time;

public:

	int loc_time;
	int back_time;
//	void printTime(ofstream& process_file)
//	{
////		process_file<<"Loc Time:"<<loc_time/CLOCKS_PER_SEC/60<<"min"<<loc_time/CLOCKS_PER_SEC%60<<endl;
////		process_file<<"Check Time:"<<back_time/CLOCKS_PER_SEC/60<<"min"<<back_time/CLOCKS_PER_SEC%60<<endl;
//	}
	Checker5(ASTResource *resource, ASTManager *manager,
                  CallGraph *call_graph, Config *configure)
      : BasicChecker(resource, manager, call_graph, configure){

        std::unordered_map<std::string, std::string> ptrConfig =configure->getOptionBlock("Checker5");
	}

    ~Checker5() {

    }
	void check();

};

#endif

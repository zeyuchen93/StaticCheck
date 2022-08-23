#ifndef _ReturnStackAddrChecker_H
#define _ReturnStackAddrChecker_H
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


class FindReturnStmt : public StmtVisitor<FindReturnStmt>
{
private:
    std::vector<ReturnStmt *> retStmts;

public:
    void VisitChildren(Stmt *stmt)
    {
        for (Stmt *SubStmt : stmt->children())
        {
            if (SubStmt)
            {
                this->Visit(SubStmt);
            }
        }
    }
    void VisitStmt(Stmt *expr)
    {
        if (ReturnStmt *ret = dyn_cast<ReturnStmt>(expr))
        {
            retStmts.push_back(ret);
        }
        VisitChildren(expr);
    }
    std::vector<ReturnStmt *> &getReturnStmts()
    {
        return retStmts;
    }
};

class ReturnStackAddrChecker : public BasicChecker {
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
	ReturnStackAddrChecker(ASTResource *resource, ASTManager *manager,
                  CallGraph *call_graph, Config *configure)
      : BasicChecker(resource, manager, call_graph, configure){

        std::unordered_map<std::string, std::string> ptrConfig =configure->getOptionBlock("ReturnStackAddrChecker");
        run_time = 0;
	}

    ~ReturnStackAddrChecker() {

    }

	clock_t get_time();
	void check();
};

#endif

#include "checkers/ReturnStackAddrChecker/ReturnStackAddrChecker.h"

#include "clang/Frontend/FrontendAction.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/Decl.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/MemoryBuffer.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Frontend/ASTUnit.h"

using namespace std;

void ReturnStackAddrChecker::check() {
	clock_t start = clock();
    for(auto F:resource->getFunctions()) {
		FunctionDecl *FD = manager->getFunctionDecl(F);
		std::unique_ptr<CFG> &function_cfg = manager->getCFG(F);
	/*
		FunctionDecl *Func = common::manager->getFunctionDecl(astFunc);
	std::unique_ptr<CFG> & myCFG= common::manager->getCFG(astFunc,Func);*/	
		const clang::SourceManager &SM = FD->getASTContext().getSourceManager();
		string filename = "";
		if(FD->hasBody())
			FD=FD->getDefinition();
		else
			continue;
		SourceLocation SL = FD->getBeginLoc();
		filename = SL.printToString(SM);
		if (filename.find(".h") != string::npos || filename.find("include") != string::npos)
			continue;
		common::printLog( "Start check function "+FD->getNameAsString()+"\n",common::CheckerName::ReturnStackAddrChecker,4,*configure);
		FindReturnStmt returnFinder;
		returnFinder.Visit(FD->getBody());
		std::vector<ReturnStmt*> returnStmts = returnFinder.getReturnStmts();
		for (auto ret : returnStmts) {
			if (UnaryOperator* uo = dyn_cast<UnaryOperator>(ret->getRetValue()->IgnoreCasts()->IgnoreParens())) {
				if (uo->getOpcode() == UO_AddrOf) {//return &xx
					Expr* sub = uo->getSubExpr();
					common::printLog( "return address of "+common::print(sub)+"\n",common::CheckerName::ReturnStackAddrChecker,3,*configure);
					if (DeclRefExpr* decl = dyn_cast<DeclRefExpr>(sub)) {
						VarDecl* vardecl = (VarDecl*)(decl->getDecl());
						if (vardecl->isLocalVarDecl()) {
							common::printLog( "Warning: return address of local declared variable "+common::print(sub)+"\n",common::CheckerName::ReturnStackAddrChecker,4,*configure);
						}
					}
				}
			}
		}
		
		
    }
    
}
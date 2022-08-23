#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <vector>
#include <unordered_map>

#include <clang/Frontend/ASTUnit.h>
#include <clang/Analysis/CFG.h>
#include "Config.h"

using namespace clang;

std::vector<std::string> initialize(std::string astList);

void prcoess_bar(float progress);

namespace common {

enum CheckerName {
  ReturnStackAddrChecker,
  Checker5
};
/**
 * 判断某一个call site是不是一个函数指针引起
 */
bool isThisCallSiteAFunctionPointer(Stmt *callsite);
/**
 * 根据ID获取实际的Stmt。主要应用于获取call site
 * @param  parent 于call site处调用了某一函数的函数。
 * @param  id     Stmt的ID，通过Stmt->getID(ASTContext&)获取
 */
Stmt *getStmtInFunctionWithID(FunctionDecl *parent, int64_t id);

std::string getLambdaName(FunctionDecl *FD);

std::unique_ptr<ASTUnit> loadFromASTFile(std::string AST);

std::vector<FunctionDecl *> getFunctions(ASTContext &Context);
std::vector<FunctionDecl *> getFunctions(ASTContext &Context,
                                         SourceLocation SL);

std::vector<VarDecl *> getGlobalVars(ASTContext &Context);

std::vector<VarDecl *> getVariables(FunctionDecl *FD);

std::vector<std::pair<VarDecl *, FieldDecl *>> getFieldVariables(FunctionDecl *FD);

std::vector<std::tuple<VarDecl *, FieldDecl *, FieldDecl *>> getNestingFields(FunctionDecl *FD);

//获取FD调用的函数
std::vector<std::string> getCalledFunctions(
    FunctionDecl *FD,
    const std::unordered_map<std::string, std::string> &configure);

//获取FD调用的函数与函数调用点(callsite)
std::vector<std::pair<std::string, int64_t>> getCalledFunctionsInfo(
    FunctionDecl *FD,
    const std::unordered_map<std::string, std::string> &configure);

std::vector<std::pair<int64_t, std::set<std::string>>> getFunctionPtrWithCS(
    FunctionDecl *FD,
    std::unordered_map<std::string, std::set<std::string>> &mayPointTo);

std::vector<FunctionDecl *> getCalledLambda(FunctionDecl *FD);

std::vector<CallExpr *> getCallExpr(FunctionDecl *FD);

std::string getFullName(const FunctionDecl *FD);

void printLog(std::string, CheckerName cn, int level, Config &c);

const CFGBlock *getBlockWithID(const std::unique_ptr<CFG> &cfg,unsigned id);
std::vector<CFGBlock *> getNonRecursiveSucc(CFGBlock *curBlock);


template <class T> void dumpLog(T &t, CheckerName cn, int level, Config &c) {
  auto block = c.getOptionBlock("PrintLog");
  int l = atoi(block.find("level")->second.c_str());
  switch (cn) {
  case common::CheckerName::ReturnStackAddrChecker:
    if (block.find("ReturnStackAddrChecker")->second == "true" && level >= l) {
      t.dump();
    }
    break;
  case common::CheckerName::Checker5:
    if (block.find("Checker5")->second == "true" && level >= l) {
      t.dump();
    }
    break;
  }  
}
std::string print(const Stmt* stmt);
} // end of namespace common

namespace fix {
class POffset {
public:
  std::string type;
  std::string val;
  bool needBracket;
  POffset(std::string t, std::string v) : type(t), val(v), needBracket(false) {}
  POffset() {}
  POffset operator+(const POffset &s);
  POffset operator+(const std::string &s);
};

class BufCalculator {
public:
  std::string pType;
  std::string pName;
  // string unit = "char";
  POffset calculateExpr(Stmt *stmt);
  BufCalculator(std::string type, std::string name)
      : pType(type), pName(name){};
  std::string calculateBufLen(std::string bufSize, POffset bufOffset,
                              std::string type);
  // POffset cast(POffset p, string type);
};

std::string Trim(std::string s);
// Context getContext(ASTContext* context, Stmt *expr);
std::string getSourceCode(const Stmt *stmt);
// string rmBrackets(string str);
SourceLocation getEnd(SourceManager &sm, const Stmt *expr);
void split(std::vector<std::string> &res, std::string str,
           std::string delimiter = ",");
bool contains(const Stmt *a, const Stmt *b);
Stmt *findChild(Stmt *parent, std::string child);
std::pair<std::string, std::string> getArrayTypeLen(FunctionDecl *funcDecl,
                                                    Stmt *pointer);
bool isCharKind(std::string type);

std::pair<std::string, std::string> getTypeLenFromStr(std::string str);

} // end of namespace fix

#endif

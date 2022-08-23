#ifndef AST_MANAGER_H
#define AST_MANAGER_H

#include <list>
#include <unordered_map>
#include <queue>

// #include <clang/Analysis/CFG.h>
#include <clang/Frontend/ASTUnit.h>

#include "ASTElement.h"
#include "Config.h"

// #include "../CFG/CFG.h"
#include <clang/Analysis/CFG.h>

using namespace clang;

class ASTManager;
/**
 * the resource of AST.
 * contains AST, function, function-local variables, global variables.
 */
class ASTResource {
public:
  ~ASTResource();

  const std::vector<ASTFunction *> &getFunctions(bool use = true) const;
  std::vector<ASTFile *> getASTFiles() const;
  std::vector<ASTGlobalVariable *> &getGlabalVars() { return ASTGlobalVars; };

  friend class ASTManager;

private:
  std::unordered_map<std::string, ASTFile *> ASTs;

  std::vector<ASTFunction *> ASTFunctions;
  std::vector<ASTFunction *> useASTFunctions;
  std::vector<ASTGlobalVariable *> ASTGlobalVars;
  std::vector<ASTVariable *> ASTVariables;
  std::vector<ASTFieldVariable *> ASTFieldVariables;

  void buildUseFunctions();

  ASTFile *addASTFile(std::string AST);
  ASTFunction *addASTFunction(FunctionDecl *FD, ASTFile *AF, bool use = true);
  ASTFunction *addLambdaASTFunction(FunctionDecl *FD, ASTFile *AF,
                                    std::string fullName, bool use = true);
  ASTGlobalVariable *addASTGlobalVar(VarDecl *VD, ASTFile *AF);
  ASTVariable *addASTVariable(VarDecl *VD, ASTFunction *F);
  ASTFieldVariable *addASTFieldVariable(VarDecl *VD, FieldDecl *FD, ASTFunction *F);
};

/**
 * a bidirectional map.
 * You can get a pointer from an id or get an id from a pointer.
 */
class ASTBimap {
public:
  friend class ASTManager;

private:
  void insertFunction(ASTFunction *F, FunctionDecl *FD);
  void insertVariable(ASTVariable *V, VarDecl *VD);
  void insertGlobalVariable(ASTGlobalVariable *V, VarDecl *VD);
  void insertFieldVariable(ASTFieldVariable *V);
  void insertVar2Field(ASTVariable* V, std::set<ASTFieldVariable *> F);

  FunctionDecl *getFunctionDecl(ASTFunction *F);
  ASTVariable *getASTVariable(VarDecl *VD);
  VarDecl *getVarDecl(ASTVariable *V);
  ASTGlobalVariable *getASTGlobalVariable(VarDecl *VD);
  VarDecl *getGlobalVarDecl(ASTGlobalVariable *GV);
  ASTFieldVariable *getASTFieldVariable(VarDecl *VD, FieldDecl *FD, std::vector<FieldDecl *> fathers);
  std::set<ASTFieldVariable *> getAllFields(ASTVariable *V);

  void removeFunction(ASTFunction *F);
  void removeVariable(ASTVariable *V);
  void removeGlobalVariable(ASTGlobalVariable *GV);

  std::unordered_map<ASTFunction *, FunctionDecl *> functionMap;
  std::unordered_map<ASTVariable *, VarDecl *> variableLeft;
  std::unordered_map<VarDecl *, ASTVariable *> variableRight;
  std::unordered_map<ASTGlobalVariable *, VarDecl *> astgv2gvd;
  std::unordered_map<VarDecl *, ASTGlobalVariable *> gvd2astgv;
  std::unordered_map<ASTVariable *, std::set<ASTFieldVariable *>> var2fields;

  // struct field_var_info{
  //   VarDecl *VD;
  //   FieldDecl *FD;
  //   bool operator==(const field_var_info fvi) const{
  //     return (VD == fvi.VD) && (FD == fvi.FD);
  //   }
  // };
  // struct hash_name {
  //   size_t operator()(const field_var_info& f) const {
  //       return std::hash<VarDecl *>()(f.VD) ^ std::hash<FieldDecl *>()(f.FD);
  //   }
  // };
  std::set<ASTFieldVariable *> fieldVariableSet;
};

class FunctionLoc {
public:
  FunctionDecl *FD;
  std::string fileName;
  int beginLoc;
  int endLoc;
  bool operator<(const FunctionLoc &a) const { return a.beginLoc < beginLoc; }

  FunctionLoc(FunctionDecl *D, std::string name, int begin, int end)
      : FD(D), fileName(name), beginLoc(begin), endLoc(end) {}
};
/**
 * a class that manages all ASTs.
 */
class ASTManager {
public:
  ASTManager(std::vector<std::string> &ASTs, ASTResource &resource,
             Config &configure);

  ASTUnit *getASTUnit(ASTFile *AF);
  FunctionDecl *getFunctionDecl(ASTFunction *F);
  ASTFunction *getASTFunction(FunctionDecl *FD);
  std::vector<ASTFunction *> getFunctions(bool use = true);

  struct field_identity {
    FieldDecl *field;
    std::vector<FieldDecl *> fathers;
  };
  void addAllFields(ASTVariable *root, VarDecl *VD, ASTResource &resource, ASTFunction *F);
  void addAllClassFields(ASTFunction *F, FunctionDecl *FD);

  ASTGlobalVariable *getASTGlobalVariable(VarDecl *GVD);
  VarDecl *getGlobalVarDecl(ASTGlobalVariable *GV);
  std::vector<ASTGlobalVariable *> getGlobalVars(bool uninit = false);

  ASTVariable *getASTVariable(VarDecl *VD);
  std::set<ASTFieldVariable *> getAllFields(ASTVariable *V);
  VarDecl *getVarDecl(ASTVariable *V);
  ASTFieldVariable *getASTFieldVariable(VarDecl *VD, FieldDecl *FD, std::vector<FieldDecl *> fathers);

  std::unique_ptr<CFG> &getCFG(ASTFunction *F);
  std::vector<ASTFunction *> getASTFunction(const std::string &funcName);

  void insertFunction(ASTFunction *F, FunctionDecl *FD);

  std::map<std::string, std::set<FunctionLoc>> funcLocInfo;
  void saveFuncLocInfo(FunctionLoc FL);
  CFGBlock *getBlockWithLoc(std::string fileName, int line);
  Stmt *getStmtWithLoc(std::string fileName, int line);
  std::vector<CFGBlock *> getBlocksWithLoc(std::string fileName, int line);
  std::vector<std::pair<CFGBlock *, Stmt *>>
  getCandidatePair(std::string fileName, int line);
  std::vector<Stmt *> getStmtWithLoc(int line, CFGBlock *block);

  void setMaxSize(unsigned size);

  std::unordered_map<std::string, ASTUnit *> &getASTs();
  std::unordered_map<CFG *, Decl *> parentDecls;

  bool isRecord(VarDecl *vd);  
  bool isRecord(FieldDecl *fd);
  RecordDecl *getRecord(VarDecl *vd);
  RecordDecl *getRecord(FieldDecl *fd);
  clang::QualType getRealType(VarDecl *VD);
  clang::QualType getRealType(FieldDecl *FD);
private:
  ASTResource &resource;
  Config &c;

  ASTBimap bimap;
  std::unordered_map<std::string, ASTUnit *> ASTs;
  std::unordered_map<ASTFunction *, std::unique_ptr<CFG>> CFGs;
  

  unsigned max_size;
  std::list<std::unique_ptr<ASTUnit>> ASTQueue;

  void pop();
  void move(ASTUnit *AU);
  void push(std::unique_ptr<ASTUnit> AU);

  void loadASTUnit(std::unique_ptr<ASTUnit> AU);
};

#endif
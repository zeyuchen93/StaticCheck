#ifndef AST_ELEMENT_H
#define AST_ELEMENT_H

#include <string>
#include <vector>

#include <clang/Frontend/ASTUnit.h>

#include "Common.h"

using namespace clang;

class ASTFunction;
class ASTVariable;
class ASTGlobalVariable;
class ASTFieldVariable;
class ASTArrayVariable;

class ASTFile {
public:
  ASTFile(unsigned id, std::string AST) : id(id), AST(AST){};

  const std::string &getAST() const { return AST; }

  void addFunction(ASTFunction *F) { functions.push_back(F); }
  void addGlobalVar(ASTGlobalVariable *GV) { globalvars.push_back(GV); }

  const std::vector<ASTFunction *> &getFunctions() const { return functions; }
  const std::vector<ASTGlobalVariable *> &getGlobalVars() const {
    return globalvars;
  }

private:
  unsigned id;
  std::string AST;

  std::vector<ASTFunction *> functions;
  std::vector<ASTGlobalVariable *> globalvars;
};

class ASTElement {
public: 
  ASTElement(unsigned id, std::string name, ASTFile *AF)
      : id(id), name(name), AF(AF) {}

  unsigned getID() const { return id; }

  const std::string &getName() const { return name; }

  ASTFile *getASTFile() const { return AF; }

  const std::string &getAST() const { return AF->getAST(); }

protected:
  unsigned id;
  std::string name;

  ASTFile *AF;
};

class ASTFunction : public ASTElement {
public:
  //表示该ASTFunction是什么类型
  enum Kind {
    //源代码中定义的一般函数
    NormalFunction,
    //源代码中没有定义的库函数，比如标准库中的函数
    LibFunction,
    //匿名函数，即lambda表达式
    AnonymousFunction,
  };
  ASTFunction(unsigned id, FunctionDecl *FD, ASTFile *AF, bool use = true)
      : ASTElement(id, FD->getNameAsString(), AF) {

    this->use = use;
    funcName = FD->getQualifiedNameAsString();
    fullName = common::getFullName(FD);
    param_size = FD->param_size();
    is_class_member = FD->isCXXClassMember();

    if (FD->hasBody())
      functionType = ASTFunction::NormalFunction;
    else
      functionType = ASTFunction::LibFunction;
  }

  ASTFunction(unsigned id, FunctionDecl *FD, ASTFile *AF,
              std::string funFullName, bool use = true,
              Kind kind = ASTFunction::AnonymousFunction)
      : ASTElement(id, FD->getQualifiedNameAsString(), AF) {

    this->use = use;

    fullName = funFullName;
    funcName = FD->getQualifiedNameAsString();

    param_size = FD->param_size();
    functionType = kind;
    is_class_member = FD->isCXXClassMember();
  }

  void addVariable(ASTVariable *V) { variables.push_back(V); }

  void addFieldVariable(ASTFieldVariable *V) { field_variables.push_back(V); }

  void addClassField(ASTFieldVariable *V) { class_fields.push_back(V); }

  unsigned getParamSize() const { return param_size; }

  const std::string &getFullName() const { return fullName; }

  const std::string &getFunctionName() const { return funcName; }

  const std::vector<ASTVariable *> &getVariables() const { return variables; }

  const std::vector<ASTFieldVariable *> &getFieldVariables() const { return field_variables; }

  const std::vector<ASTFieldVariable *> &getClassFields() const { return class_fields; }

  bool isUse() const { return use; }

  void setUse(bool used) { use = used; }

  Kind getFunctionType() const { return functionType; }

  bool isMemberFunc() const { return is_class_member; }

private:
  std::string funcName;
  std::string fullName;
  unsigned param_size;

  Kind functionType;
  bool use;
  bool is_class_member;

  std::vector<ASTVariable *> variables;
  std::vector<ASTFieldVariable *> field_variables;
  std::vector<ASTFieldVariable *> class_fields;
};

class ASTGlobalVariable : public ASTElement {
public:
  /**
   * @param: id  SAFE-HW 框架自定义的 id 规则，是 vector 长度
   *             不是 Clang-AST Decl->getID()
   */
  ASTGlobalVariable(unsigned id, VarDecl *VD, ASTFile *AF)
      : ASTElement(id, VD->getNameAsString(), AF) {
    isExtern = VD->hasExternalStorage();
    hasExplicitInit = VD->hasInit();
    clangid = VD->getID();
    auto &sm = VD->getASTContext().getSourceManager();
    loc = VD->getLocation().printToString(sm);
  }

public:
  bool isExtern;
  bool hasExplicitInit; // in many compilers, globar will has default value
  int64_t clangid;      // Clang-AST id
  std::string loc;
};

class ASTVariable : public ASTElement {
public:
  ASTVariable(unsigned id, VarDecl *VD, ASTFunction *F)
      : ASTElement(id, VD->getNameAsString(), F->getASTFile()), F(F) {

    if (VD->getType()->isPointerType() || VD->getType()->isReferenceType())
      pointer_reference_type = true;
    else
      pointer_reference_type = false;
  }

  ASTFunction *getFunction() const { return F; }

  bool isPointerOrReferenceType() const { return pointer_reference_type; }

/**
 * Field APIs
**/

  bool hasField() const { return fields.size() != 0; }

  void addField(ASTFieldVariable *field) { fields.push_back(field); }

  std::vector<ASTFieldVariable *> getFields() { return fields; }

/**
 * Array APIs
**/

  bool isArray() const { return array_elements.size() != 0; }

  void addArrayElements(ASTArrayVariable *element) {array_elements.push_back(element); }

  std::vector<ASTArrayVariable *> getArrayElements() { return array_elements; }

private:
  bool pointer_reference_type;
  std::vector<ASTFieldVariable *> fields;
  std::vector<ASTArrayVariable *> array_elements;

  ASTFunction *F;
};

class ASTFieldVariable : public ASTElement {
public:
  ASTFieldVariable(unsigned id, VarDecl *VD, FieldDecl *FD, ASTFunction *F) 
    : ASTElement(id, FD->getNameAsString(), F->getASTFile()), 
      F(F), FD(FD), VD(VD) {
        if(FD->getType()->isPointerType() || FD->getType()->isReferenceType()) 
          pointer_reference_type = true;
        else
          pointer_reference_type = false;

        this->isClassField = false;
        this->nesting = false;
        this->fatherField = nullptr;
      }
  
  ASTFieldVariable(unsigned id, FieldDecl *FD, ASTFunction *F) 
    : ASTElement(id, FD->getNameAsString(), F->getASTFile()), 
      F(F), FD(FD), VD(nullptr) {
        if(FD->getType()->isPointerType() || FD->getType()->isReferenceType()) 
          pointer_reference_type = true;
        else
          pointer_reference_type = false;

        this->isClassField = true;
        this->nesting = false;
        this->fatherField = nullptr;
      }  
  
  ASTFunction *getFunction() const { return F; }

  bool isPointerOrReferenceType() const { return pointer_reference_type; }

  VarDecl *getVar(){ return VD; }

  FieldDecl *getFieldDecl(){ return FD; }
  
  std::vector<ASTFieldVariable *> getChildren() { return childFields; }

  ASTFieldVariable *getFather() { return fatherField; }

  bool isSameField(ASTFieldVariable *FV){ return FD == FV->FD; }

  bool hasChild() { return childFields.size() != 0; }

  bool hasFather() { return nesting; }

  void setFather(ASTFieldVariable *father) { this->fatherField = father; nesting = true; }

  void addNestingField(ASTFieldVariable *FV) { childFields.push_back(FV); }

  bool isClassMember() { return isClassField; }

  bool isSameFieldAsFather() {
    if(hasFather()) {
      ASTFieldVariable *current = getFather();
      if(FD == current->getFieldDecl()) return true;
      while(current->hasFather()) {
        current = current->getFather();
        if(FD == current->getFieldDecl()) return true;
      }
    }
    return false;
  }

  bool isSameFieldAsFather(FieldDecl *child_fd) {
    if(child_fd == FD) return true;
    if(hasFather()) {
      ASTFieldVariable *current = getFather();
      if(current->getFieldDecl() == child_fd) return true;
      while(current->hasFather()) {
        current = current->getFather();
        if(current->getFieldDecl() == child_fd) return true;
      }
    }
    return false;
  }

private:
/**
 * 1. normal member variable
 * 2. nesting member variable
 * 3. class/struct/union member variable(whose father is a this pointer)
 * 
*/  
//default(normal variable)
  VarDecl *VD;
  FieldDecl *FD;
  bool pointer_reference_type;  
  ASTFunction *F;

//class member variable
  bool isClassField;

//nesting variable
//as father
  std::vector<ASTFieldVariable *> childFields;

//as child
  bool nesting;
  ASTFieldVariable *fatherField;
};

class ASTArrayVariable : public ASTElement {
public:
  ASTArrayVariable(unsigned id, VarDecl *VD, int index, ASTFunction *F) 
    : ASTElement(id, VD->getNameAsString() + "[" + std::to_string(index) + "]", F->getASTFile()), 
      F(F), VD(VD) {

      }

    VarDecl *getVar() { return VD; }
private:
  VarDecl *VD;
  int index;
  ASTFunction *F;
};

#endif

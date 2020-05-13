#ifndef AST_H
#define AST_H

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <iostream>
#include <string>
#include <map>
#include <ostream>

using namespace std;

class Node;
class Expression;
class Statement;
class Integer;
class Real;
class Char;
class Boolean;
class ConstValue;
class Identifier;
class ConstDeclaration;
class TypeDeclaration;
class VarDeclaration;
class Type;
class EnumType;
class ArrayType;
class RecordType;
class ConstRangeType;
class EnumRangeType;
class FieldDeclaration;
class FuncDeclaration;
class Parameter;
class Routine;
class Program;
class AssignStatement;
class BinaryExpression;
class ArrayReference;
class RecordReference;
class FunctionCall;
class SysFunctionCall;
class ProcedureCall;
class SysProcedureCall;
class IfStatement;
class RepeatStatement;
class WhileStatement;
class ForStatement;
class CaseStatement;
class CaseExpression;
class GotoStatement;
class CompoundStatement;

using ExpressionList = vector<Expression *>;
using StatementList = vector<Statement *>;
using ConstDeclList = vector<ConstDeclaration *>;
using VarDeclList = vector<VarDeclaration *>;
using TypeDeclList = vector<TypeDeclaration *>;
using NameList = vector<Identifier *>;
using FieldList = vector<FieldDeclaration *>;
using ParaList = vector<Parameter *>;
using RoutineList = vector<FuncDeclaration *>;
using StatementList = vector<Statement *>;
using ArgsList = vector<Expression *>;
using CaseExprList = vector<CaseExpression *>;

enum BuildInType
{
    SPL_INTEGER,
    SPL_REAL,
    SPL_CHAR,
    SPL_BOOLEAN
};

class Node
{
public:
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) = 0;
    virtual string getJson() {

    };
    virtual ~Node()
    {
    }
};

class Expression : public Node
{
};

class Statement : public Node
{
public:
    void setLabel(int label) {
        this->label = label;
     }
private:
    int label = -1;
};

class Identifier : public Expression
{
public:
    Identifier(string *name) : name(name)
    {
    }
    string getName() {
        return *name;
    }
    virtual string getJson() override;
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
private:
    string *name;
};

class ConstValue : public Expression
{
public:
    union Value {
        int i;
        double r;
        bool b;
        char c;
    };
    virtual BuildInType getType() = 0;
    virtual ConstValue::Value getValue() = 0;
    virtual ConstValue *operator-() = 0;
};

class Integer : public ConstValue
{
public:
    Integer(int value) : value(value)
    {
    }
    virtual BuildInType getType() override
    {
        return SPL_INTEGER;
    }
    virtual ConstValue::Value getValue() override
    {
        Value v;
        v.i = value;
        return v;
    }
    virtual ConstValue *operator-() override {
        return new Integer(-value);
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    
    virtual string getJson() override;
private:
    int value;
};

class Real : public ConstValue
{
public:
    Real(double value) : value(value)
    {
    }
    virtual BuildInType getType() override
    {
        return SPL_REAL;
    }
    virtual ConstValue::Value getValue() override
    {
        Value v;
        v.r = value;
        return v;
    }
    virtual ConstValue *operator-() override {
        return new Integer(-value);
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    double value;
};

class Char : public ConstValue
{
public:
    Char(char value) : value(value)
    {
    }
    virtual BuildInType getType() override
    {
        return SPL_CHAR;
    }
    virtual ConstValue::Value getValue() override {
        Value v;
        v.c = value;
        return v;
    }
    virtual ConstValue *operator-() override {
        return new Integer(-value);
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    char value;
};

class Boolean : public ConstValue
{
public:
    Boolean(bool value) : value(value)
    {
    }
    virtual BuildInType getType() override
    {
        return SPL_BOOLEAN;
    }
    virtual ConstValue::Value getValue() override {
        Value v;
        v.b = value;
        return v;
    }
    virtual ConstValue *operator-() override {
        return new Integer(!value);
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    bool value;
};

class ConstDeclaration : public Statement
{
public:
    ConstDeclaration(Identifier *ip, ConstValue *cp) : name(ip), value(cp)
    {
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    
    virtual string getJson() override;
private:
    Identifier *name;
    ConstValue *value;
};

class EnumType : public Statement {
public:
    EnumType(NameList *nl) : enumList(nl) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    NameList *enumList;
};

class ArrayType : public Statement {
public:
    ArrayType(Type *type, Type *range) : type(type), range(range) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    Type *range;
    Type *type;
};

class ConstRangeType : public Statement {
public:
    ConstRangeType(ConstValue *lowBound, ConstValue *upBound) : lowBound(lowBound), upBound(upBound) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    ConstValue *lowBound;
    ConstValue *upBound;
};

class EnumRangeType : public Statement {
public:
    EnumRangeType(Identifier *lowBound, Identifier *upBound) : lowBound(lowBound), upBound(upBound) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    Identifier *lowBound;
    Identifier *upBound;
};

class FieldDeclaration : public Statement {
public:
    FieldDeclaration(NameList *nameList, Type *td) : nameList(nameList), type(td) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    NameList *nameList;
    Type *type;
};

class RecordType : public Statement {
public:
    RecordType(FieldList *fl) : fieldList(fl) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    
    virtual string getJson() override;
private:
    FieldList *fieldList;
};

// 类型
class Type : public Statement {
public:
    enum TypeOfType {
        SPL_ARRAY,
        SPL_RECORD,
        SPL_ENUM,
        SPL_CONST_RANGE,
        SPL_ENUM_RANGE,
        SPL_BUILD_IN,
        SPL_USER_DEFINE,
        SPL_VOID
    };
    Type(ArrayType *at) : arrayType(at), type(SPL_ARRAY) { }
    Type(RecordType *rt) : recordType(rt), type(SPL_RECORD) { }
    Type(EnumType *et) : enumType(et), type(SPL_ENUM) { }
    Type(ConstRangeType *crt) : constRangeType(crt), type(SPL_CONST_RANGE) { }
    Type(EnumRangeType *ert) : enumRangeType(ert), type(SPL_ENUM_RANGE) { }
    Type(BuildInType bt) : buildInType(bt), type(SPL_BUILD_IN) { }
    Type(Identifier *udt) : userDefineType(udt), type(SPL_USER_DEFINE) { }
    Type() : type(SPL_VOID) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    ArrayType *arrayType;
    RecordType *recordType;
    EnumType *enumType;
    ConstRangeType *constRangeType;
    EnumRangeType *enumRangeType;
    BuildInType buildInType;
    Identifier *userDefineType;
    TypeOfType type;
};


class TypeDeclaration : public Statement {
public:
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    TypeDeclaration(Identifier *name, Type *type) : name(name), type(type) {

    }

    virtual string getJson() override;
private:
    Identifier *name;
    Type *type;
};


class VarDeclaration : public Statement {
public:
    VarDeclaration(NameList *nl, Type *td) : nameList(nl), type(td) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    NameList *nameList;
    Type *type;
};

class FuncDeclaration : public Statement {
public: 
    FuncDeclaration(Identifier *name, ParaList *paraList, Type *returnType) : name(name), paraList(paraList), returnType(returnType) {

    }
    FuncDeclaration(Identifier *name, ParaList *paraList) : name(name), paraList(paraList) {

    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    void setRoutine(Routine *routine) {
        subRoutine = routine;
    }

    virtual string getJson() override;
private:
    Identifier *name;
    ParaList *paraList;
    Type *returnType;
    Routine *subRoutine;
};

class Parameter : public Statement {
public:
    Parameter(NameList *nl, bool isVar) : nameList(nl), isVar(isVar) { }
    void setType(Type *type) {
        this->type = type;
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    bool isVar;
    NameList *nameList;
    Type *type;
};

class Routine : public Node {
public:
    Routine(ConstDeclList *cd, TypeDeclList *tp, VarDeclList *vd, RoutineList *rl) 
                : constDeclList(cd), varDeclList(vd), typeDeclList(tp), routineList(rl) {

    }
    void setRoutineBody(CompoundStatement *routineBody) { this->routineBody = routineBody; }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    ConstDeclList *constDeclList;
    VarDeclList *varDeclList;
    TypeDeclList *typeDeclList;
    RoutineList *routineList;
    CompoundStatement *routineBody;
};

class Program : public Node {
public:
    Program(string *programID, Routine *routine) : programID(programID), routine(routine) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    virtual string getJson() override;
private:
    string *programID;
    Routine *routine;
};

class AssignStatement : public Statement {
public:
    AssignStatement(Identifier *lhs, Expression *rhs) : lhs(lhs), rhs(rhs), type(ID_ASSIGN) { }
    AssignStatement(Identifier *lhs, Expression *sub, Expression *rhs) : lhs(lhs), sub(sub), rhs(rhs), type(ARRAY_ASSIGN) { }
    AssignStatement(Identifier *lhs, Identifier *field, Expression *rhs) : lhs(lhs), field(field), rhs(rhs), type(RECORD_ASSIGN) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    
    virtual string getJson() override;
private:
    enum AssignType {
        ID_ASSIGN,
        ARRAY_ASSIGN,
        RECORD_ASSIGN
    };
    Identifier *lhs;
    Expression *rhs;
    Expression *sub;
    Identifier *field;
    AssignType type;
};

class BinaryExpression : public Expression {
public:
    enum BinaryOperator {
        SPL_PLUS,
        SPL_MINUS,
        SPL_MUL,
        SPL_DIV,
        SPL_GE,
        SPL_GT, 
        SPL_LT,
        SPL_LE,
        SPL_EQUAL,
        SPL_UNEQUAL,
        SPL_OR,
        SPL_MOD,
        SPL_AND,
        SPL_XOR,
    };
    BinaryExpression(Expression *lhs, BinaryOperator op, Expression *rhs) : lhs(lhs), op(op), rhs(rhs) {     }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    vector<string> opString{"+", "-", "*", "/", ">=", ">", "<", "<=", "==", "!=", "or", "mod", "and", "xor"};
    Expression *lhs;
    Expression *rhs;
    BinaryOperator op;
};

class ArrayReference : public Expression {
public:
    ArrayReference(Identifier *array, Expression *index) : array(array), index(index) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Identifier *array;
    Expression *index;
};

class RecordReference : public Expression {
public:
    RecordReference(Identifier *record, Identifier *field) : record(record), field(field) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Identifier *record;
    Identifier *field;
};

class FunctionCall : public Expression, public Statement{
public:
    FunctionCall(Identifier *name) : function(name), args(new ArgsList()) { }
    FunctionCall(Identifier *name, ArgsList *args) : function(name), args(args) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Identifier *function;
    ArgsList *args;
};

class ProcedureCall : public Statement {
public:
    ProcedureCall(Identifier *name) : function(name), args(new ArgsList()) { }
    ProcedureCall(Identifier *name, ArgsList *args) : function(name), args(args) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Identifier *function;
    ArgsList *args;
};

class SysFunctionCall : public Expression, public Statement{
public:
    SysFunctionCall(string *name) : function(getFunction(name)), name(name) { }
    SysFunctionCall(string *name, ArgsList *args) : function(getFunction(name)), args(args), name(name) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    enum SysFunction {
        SPL_ABS, 
        SPL_CHR, 
        SPL_ODD, 
        SPL_ORD, 
        SPL_PRED, 
        SPL_SQR, 
        SPL_SQRT, 
        SPL_SUCC,
        SPL_ERROR_FUCNTION
    };
    SysFunction getFunction(string *name) {
        string &sname = *name;
        if(sname == "abs")
            return SPL_ABS;
        else if(sname == "chr")
            return SPL_CHR;
        else if(sname == "odd")
            return SPL_ODD;
        else if(sname == "ord")
            return SPL_ORD;
        else if(sname == "pred")
            return SPL_PRED;
        else if(sname == "sqr")
            return SPL_SQR;
        else if(sname == "sqrt")
            return SPL_SQRT;
        else if(sname == "succ")
            return SPL_SUCC;
        else
            return SPL_ERROR_FUCNTION;
    }
    SysFunction function;
    ArgsList *args;
    string *name;
};

class SysProcedureCall : public Statement {
public:
    SysProcedureCall(string *name) : procedure(getProcedure(name)), name(name) { }
    SysProcedureCall(string *name, ArgsList *args) : procedure(getProcedure(name)), args(args), name(name) { }
    SysProcedureCall(string *name, Expression *expr) : procedure(getProcedure(name)), args(new ArgsList()), name(name) {
        args->push_back(expr);
    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    enum SysProcedure {
        SPL_WRITE,
        SPL_WRITELN,
        SPL_READ,
        SPL_ERROR_PROCEDURE
    };
    SysProcedure getProcedure(string *name) {
        string &sname = *name;
        if(sname == "write")
            return SPL_WRITE;
        else if(sname == "writeln")
            return SPL_WRITELN;
        else if(sname == "read")
            return SPL_READ;
        else
            return SPL_ERROR_PROCEDURE;
    }
    SysProcedure procedure;
    ArgsList *args;
    string *name;
};

class IfStatement : public Statement {
public:
    IfStatement(Expression *condition, Statement *thenStatement, Statement *elseStatement) : 
                                        condition(condition), thenStatement(thenStatement), elseStatement(elseStatement) {

                                        }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Expression *condition;
    Statement *thenStatement;
    // elseStatement can be nullptr 你需要自己检测下是否是空指针
    Statement *elseStatement;
};

class RepeatStatement : public Statement {
public:
    RepeatStatement(Expression *condition, StatementList *stmtList) : condition(condition), repeatStatement(stmtList) {

    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;
    
    virtual string getJson() override;
private:
    Expression *condition;
    StatementList *repeatStatement;
};

class WhileStatement : public Statement {
public:
    WhileStatement(Expression *condition, Statement *stmt) : condition(condition), stmt(stmt) {

    }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Expression *condition;
    Statement *stmt;
};

class ForStatement : public Statement {
public:
    ForStatement(Identifier *var, Expression *value, bool isAdd, Expression *step, Statement *stmt) :
                                var(var), value(value), isAdd(isAdd), step(step), stmt(stmt) {

                                }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;   
private:
    Identifier *var;
    Expression *value;
    // 递增:true 递减:false
    bool isAdd;
    Expression *step;
    Statement *stmt;
};

class CaseStatement : public Statement {
public:
    CaseStatement(Expression *value, CaseExprList *caseExprList) : value(value), caseExprList(caseExprList) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    Expression *value;
    CaseExprList *caseExprList;
};

class CaseExpression : public Statement {
public:
    CaseExpression(Expression *value, Statement *stmt) : value(value), stmt(stmt) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    // 可以与CaseStatement中的变量建立EQUAL的BinaryStatment进行条件判断
    Expression *value;
    Statement *stmt;
};

class GotoStatement : public Statement {
public:
    GotoStatement(int label) : label(label) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    int label;
};

class CompoundStatement : public Statement {
public:
    CompoundStatement(StatementList *stmtList) : stmtList(stmtList) { }
    virtual llvm::Value *codeGen(llvm::LLVMContext &context) override;

    virtual string getJson() override;
private:
    StatementList *stmtList;
};

#endif
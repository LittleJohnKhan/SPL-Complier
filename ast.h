#ifndef AST_H
#define AST_H

#include <llvm/IR/Value.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ostream>

using namespace std;

class CodeGenerator;

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
class AstType;
class EnumType;
class AstArrayType;
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

// SPL 内置类型
enum BuildInType
{
    SPL_INTEGER,
    SPL_REAL,
    SPL_CHAR,
    SPL_BOOLEAN
};

// AST 节点
class Node
{
public:
    // 用于llvm生成中间代码
    virtual llvm::Value *codeGen(CodeGenerator & generator) = 0;
    // 用于生成AST可视化需要的Json数据
    virtual string getJson(){return "";};
    virtual ~Node()
    {
    }
};

// 表达式，特征是能返回值或能存储值
class Expression : public Node
{
};

// 语句，特征是能完成某些操作
class Statement : public Node
{
public:
    // 用于Goto语句设置标号
    void setLabel(int label)
    {
        this->label = label;
    }
    // 用于得到Goto语句需要的标号，若不存在则返回-1
    int getLable() const
    {
        return label;
    }
    void forward(CodeGenerator & generator);
    void backward(CodeGenerator & generator);
    llvm::BasicBlock *afterBB;
private:
    int label = -1;
};

// 标识符
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
private:
    string *name;
};

// 常量
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
    virtual bool isValidConstRangeType()
    {
        BuildInType t = getType();
        return t == SPL_INTEGER || t == SPL_CHAR;
    }
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    
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
        return new Real(-value);
    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

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
        return new Char(-value);
    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

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
        return new Boolean(!value);
    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    bool value;
};

// 常量声明语句
class ConstDeclaration : public Statement
{
public:
    ConstDeclaration(Identifier *ip, ConstValue *cp) : name(ip), value(cp), globalFlag(false)
    {
    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    
    virtual string getJson() override;
    void setGlobal()
    {
        globalFlag = true;
    }
    
    bool isGlobal()
    {
        return globalFlag;
    }
private:
    Identifier *name;
    ConstValue *value;
    AstType *type;
    bool globalFlag;
};

class EnumType : public Statement {
public:
    EnumType(NameList *nl) : enumList(nl) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;
private:
    NameList *enumList;
};

class AstArrayType : public Statement {
public:
    AstArrayType(AstType *type, AstType *range) : type(type), range(range) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;

    AstType *range;
    AstType *type;
};

class ConstRangeType : public Statement {
public:
    ConstRangeType(ConstValue *lowBound, ConstValue *upBound) : lowBound(lowBound), upBound(upBound) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;
    size_t size()
    {
        int s;
        if (lowBound->getType() == upBound->getType() && lowBound->isValidConstRangeType())
        {
            if (lowBound->getType() == SPL_INTEGER)
            {
//                std::cout << lowBound->getValue().i << ".." << upBound->getValue().i << std::endl;
                s = upBound->getValue().i - lowBound->getValue().i + 1;
            }
            else
            {
//                std::cout << lowBound->getValue().c << ".." << upBound->getValue().c << std::endl;
                s = upBound->getValue().c - lowBound->getValue().c + 1;
            }
            if (s <= 0)
            {
                throw std::range_error("[ERROR]low bound > up bound.");
            }
        }
        else
        {
            throw std::domain_error("[ERROR]Invalid range type.");
        }
        return s;
    }
    llvm::Value *mapIndex(llvm::Value* indexValue, CodeGenerator & generator);
private:
    ConstValue *lowBound;
    ConstValue *upBound;
};

class EnumRangeType : public Statement {
public:
    EnumRangeType(Identifier *lowBound, Identifier *upBound) : lowBound(lowBound), upBound(upBound) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;
    llvm::Value *lowValue, *upValue, *lowValueAddr, *upValueAddr;
    llvm::Value *mapIndex(llvm::Value *indexValue, CodeGenerator & generator);
    size_t size();
private:
    Identifier *lowBound;
    Identifier *upBound;
};

class FieldDeclaration : public Statement {
public:
    FieldDeclaration(NameList *nameList, AstType *td) : nameList(nameList), type(td) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;
private:
    NameList *nameList;
    AstType *type;
};

class RecordType : public Statement {
public:
    RecordType(FieldList *fl) : fieldList(fl) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    
    virtual string getJson() override;
private:
    FieldList *fieldList;
};

// 类型
class AstType : public Statement {
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
    AstType(AstArrayType *at) : arrayType(at), type(SPL_ARRAY) { }
    AstType(RecordType *rt) : recordType(rt), type(SPL_RECORD) { }
    AstType(EnumType *et) : enumType(et), type(SPL_ENUM) { }
    AstType(ConstRangeType *crt) : constRangeType(crt), type(SPL_CONST_RANGE) { }
    AstType(EnumRangeType *ert) : enumRangeType(ert), type(SPL_ENUM_RANGE) { }
    AstType(BuildInType bt) : buildInType(bt), type(SPL_BUILD_IN) { }
    AstType(Identifier *udt) : userDefineType(udt), type(SPL_USER_DEFINE) { }
    AstType() : type(SPL_VOID) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;
    llvm::Type* toLLVMType();
    llvm::Constant* initValue(ConstValue *v = nullptr);

    AstArrayType *arrayType;
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    TypeDeclaration(Identifier *name, AstType *type) : name(name), type(type) {

    }
    virtual string getJson() override;
private:
    Identifier *name;
    AstType *type;
};


class VarDeclaration : public Statement {
public:
    VarDeclaration(NameList *nl, AstType *td) : nameList(nl), type(td), globalFlag(false) {}
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
    
    void setGlobal()
    {
        globalFlag = true;
    }
    
    bool isGlobal()
    {
        return globalFlag;
    }
private:
    NameList *nameList;
    AstType *type;
    bool globalFlag;
};

class FuncDeclaration : public Statement {
public: 
    FuncDeclaration(Identifier *name, ParaList *paraList, AstType *returnType) : name(name), paraList(paraList), returnType(returnType) {

    }
    FuncDeclaration(Identifier *name, ParaList *paraList) : name(name), paraList(paraList) {
        returnType = new AstType();
    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    
    void setRoutine(Routine *routine) {
        subRoutine = routine;
    }

    virtual string getJson() override;
private:
    Identifier *name;
    ParaList *paraList;
    AstType *returnType;
    Routine *subRoutine;
};

class Parameter : public Statement {
public:
    Parameter(NameList *nl, bool isVar) : nameList(nl), isVar(isVar) { }
    void setType(AstType *type) {
        this->type = type;
    }
    AstType* getType()
    {
        return type;
    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;

    bool isVar;
    NameList *nameList;
    AstType *type;
};

class Routine : public Node {
public:
    Routine(ConstDeclList *cd, TypeDeclList *tp, VarDeclList *vd, RoutineList *rl) 
                : constDeclList(cd), varDeclList(vd), typeDeclList(tp), routineList(rl) {

    }
    void setRoutineBody(CompoundStatement *routineBody) { this->routineBody = routineBody; }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    virtual string getJson() override;
    void setGlobal()
    {
        for (auto & constDecl : *constDeclList)
        {
            constDecl->setGlobal();
        }
        for (auto & varDecl : *varDeclList)
        {
            varDecl->setGlobal();
        }
    }
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
    
    llvm::Value *getReference(CodeGenerator & generator);
    llvm::Type *getElementType(CodeGenerator & generator);
private:
    Identifier *array;
    Expression *index;
};

class RecordReference : public Expression {
public:
    RecordReference(Identifier *record, Identifier *field) : record(record), field(field) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    Identifier *record;
    Identifier *field;
};

class FunctionCall : public Expression, public Statement{
public:
    FunctionCall(Identifier *name) : function(name), args(new ArgsList()) { }
    FunctionCall(Identifier *name, ArgsList *args) : function(name), args(args) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    Identifier *function;
    ArgsList *args;
};

class ProcedureCall : public Statement {
public:
    ProcedureCall(Identifier *name) : function(name), args(new ArgsList()) { }
    ProcedureCall(Identifier *name, ArgsList *args) : function(name), args(args) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    Identifier *function;
    ArgsList *args;
};

class SysFunctionCall : public Expression, public Statement{
public:
    SysFunctionCall(string *name) : function(getFunction(name)), name(name) { }
    SysFunctionCall(string *name, ArgsList *args) : function(getFunction(name)), args(args), name(name) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
    
    llvm::Value *SysProcWrite(CodeGenerator & generator, bool isLineBreak);
    llvm::Value *SysProcRead(CodeGenerator & generator);
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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;
    
    virtual string getJson() override;
private:
    Expression *condition;
    StatementList *repeatStatement;
};

class WhileStatement : public Statement {
public:
    WhileStatement(Expression *condition, Statement *stmt) : condition(condition), stmt(stmt) {

    }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

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
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;   
private:
    Identifier *var;
    Expression *value;
    // increase:true decrease:false
    bool isAdd;
    Expression *step;
    Statement *stmt;
};

class CaseStatement : public Statement {
public:
    CaseStatement(Expression *value, CaseExprList *caseExprList) : value(value), caseExprList(caseExprList) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    Expression *value;
    CaseExprList *caseExprList;
};

class CaseExpression : public Statement {
public:
    CaseExpression(Expression *value, Statement *stmt) : value(value), stmt(stmt) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
    Expression *value;
private:
    // 可以与CaseStatement中的变量建立EQUAL的BinaryStatment进行条件判断
    Statement *stmt;
};

class GotoStatement : public Statement {
public:
    GotoStatement(int label) : toLabel(label) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    int toLabel;
};

class CompoundStatement : public Statement {
public:
    CompoundStatement(StatementList *stmtList) : stmtList(stmtList) { }
    virtual llvm::Value *codeGen(CodeGenerator & generator) override;

    virtual string getJson() override;
private:
    StatementList *stmtList;
};

#endif

%{
#include "ast.h"
#include <string>
#include <iostream>

using namespace std;

void yyerror(const char *s) { 
    std::printf("Error: %s\n", s);
    std::exit(1); 
}

int yylex(void);

Program *root;
%}
%union {
    int iVal;
    string *sVal;
    double dVal;
    bool bVal;
    char cVal;
    Node *node;
    Expression *expression;
    Statement *statement;
    Integer *integer;
    Real *real;
    Char *astChar;
    Boolean *boolean;
    ConstValue *constValue;
    Identifier *identifier;
    ConstDeclaration *constDeclaration;
    TypeDeclaration *typeDeclaration;
    VarDeclaration *varDeclaration;
    AstType *type;
    EnumType *enumType;
    AstArrayType *AstArrayType;
    RecordType  *recordType;
    ConstRangeType *constRangeType;
    EnumRangeType *enumRangeType;
    FieldDeclaration *fieldDeclaration;
    FuncDeclaration *funcDeclaration;
    Parameter *parameter;
    Routine *routine;
    Program *program;
    AssignStatement *assignStatement;
    BinaryExpression *binaryExpression;
    ArrayReference *arrayReference;
    RecordReference *recordReference;
    FunctionCall *functionCall;
    SysFunctionCall *sysFunctionCall;
    ProcedureCall *procedureCall;
    SysProcedureCall *sysProcedureCall;
    IfStatement *ifStatement;
    RepeatStatement *repeatStatement;
    WhileStatement *whileStatement;
    ForStatement *forStatement;
    CaseStatement *caseStatement;
    CaseExpression *caseExpression;
    GotoStatement *gotoStatement;

    ExpressionList *expressionList;
    StatementList *statementList;
    ConstDeclList *constDeclList;
    VarDeclList *varDeclList;
    TypeDeclList *typeDeclList;
    NameList *nameList;
    FieldList *fieldList;
    ParaList *paraList;
    RoutineList *routineList;
    ArgsList *argsList;
    CaseExprList *caseExprList;
    CompoundStatement *compoundStatement;
}

%token  LP RP LB RB DOT COMMA COLON
        MUL UNEQUAL NOT PLUS MINUS
        GE GT LE LT EQUAL ASSIGN MOD DOTDOT
        SEMI
        AND ARRAY TOKEN_BEGIN CASE CONST
        DIV DO DOWNTO ELSE END
        FOR FUNCTION GOTO
        IF OF OR
        PROCEDURE PROGRAM RECORD REPEAT
        THEN TO TYPE UNTIL VAR WHILE
%token<iVal> INTEGER
%token<sVal> IDENTIFIER SYS_CON SYS_FUNCT SYS_PROC SYS_TYPE READ
%token<dVal> REAL
%token<cVal> CHAR

%type<identifier>                       name
%type<program>                          program
%type<sVal>                             program_head
%type<routine>                          routine routine_head sub_routine
%type<constDeclList>                    const_part const_expr_list
%type<typeDeclList>                     type_part type_decl_list
%type<typeDeclaration>                  type_definition
%type<varDeclList>                      var_part var_decl_list
%type<varDeclaration>                   var_decl
%type<routineList>                      routine_part
%type<constValue>                       const_value
%type<type>                             type_decl simple_type_decl array_type_decl record_type_decl
%type<nameList>                         name_list
%type<fieldList>                        field_decl_list
%type<fieldDeclaration>                 field_decl
%type<funcDeclaration>                  function_decl procedure_decl function_head procedure_head
%type<paraList>                         parameters para_decl_list
%type<parameter>                        para_type_list var_para_list val_para_list
%type<statement>                        stmt non_label_stmt else_clause
%type<assignStatement>                  assign_stmt
%type<statement>                        proc_stmt     
%type<expressionList>                   expression_list
%type<expression>                       expression expr term factor
%type<argsList>                         args_list 
%type<ifStatement>                      if_stmt 
%type<repeatStatement>                  repeat_stmt
%type<whileStatement>                   while_stmt
%type<forStatement>                     for_stmt
%type<bVal>                             direction
%type<caseStatement>                    case_stmt
%type<caseExprList>                     case_expr_list
%type<caseExpression>                   case_expr
%type<gotoStatement>                    goto_stmt
%type<statementList>                    stmt_list
%type<compoundStatement>                routine_body compound_stmt 


%start program
%%

// 
name: IDENTIFIER                                                     { $$ = new Identifier($1); }
                                                                     ;

program: program_head routine DOT                                    { $$ = new Program($1, $2); root = $$; }
                                                                     ;

program_head: PROGRAM IDENTIFIER SEMI                                { $$ = $2; }
                                                                     ;

routine: routine_head routine_body                                   { $$ = $1; $$->setRoutineBody($2); }
                                                                     ;

routine_head: const_part  type_part  var_part  routine_part          { $$ = new Routine($1, $2, $3, $4); }
                                                                     ;

const_part : 
    CONST  const_expr_list                                           { $$ = $2; }
    |                                                                { $$ = new ConstDeclList(); }
                                                                     ;
const_expr_list : 
    const_expr_list  name  EQUAL  const_value  SEMI                  { $$ = $1; $$->push_back(new ConstDeclaration($2, $4)); }
    |  name  EQUAL  const_value  SEMI                                { $$ = new ConstDeclList(); $$->push_back(new ConstDeclaration($1, $3)); }
                                                                     ;
const_value :
    INTEGER                                                          { $$ = new Integer($1); }
    |  REAL                                                          { $$ = new Real($1); }
    |  CHAR                                                          { $$ = new Char($1); }
    |  SYS_CON                                                       {
                                                                         if(*$1 == "true")
                                                                             $$ = new Boolean(true);
                                                                         else if(*$1 == "false")
                                                                             $$ = new Boolean(false);
                                                                         else
                                                                             $$ = new Integer(0x7FFFFFFF);
                                                                     }
                                                                     ;

type_part : 
    TYPE type_decl_list                                              { $$ = $2; }
    |                                                                { $$ = new TypeDeclList(); }
                                                                     ;

type_decl_list : 
    type_decl_list  type_definition                                  { $$ = $1; $$->push_back($2); }
    |  type_definition                                               { $$ = new TypeDeclList(); $$->push_back($1); }
                                                                     ;

type_definition : 
    name  EQUAL  type_decl  SEMI                                     { $$ = new TypeDeclaration($1, $3); }
                                                                     ;

type_decl : 
    simple_type_decl                                                 { $$ = $1; }
    |  array_type_decl                                               { $$ = $1; }
    |  record_type_decl                                              { $$ = $1; }
                                                                     ;
simple_type_decl : 
    SYS_TYPE                                                         {
                                                                         if(*$1 == "integer")
                                                                             $$ = new AstType(SPL_INTEGER);
                                                                         else if(*$1 == "boolean")
                                                                             $$ = new AstType(SPL_BOOLEAN);
                                                                         else if(*$1 == "real")
                                                                             $$ = new AstType(SPL_REAL);
                                                                         else if(*$1 == "char")
                                                                             $$ = new AstType(SPL_CHAR);
                                                                         else
                                                                             cout << "UNKNOWN SYS_TYPE" << endl;
                                                                     }
    |  name                                                          { $$ = new AstType($1); }
    |  LP  name_list  RP                                             { $$ = new AstType(new EnumType($2)); }
    |  const_value  DOTDOT  const_value                              { $$ = new AstType(new ConstRangeType($1, $3)); }
    |  MINUS  const_value  DOTDOT  const_value                       { $$ = new AstType(new ConstRangeType(-*$2, $4)); }
    |  MINUS  const_value  DOTDOT  MINUS  const_value                { $$ = new AstType(new ConstRangeType(-*$2, -*$5)); }
    |  name  DOTDOT  name                                            { $$ = new AstType(new EnumRangeType($1, $3)); }
                                                                     ;

array_type_decl : ARRAY  LB  simple_type_decl  RB  OF  type_decl     { $$ = new AstType(new AstArrayType($6, $3)); }
                                                                     ;

record_type_decl : RECORD  field_decl_list  END                      { $$ = new AstType(new RecordType($2)); }
                                                                     ;

field_decl_list : 
    field_decl_list  field_decl                                      { $$ = $1; $$->push_back($2); }
    |  field_decl                                                    { $$ = new FieldList(); $$->push_back($1); }
                                                                     ;

field_decl : name_list  COLON  type_decl  SEMI                       { $$ = new FieldDeclaration($1, $3); }
                                                                     ;

name_list : 
    name_list  COMMA  name                                           { $$ = $1; $$->push_back($3); }
    |  name                                                          { $$ = new NameList(); $$->push_back($1); }
                                                                     ;

var_part : 
    VAR  var_decl_list                                               { $$ = $2; }
    |                                                                { $$ = new VarDeclList(); }
                                                                     ;

var_decl_list :  
    var_decl_list  var_decl                                          { $$ = $1; $$->push_back($2); }
    |  var_decl                                                      { $$ = new VarDeclList(); $$->push_back($1); }
                                                                     ;

var_decl :  name_list  COLON  type_decl  SEMI                        { $$ = new VarDeclaration($1, $3); }
                                                                     ;

routine_part:  
    routine_part  function_decl                                      { $$ = $1; $$->push_back($2); }
    |  routine_part  procedure_decl                                  { $$ = $1; $$->push_back($2); }
    |  function_decl                                                 { $$ = new RoutineList(); $$->push_back($1); }
    |  procedure_decl                                                { $$ = new RoutineList(); $$->push_back($1); }
    |                                                                { $$ = new RoutineList(); }
                                                                     ;

function_decl : function_head  SEMI  sub_routine  SEMI               { $$ = $1; $$->setRoutine($3); }
                                                                     ;

function_head :  FUNCTION  name  parameters  COLON  simple_type_decl { $$ = new FuncDeclaration($2, $3, $5); }
                                                                     ;

procedure_decl :  procedure_head  SEMI  sub_routine  SEMI            { $$ = $1; $$->setRoutine($3); }
                                                                     ;

procedure_head :  PROCEDURE name parameters                          { $$ = new FuncDeclaration($2, $3); }
                                                                     ;

parameters : 
    LP  para_decl_list  RP                                           { $$ = $2; }
    |                                                                { $$ = new ParaList(); }
                                                                     ;

para_decl_list : 
    para_decl_list  SEMI  para_type_list                             { $$ = $1; $$->push_back($3); }
    | para_type_list                                                 { $$ = new ParaList(); $$->push_back($1); }
                                                                     ;

para_type_list : 
    var_para_list COLON  simple_type_decl                            { $$ = $1, $$->setType($3); }
    |  val_para_list  COLON  simple_type_decl                        { $$ = $1, $$->setType($3); }
                                                                     ;

var_para_list : VAR  name_list                                       { $$ = new Parameter($2, true); }
                                                                     ;

val_para_list : name_list                                            { $$ = new Parameter($1, false); }
                                                                     ;

sub_routine : routine_head  routine_body                             { $$ = $1; $$->setRoutineBody($2); }
                                                                     ;

routine_body : compound_stmt                                         { $$ = $1; }
                                                                     ;

compound_stmt : TOKEN_BEGIN  stmt_list  END                                { $$ = new CompoundStatement($2); }
                                                                     ;
stmt_list : 
    stmt_list  stmt  SEMI                                            { $$ = $1; $$->push_back($2); }
    |                                                                { $$ = new StatementList(); }
                                                                     ;

stmt : 
    INTEGER  COLON  non_label_stmt                                   { $$ = $3; $$->setLabel($1); }
    |  non_label_stmt                                                { $$ = $1; }
                                                                     ;

non_label_stmt : 
    assign_stmt                                                      { $$ = $1; }
    | proc_stmt                                                      { $$ = $1; }
    | compound_stmt                                                  { $$ = $1; }
    | if_stmt                                                        { $$ = $1; }
    | repeat_stmt                                                    { $$ = $1; }
    | while_stmt                                                     { $$ = $1; }
    | for_stmt                                                       { $$ = $1; }
    | case_stmt                                                      { $$ = $1; }
    | goto_stmt                                                      { $$ = $1; }
                                                                     ;

assign_stmt : 
    name  ASSIGN  expression                                         { $$ = new AssignStatement($1, $3); }
    | name LB expression RB ASSIGN expression                        { $$ = new AssignStatement($1, $3, $6); }
    | name  DOT  name  ASSIGN  expression                            { $$ = new AssignStatement($1, $3, $5); }
                                                                     ;

proc_stmt : 
    name                                                             { $$ = new ProcedureCall($1); }
    |  name  LP  args_list  RP                                       { $$ = new ProcedureCall($1, $3); }
    |  SYS_PROC                                                      { $$ = new SysProcedureCall($1); }
    |  SYS_PROC  LP  expression_list  RP                             { $$ = new SysProcedureCall($1, $3); }
    |  READ  LP  factor  RP                                          { $$ = new SysProcedureCall($1, $3); }
                                                                     ;

expression_list : 
    expression_list  COMMA  expression                               { $$ = $1; $$->push_back($3); }
    |  expression                                                    { $$ = new ExpressionList(); $$->push_back($1); }
                                                                     ;

expression : 
    expression  GE  expr                                             { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_GE, $3); }
    |  expression  GT  expr                                          { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_GT, $3); }
    |  expression  LE  expr                                          { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_LE, $3); }
    |  expression  LT  expr                                          { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_LT, $3); }
    |  expression  EQUAL  expr                                       { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_EQUAL, $3); }
    |  expression  UNEQUAL  expr                                     { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_UNEQUAL, $3); }
    |  expr                                                          { $$ = $1; }
                                                                     ;

expr : 
    expr  PLUS  term                                                 { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_PLUS, $3); }
    |  expr  MINUS  term                                             { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_MINUS, $3); }
    |  expr  OR  term                                                { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_OR, $3); }
    |  term                                                          { $$ = $1; }
                                                                     ;

term : 
    term  MUL  factor                                                { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_MUL, $3); }
    |  term  DIV  factor                                             { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_DIV, $3); }
    |  term  MOD  factor                                             { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_MOD, $3); }
    |  term  AND  factor                                             { $$ = new BinaryExpression($1, BinaryExpression::BinaryOperator::SPL_AND, $3); }
    |  factor                                                        { $$ = $1; }
                                                                     ;

factor : 
    name                                                             { $$ = $1; }
    |  name  LP  args_list  RP                                       { $$ = new FunctionCall($1, $3); }
    |  SYS_FUNCT                                                     { $$ = new SysFunctionCall($1); }
    |  SYS_FUNCT  LP  args_list  RP                                  { $$ = new SysFunctionCall($1, $3); }
    |  const_value                                                   { $$ = $1; }
    |  LP  expression  RP                                            { $$ = $2; }
    |  NOT  factor                                                   { $$ = new BinaryExpression(new Boolean(true), BinaryExpression::BinaryOperator::SPL_XOR, $2); }
    |  MINUS  factor                                                 { $$ = new BinaryExpression(new Integer(0), BinaryExpression::BinaryOperator::SPL_MINUS, $2); }
    |  name  LB  expression  RB                                      { $$ = new ArrayReference($1, $3); }
    |  name  DOT  name                                               { $$ = new RecordReference($1, $3); }
                                                                     ;

args_list : 
    args_list  COMMA  expression                                     { $$ = $1; $$->push_back($3); }
    |  expression                                                    { $$ = new ArgsList(); $$->push_back($1); }
                                                                     ;

if_stmt : IF  expression  THEN  stmt  else_clause                    { $$ = new IfStatement($2, $4, $5); }
                                                                     ;

else_clause : 
    ELSE stmt                                                        { $$ = $2; }
    |                                                                { $$ = nullptr; }
                                                                     ;

repeat_stmt : REPEAT  stmt_list  UNTIL  expression                   { $$ = new RepeatStatement($4, $2); }
                                                                     ;

while_stmt : WHILE  expression  DO stmt                              { $$ = new WhileStatement($2, $4); }
                                                                     ;

for_stmt : 
    FOR  name  ASSIGN  expression  direction  expression  DO stmt    { $$ = new ForStatement($2, $4, $5, $6, $8); }
                                                                     ;

direction : 
    TO                                                               { $$ = true; }
    | DOWNTO                                                         { $$ = false; }
                                                                     ;

case_stmt : CASE expression OF case_expr_list  END                   { $$ = new CaseStatement($2, $4);}
                                                                     ;

case_expr_list : 
    case_expr_list  case_expr                                        { $$ = $1; $$->push_back($2); }
    |  case_expr                                                     { $$ = new CaseExprList(); $$->push_back($1); }
                                                                     ;

case_expr : 
    const_value  COLON  stmt  SEMI                                   { $$ = new CaseExpression($1, $3); }
    |  name  COLON  stmt  SEMI                                       { $$ = new CaseExpression($1, $3); }
                                                                     ;

goto_stmt : GOTO  INTEGER                                            { $$ = new GotoStatement($2); }
                                                                     ;

%%

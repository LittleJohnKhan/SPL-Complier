%{
#include "ast.h"
#include "parser.hpp"
#include <stdio.h>
#include <string>
extern "C" int yywrap() {return 1;}
%}

%%
[ \t\n]                                                 { ; }
\'.\'                                                   { 
                                                            yylval.cVal = yytext[1];
                                                            return CHAR; 
                                                        }
"("                                                     { return LP; }
")"                                                     { return RP; }
"["                                                     { return LB; }
"]"                                                     { return RB; }
".."                                                    { return DOTDOT; }
"."                                                     { return DOT; }
","                                                     { return COMMA; }
":"                                                     { return COLON; }
"*"                                                     { return MUL; }
"/"                                                     { return DIV; }
"<>"                                                    { return UNEQUAL; }
"not"                                                   { return NOT; }
"+"                                                     { return PLUS; }
"-"                                                     { return MINUS; }
">="                                                    { return GE; }
">"                                                     { return GT; }
"<="                                                    { return LE; }
"<"                                                     { return LT; }
"="                                                     { return EQUAL; }
":="                                                    { return ASSIGN; }
"mod"                                                   { return MOD; }
";"                                                     { return SEMI; }
"and"                                                   { return AND; }
"array"                                                 { return ARRAY; }
"begin"                                                 { return TOKEN_BEGIN; }
"case"                                                  { return CASE; }
"const"                                                 { return CONST; }
"div"                                                   { return DIV; }
"do"                                                    { return DO; }
"downto"                                                { return DOWNTO; }
"else"                                                  { return ELSE; }
"end"                                                   { return END; }
"for"                                                   { return FOR; }
"function"                                              { return FUNCTION; }
"goto"                                                  { return GOTO; }
"if"                                                    { return IF; }
"of"                                                    { return OF; }
"or"                                                    { return OR; }
"procedure"                                             { return PROCEDURE; }
"program"                                               { return PROGRAM; }
"record"                                                { return RECORD; }
"repeat"                                                { return REPEAT; }
"then"                                                  { return THEN; }
"to"                                                    { return TO; }
"type"                                                  { return TYPE; }
"until"                                                 { return UNTIL; }
"var"                                                   { return VAR; }
"while"                                                 { return WHILE; }
"boolean"|"char"|"integer"|"real"                       {
                                                            yylval.sVal = new std::string(yytext, yyleng); 
                                                            return SYS_TYPE; 
                                                        }
"abs"|"chr"|"odd"|"ord"|"pred"|"sqr"|"sqrt"|"succ"      {
                                                            yylval.sVal = new std::string(yytext, yyleng); 
                                                            return SYS_FUNCT; 
                                                        }
"false"|"maxint"|"true"                                 {
                                                            yylval.sVal = new std::string(yytext, yyleng); 
                                                            return SYS_CON; 
                                                        }
"write"|"writeln"                                       { 
                                                            yylval.sVal = new std::string(yytext, yyleng);
                                                            return SYS_PROC; 
                                                        }
"read"                                                  { 
                                                            yylval.sVal = new std::string(yytext, yyleng);
                                                            return READ; 
                                                        }

[a-zA-Z_][a-zA-Z0-9_]*                                  { 
                                                            yylval.sVal = new std::string(yytext, yyleng);
                                                            return IDENTIFIER; 
                                                        } 
[0-9]+\.[0-9]+                                          { 
                                                            double dtmp;
                                                            sscanf(yytext, "%lf", &dtmp);
                                                            yylval.dVal = dtmp;
                                                            return REAL; 
                                                        }
[0-9]+                                                  {
                                                            int itmp;                                                           double tmp;
                                                            sscanf(yytext, "%d", &itmp);
                                                            yylval.iVal = itmp; 
                                                            return INTEGER; 
                                                        }



%%
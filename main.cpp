#include "ast.h"

extern int yyparse();
extern Program *program;

int main(int argc, char **argv) {
    yyparse();
    return 0;
}
#include "ast.h"
#include <fstream>

using namespace std;

extern int yyparse();
extern Program *root;

int main(int argc, char **argv) {
    yyparse();
    ofstream os("tree.json");
    os << root->getJson() << endl;
    return 0;
}
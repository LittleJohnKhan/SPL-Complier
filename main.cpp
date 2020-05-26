#include <fstream>
#include "CodeGenerator.h"
#include "parser.hpp"
#include "parser.hpp"
extern int yyparse();
extern Program *root;

int main(int argc, char **argv) {
    yyparse();
    std::ofstream os("tree.json");
    os << root->getJson() << std::endl;
    
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    CodeGenerator generator;
    generator.generate(*root);
//    generator.run();
    
    return 0;
}

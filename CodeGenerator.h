#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <memory>

using namespace llvm;

class CodeGenerator 
{
public:
    CodeGenerator() : Builder(TheContext) {
        
    }
private:
    LLVMContext TheContext;
    IRBuilder<> Builder;
    std::unique_ptr<Module> TheModule;
    std::map<std::string, Value *> NamedValues;
};

#endif
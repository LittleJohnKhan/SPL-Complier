#include "ast.h"
#include "CodeGenerator.h"

using namespace std;

void CodeGenerator::generate(Program& astRoot)
{
    cout << "[INFO]" << "IR generation begin..." << endl;
    astRoot.codeGen(*this);
    cout << "[INFO]" << "IR generation finished." << endl;
    
//    llvm::legacy::PassManager pm;
//    pm.add(createPrintModulePass(llvm::outs()));
//    pm.run(*TheModule);
    // TheModule->dump();
    TheModule->print(llvm::errs(), nullptr);
}

llvm::GenericValue CodeGenerator::run()
{
    cout << "[INFO]" << "IR running begin..." << endl;
    llvm::ExecutionEngine* ee = genExeEngine();
    vector<llvm::GenericValue> args;
    llvm::GenericValue res = ee->runFunction(mainFunction, args);
    cout << "[INFO]" << "IR running finished." << endl;
    return res;
}

llvm::ExecutionEngine* CodeGenerator::genExeEngine()
{
    std::string errStr;
    auto RTDyldMM = unique_ptr<llvm::SectionMemoryManager>(new llvm::SectionMemoryManager());
    llvm::ExecutionEngine* ee = llvm::EngineBuilder(std::move(TheModule))
//        .setEngineKind(llvm::EngineKind::Interpreter)
        .setEngineKind(llvm::EngineKind::JIT)
        .setErrorStr(&errStr)
        .setVerifyModules(true)
        .setMCJITMemoryManager(move(RTDyldMM))
        .setOptLevel(llvm::CodeGenOpt::Default)
        .create();
    if (!ee)
    {
        throw std::logic_error("[ERROR]Create Engine Error: " + errStr);
    }
    ee->addModule(std::move(TheModule));
    ee->finalizeObject();
    return ee;
}

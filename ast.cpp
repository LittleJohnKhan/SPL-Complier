#include "ast.h"
#include "CodeGenerator.h"

using namespace std;

void LOG_I(const string & msg)
{
    //cout << "[INFO]" << msg << endl;
}

void printType(llvm::Value* v)
{
    if (v->getType()->isIntegerTy())
    {
        cout << "int " << v->getType()->getIntegerBitWidth() << endl;
    }
    else if (v->getType()->isFloatTy())
    {
        cout << "float" << endl;
    }
    else if (v->getType()->isDoubleTy())
    {
        cout << "double" << endl;
    }
    else
    {
        cout << "unknown" << endl;
    }
}

template <typename T>
void printT(T* vt)
{
    string str;
    llvm::raw_string_ostream stream(str);
    vt->print(stream);
    cout << str << endl;
}

llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, llvm::StringRef VarName, llvm::Type* type)
{
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(type, nullptr, VarName);
}

llvm::Type* toLLVMPtrType(const BuildInType & type)
{
    switch (type)
    {
        case SPL_INTEGER: return llvm::Type::getInt32PtrTy(TheContext);
        case SPL_REAL: return llvm::Type::getDoublePtrTy(TheContext);
        case SPL_CHAR: return llvm::Type::getInt8PtrTy(TheContext);
        case SPL_BOOLEAN: return llvm::Type::getInt1PtrTy(TheContext);
        default: throw logic_error("Not supported pointer type.");
    }
}

llvm::Type* AstType::toLLVMType()
{
    switch (this->type)
    {
        case SPL_ARRAY:
            if (this->arrayType->range->type == SPL_CONST_RANGE)
            {
                return llvm::ArrayType::get(this->arrayType->type->toLLVMType(), this->arrayType->range->constRangeType->size());
            }
            else
            {
                return llvm::ArrayType::get(this->arrayType->type->toLLVMType(), this->arrayType->range->enumRangeType->size());
            }
        case SPL_CONST_RANGE: return TheBuilder.getInt32Ty();
        case SPL_ENUM_RANGE: return TheBuilder.getInt32Ty();
        case SPL_BUILD_IN:
            switch (buildInType)
            {
                case SPL_INTEGER: return TheBuilder.getInt32Ty();
                case SPL_REAL: return TheBuilder.getDoubleTy();
                case SPL_CHAR: return TheBuilder.getInt8Ty();
                case SPL_BOOLEAN: return TheBuilder.getInt1Ty();
            }
            break;
        case SPL_ENUM:
        case SPL_RECORD:
        case SPL_USER_DEFINE:
        case SPL_VOID: return TheBuilder.getVoidTy();
    }
}

llvm::Constant* AstType::initValue(ConstValue *v)
{
    vector<llvm::Constant*> element;
    llvm::ArrayType* arrayType;;
    size_t size = 0;
    if (v != nullptr)
    {
        switch (this->type)
        {
            case SPL_ARRAY:
                if (this->arrayType->range->type == SPL_CONST_RANGE)
                {
                    size = this->arrayType->range->constRangeType->size();
                }
                else
                {
                    size = this->arrayType->range->enumRangeType->size();
                }
                for (int i = 0; i < size; i++)
                {
                    element.push_back(this->arrayType->type->initValue(v));
                }
                arrayType = (llvm::ArrayType*)this->toLLVMType();
                return llvm::ConstantArray::get(arrayType, element);
            case SPL_CONST_RANGE:
            case SPL_ENUM_RANGE: return TheBuilder.getInt32(v->getValue().i);
            case SPL_BUILD_IN:
                switch (buildInType)
                {
                    case SPL_INTEGER: return TheBuilder.getInt32(v->getValue().i);
                    case SPL_REAL: return llvm::ConstantFP::get(TheBuilder.getDoubleTy(), v->getValue().r);
                    case SPL_CHAR: return TheBuilder.getInt8(v->getValue().c);
                    case SPL_BOOLEAN: return TheBuilder.getInt1(v->getValue().b);
                }
                break;
            case SPL_ENUM:
            case SPL_RECORD:
            case SPL_USER_DEFINE:
            case SPL_VOID: return nullptr;
        }
    }
    else
    {
        switch (this->type)
        {
            case SPL_ARRAY:
                if (this->arrayType->range->type == SPL_CONST_RANGE)
                {
                    size = this->arrayType->range->constRangeType->size();
                }
                else
                {
                    size = this->arrayType->range->enumRangeType->size();
                }
                for (int i = 0; i < size; i++)
                {
                    element.push_back(this->arrayType->type->initValue());
                }
                arrayType = (llvm::ArrayType*)this->toLLVMType();
                return llvm::ConstantArray::get(arrayType, element);
            case SPL_CONST_RANGE:
            case SPL_ENUM_RANGE: return TheBuilder.getInt32(0);
            case SPL_BUILD_IN:
                switch (buildInType)
                {
                    case SPL_INTEGER: return TheBuilder.getInt32(0);
                    case SPL_REAL: return llvm::ConstantFP::get(TheBuilder.getDoubleTy(), 0.0);
                    case SPL_CHAR: return TheBuilder.getInt8(0);
                    case SPL_BOOLEAN: return TheBuilder.getInt1(0);
                }
                break;
            case SPL_ENUM:
            case SPL_RECORD:
            case SPL_USER_DEFINE:
            case SPL_VOID: return nullptr;
        }
    }
}

llvm::Value *BinaryOp(llvm::Value *lValue, BinaryExpression::BinaryOperator op, llvm::Value *rValue)
{
//        printType(lValue);
//        printType(rValue);
    bool flag = lValue->getType()->isDoubleTy() || rValue->getType()->isDoubleTy();
    switch (op)
    {
        case BinaryExpression::SPL_PLUS: return flag ? TheBuilder.CreateFAdd(lValue, rValue, "addtmpf") : TheBuilder.CreateAdd(lValue, rValue, "addtmpi");

        case BinaryExpression::SPL_MINUS: return flag ? TheBuilder.CreateFSub(lValue, rValue, "subtmpf") : TheBuilder.CreateSub(lValue, rValue, "subtmpi");

        case BinaryExpression::SPL_MUL: return flag ? TheBuilder.CreateFMul(lValue, rValue, "multmpf") : TheBuilder.CreateMul(lValue, rValue, "multmpi");

        case BinaryExpression::SPL_DIV: return TheBuilder.CreateSDiv(lValue, rValue, "tmpDiv");

        case BinaryExpression::SPL_GE: return TheBuilder.CreateICmpSGE(lValue, rValue, "tmpSGE");

        case BinaryExpression::SPL_GT: return TheBuilder.CreateICmpSGT(lValue, rValue, "tmpSGT");

        case BinaryExpression::SPL_LT: return TheBuilder.CreateICmpSLT(lValue, rValue, "tmpSLT");

        case BinaryExpression::SPL_LE: return TheBuilder.CreateICmpSLE(lValue, rValue, "tmpSLE");

        case BinaryExpression::SPL_EQUAL: return TheBuilder.CreateICmpEQ(lValue, rValue, "tmpEQ");

        case BinaryExpression::SPL_UNEQUAL: return TheBuilder.CreateICmpNE(lValue, rValue, "tmpNE");

        case BinaryExpression::SPL_OR: return TheBuilder.CreateOr(lValue, rValue, "tmpOR");

        case BinaryExpression::SPL_MOD: return TheBuilder.CreateSRem(lValue, rValue, "tmpSREM");

        case BinaryExpression::SPL_AND: return TheBuilder.CreateAnd(lValue, rValue, "tmpAND");

        case BinaryExpression::SPL_XOR: return TheBuilder.CreateXor(lValue, rValue, "tmpXOR");
    }
}

llvm::Value *Identifier::codeGen(CodeGenerator & generator) {
    LOG_I("Idnetifier");
//    return TheBuilder.CreateLoad(generator.findValue(*(this->name)), *(this->name));
    return new llvm::LoadInst(generator.findValue(*(this->name)), "tmp", false, TheBuilder.GetInsertBlock());
}

llvm::Value *Integer::codeGen(CodeGenerator & generator) {
    LOG_I("Integer");
    return TheBuilder.getInt32(this->value);
}

llvm::Value *Char::codeGen(CodeGenerator & generator) {
    LOG_I("Char");
    return TheBuilder.getInt8(this->value);
}

llvm::Value *Real::codeGen(CodeGenerator & generator) {
    LOG_I("Real");
//    return llvm::ConstantFP::get(TheContext, llvm::APFloat(this->value));
    return llvm::ConstantFP::get(TheBuilder.getDoubleTy(), this->value);
}

llvm::Value *Boolean::codeGen(CodeGenerator & generator) {
    LOG_I("Boolean");
    return TheBuilder.getInt1(this->value);
}

llvm::Value *ConstDeclaration::codeGen(CodeGenerator & generator) {
    LOG_I("Const Declaration");
    string name = this->name->getName();
    this->type = new AstType(this->value->getType());
    if (this->isGlobal())
    {
         return new llvm::GlobalVariable(*generator.TheModule, this->type->toLLVMType(), true, llvm::GlobalValue::ExternalLinkage, this->type->initValue(this->value), name);
    }
    else
    {
        auto alloc = CreateEntryBlockAlloca(generator.getCurFunction(), name, this->type->toLLVMType());
        return TheBuilder.CreateStore(this->value->codeGen(generator), alloc);
    }
}

llvm::Value *TypeDeclaration::codeGen(CodeGenerator & generator) {
    LOG_I("Type Declaration");
    return nullptr;
}

llvm::Value *AstType::codeGen(CodeGenerator & generator) {
    LOG_I("Type");
    switch (this->type)
    {
        case SPL_ARRAY: this->arrayType->codeGen(generator); break;
        case SPL_CONST_RANGE: this->constRangeType->codeGen(generator); break;
        case SPL_ENUM_RANGE: this->enumRangeType->codeGen(generator); break;
        case SPL_RECORD: this->recordType->codeGen(generator); break;
        case SPL_ENUM: this->enumType->codeGen(generator); break;
        case SPL_USER_DEFINE: this->userDefineType->codeGen(generator); break;
        case SPL_BUILD_IN: break;
        case SPL_VOID: break;
    }
    return (llvm::Value*)this->toLLVMType();
}

llvm::Value *EnumType::codeGen(CodeGenerator & generator) {
    LOG_I("Enum Type");
    //TODO
    return nullptr;
}

llvm::Value *AstArrayType::codeGen(CodeGenerator & generator) {
    LOG_I("Array Type");
    return this->range->codeGen(generator);
}

llvm::Value *RecordType::codeGen(CodeGenerator & generator) {
    LOG_I("Record Type");
    return nullptr;
}

llvm::Value *ConstRangeType::mapIndex(llvm::Value *indexValue, CodeGenerator & generator)
{
    return BinaryOp(indexValue, BinaryExpression::SPL_MINUS, this->lowBound->codeGen(generator));
}

llvm::Value *ConstRangeType::codeGen(CodeGenerator & generator) {
    LOG_I("Const Range Type");
    this->size();
    return nullptr;
}

llvm::Value *EnumRangeType::mapIndex(llvm::Value *indexValue, CodeGenerator & generator)
{
    return BinaryOp(indexValue, BinaryExpression::SPL_MINUS, this->lowValue);
}

int64_t getActualValue(llvm::Value *v)
{
    llvm::Constant *constValue = llvm::cast<llvm::GlobalVariable>(v)->getInitializer();
    llvm::ConstantInt *constInt = llvm::cast<llvm::ConstantInt>(constValue);
    return constInt->getSExtValue();
}

size_t EnumRangeType::size()
{
    int64_t low = 1, up = 0;
    if (this->lowValue->getType() == this->upValue->getType())
    {
        low = getActualValue(this->lowValueAddr);
        up = getActualValue(this->upValueAddr);
//        cout << "low: " << low << " up: " << up << endl;
        if (low > up)
        {
            throw std::range_error("[ERROR]low > up.");
        }
    }
    else
    {
        throw std::domain_error("[ERROR]Invalid range type.");
    }
    return up - low + 1;
}

llvm::Value *EnumRangeType::codeGen(CodeGenerator & generator) {
    LOG_I("Enum Range Type");
    this->upValue = this->upBound->codeGen(generator);
    this->lowValue = this->lowBound->codeGen(generator);
    this->upValueAddr = generator.findValue(this->upBound->getName());
    this->lowValueAddr = generator.findValue(this->lowBound->getName());
    this->size();
    return nullptr;
}

llvm::Value *FieldDeclaration::codeGen(CodeGenerator & generator) {
    LOG_I("Field Declaration");
    return nullptr;
}

llvm::Value *VarDeclaration::codeGen(CodeGenerator & generator) {
    LOG_I("Var Declaration");
    llvm::Value* alloc = nullptr;
    llvm::Type* varType;
    for (auto & id : *(this->nameList))
    {
        if (this->type->type == AstType::SPL_ARRAY)
        {
            generator.arrayMap[id->getName()] = this->type->arrayType;
        }
        varType = (llvm::Type*)(this->type->codeGen(generator));
        if (this->isGlobal())
        {
            alloc = new llvm::GlobalVariable(*generator.TheModule, varType, false, llvm::GlobalValue::ExternalLinkage, this->type->initValue(), id->getName());
        }
        else
        {
            alloc = CreateEntryBlockAlloca(generator.getCurFunction(), id->getName(), varType);
        }
    }
    return alloc;
}

llvm::Value *FuncDeclaration::codeGen(CodeGenerator & generator) {
    LOG_I("Function Declaration");
    //Prototype
    vector<llvm::Type*> argTypes;
    for (auto & argType : *(this->paraList))
    {
        if (argType->isVar)
        {
            argTypes.insert(argTypes.end(), argType->nameList->size(), toLLVMPtrType(argType->getType()->buildInType));
        }
        else
        {
            argTypes.insert(argTypes.end(), argType->nameList->size(), argType->getType()->toLLVMType());
        }
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(this->returnType->toLLVMType(), argTypes, false);
    llvm::Function *function = llvm::Function::Create(funcType, llvm::GlobalValue::InternalLinkage, this->name->getName(), generator.TheModule.get());
    generator.pushFunction(function);
    
    //Block
    llvm::BasicBlock *newBlock = llvm::BasicBlock::Create(TheContext, "entrypoint", function, nullptr);
    TheBuilder.SetInsertPoint(newBlock);
    
    //Parameters
    llvm::Function::arg_iterator argIt =  function->arg_begin();
    int index = 1;
    for (auto & args : *(this->paraList))
    {
        for (auto & arg : *(args->nameList))
        {
            llvm::Value *alloc = nullptr;
            if (args->isVar)
            {
                //Check value
//                alloc = generator.findValue(arg->getName());
                function->addAttribute(index, llvm::Attribute::NonNull);
                alloc = TheBuilder.CreateGEP(argIt++, TheBuilder.getInt32(0), arg->getName());
            }
            else
            {
                alloc = CreateEntryBlockAlloca(function, arg->getName(), args->type->toLLVMType());
                TheBuilder.CreateStore(argIt++, alloc);
            }
            index++;
        }
    }
    
    //Return
    llvm::Value *res = nullptr;
    if (this->returnType->type != AstType::SPL_VOID)
    {
        res = CreateEntryBlockAlloca(function, this->name->getName(), this->returnType->toLLVMType());
    }
    
    //Sub routine
    this->subRoutine->codeGen(generator);
    
    //Return value
    if (this->returnType->type != AstType::SPL_VOID)
    {
        auto returnInst = this->name->codeGen(generator);
        TheBuilder.CreateRet(returnInst);
    }
    else
    {
        TheBuilder.CreateRetVoid();
    }
    
    //Pop back
    generator.popFunction();
    TheBuilder.SetInsertPoint(&(generator.getCurFunction())->getBasicBlockList().back());
    return function;
}

llvm::Value *Parameter::codeGen(CodeGenerator & generator) {
    LOG_I("Parameter");
    //Not need
    return nullptr;
}

llvm::Value *Routine::codeGen(CodeGenerator & generator) {
    LOG_I("Routine");
    llvm::Value* res = nullptr;
    
    //Const declareation part
    for (auto & constDecl : *(this->constDeclList))
    {
        res = constDecl->codeGen(generator);
    }
    //Variable declareation part
    for (auto & varDecl : *(this->varDeclList))
    {
        res = varDecl->codeGen(generator);
    }
    //Type declareation part
    for (auto & typeDecl : *(this->typeDeclList))
    {
        res = typeDecl->codeGen(generator);
    }
    //Routine declareation part
    for (auto & routineDecl : *(this->routineList))
    {
        res = routineDecl->codeGen(generator);
    }
    
    //Routine body
    res = routineBody->codeGen(generator);
    return res;
}

llvm::Value *Program::codeGen(CodeGenerator & generator) {
    LOG_I("Program");
    //Main function prototype
    vector<llvm::Type*> argTypes;
    llvm::FunctionType * funcType = llvm::FunctionType::get(TheBuilder.getVoidTy(), makeArrayRef(argTypes), false);
    generator.mainFunction = llvm::Function::Create(funcType, llvm::GlobalValue::InternalLinkage, "main", generator.TheModule.get());
    llvm::BasicBlock * basicBlock = llvm::BasicBlock::Create(TheContext, "entrypoint", generator.mainFunction, 0);
    
    generator.pushFunction(generator.mainFunction);
    TheBuilder.SetInsertPoint(basicBlock);
    //Create System functions
    generator.printf = generator.createPrintf();
    generator.scanf = generator.createScanf();
    //Code generate
    this->routine->setGlobal();
    this->routine->codeGen(generator);
    TheBuilder.CreateRetVoid();
    generator.popFunction();
    
    return nullptr;
}

llvm::Value *AssignStatement::codeGen(CodeGenerator & generator) {
    LOG_I("Assign Statement");
    llvm::Value *res = nullptr;
    this->forward(generator);
    switch (this->type)
    {
        case ID_ASSIGN: res = TheBuilder.CreateStore(this->rhs->codeGen(generator), generator.findValue(this->lhs->getName())); break;
        case ARRAY_ASSIGN: res = TheBuilder.CreateStore(this->rhs->codeGen(generator), (new ArrayReference(this->lhs, this->sub))->getReference(generator)); break;
        case RECORD_ASSIGN: res = nullptr; break;
    }
    this->backward(generator);
    return res;
}

llvm::Value *BinaryExpression::codeGen(CodeGenerator & generator) {
    LOG_I("Binary Expression");
    llvm::Value* lValue = this->lhs->codeGen(generator);
    llvm::Value* rValue = this->rhs->codeGen(generator);
    return BinaryOp(lValue, this->op, rValue);
}

llvm::Value *ArrayReference::codeGen(CodeGenerator & generator) {
    LOG_I("Array Reference");
    return TheBuilder.CreateLoad(this->getReference(generator), "arrRef");
}

llvm::Value *ArrayReference::getReference(CodeGenerator & generator)
{
    string name = this->array->getName();
    llvm::Value* arrayValue = generator.findValue(name), *indexValue;
    if (generator.arrayMap[name]->range->type == AstType::SPL_CONST_RANGE)
    {
        indexValue = generator.arrayMap[name]->range->constRangeType->mapIndex(this->index->codeGen(generator), generator);
    }
    else
    {
        indexValue = generator.arrayMap[name]->range->enumRangeType->mapIndex(this->index->codeGen(generator), generator);
    }
    vector<llvm::Value*> indexList;
    indexList.push_back(TheBuilder.getInt32(0));
    indexList.push_back(indexValue);
    return TheBuilder.CreateInBoundsGEP(arrayValue, llvm::ArrayRef<llvm::Value*>(indexList));
}

llvm::Value *RecordReference::codeGen(CodeGenerator & generator) {
    LOG_I("Record Reference");
    return nullptr;
}

llvm::Value *FunctionCall::codeGen(CodeGenerator & generator) {
    LOG_I("Function Call");
    this->forward(generator);
    llvm::Function *function = generator.TheModule->getFunction(this->function->getName());
    if (function == nullptr)
    {
        throw domain_error("[ERROR] Function not defined: " + this->function->getName());
    }
    vector<llvm::Value*> args;
    llvm::Function::arg_iterator argIt =  function->arg_begin();
    for (auto & arg : *(this->args))
    {
        if (argIt->hasNonNullAttr())
        {
//            cout << "Pass a pointer" << endl;
            llvm::Value * addr = generator.findValue(dynamic_cast<Identifier*>(arg)->getName());
            args.push_back(addr);
        }
        else
        {
//            cout << "Pass a value" << endl;
            args.push_back(arg->codeGen(generator));
        }
        argIt++;
    }
    llvm::Value *res = TheBuilder.CreateCall(function, args, "calltmp");
    this->backward(generator);
    return res;
}

llvm::Value *ProcedureCall::codeGen(CodeGenerator & generator) {
    LOG_I("Procedure Call");
    this->forward(generator);
    llvm::Function *function = generator.TheModule->getFunction(this->function->getName());
    if (function == nullptr)
    {
        throw domain_error("[ERROR] Function not defined: " + this->function->getName());
    }
    vector<llvm::Value*> args;
    llvm::Function::arg_iterator argIt =  function->arg_begin();
    for (auto & arg : *(this->args))
    {
        if (argIt->hasNonNullAttr())
        {
//            cout << "Pass a pointer" << endl;
            llvm::Value * addr = generator.findValue(dynamic_cast<Identifier*>(arg)->getName());
            args.push_back(addr);
        }
        else
        {
//            cout << "Pass a value" << endl;
            args.push_back(arg->codeGen(generator));
        }
        argIt++;
    }
    llvm::Value* res = TheBuilder.CreateCall(function, args, "calltmp");
    this->backward(generator);
    return res;
}

llvm::Value *SysFunctionCall::codeGen(CodeGenerator & generator) {
    LOG_I("System Function Call");
    this->forward(generator);
    //TODO::Code gen
    this->backward(generator);
    return nullptr;
}

llvm::Value *SysProcedureCall::SysProcWrite(CodeGenerator & generator, bool isLineBreak)
{
    string formatStr = "";
    vector<llvm::Value*> params;
    for (auto & arg : *(this->args))
    {
        llvm::Value* argValue = arg->codeGen(generator);
        if (argValue->getType() == TheBuilder.getInt32Ty())
        {
            formatStr += "%d";
        }
        else if (argValue->getType() == TheBuilder.getInt8Ty())
        {
            formatStr += "%c";
        }
        else if (argValue->getType() == TheBuilder.getInt1Ty())
        {
            formatStr += "%d";
        }
        else if (argValue->getType()->isDoubleTy())
        {
            formatStr += "%lf";
        }
        else
        {
            throw logic_error("[ERROR]Invalid type to write.");
        }
        params.push_back(argValue);
    }
    if (isLineBreak)
    {
        formatStr += "\n";
    }
    auto formatConst = llvm::ConstantDataArray::getString(TheContext, formatStr.c_str());
    auto formatStrVar = new llvm::GlobalVariable(*(generator.TheModule), llvm::ArrayType::get(TheBuilder.getInt8Ty(), formatStr.size() + 1), true, llvm::GlobalValue::ExternalLinkage, formatConst, ".str");
    auto zero = llvm::Constant::getNullValue(TheBuilder.getInt32Ty());
    llvm::Constant* indices[] = {zero, zero};
    auto varRef = llvm::ConstantExpr::getGetElementPtr(formatStrVar->getType()->getElementType(), formatStrVar, indices);
//    auto varRef
    params.insert(params.begin(), varRef);
    return TheBuilder.CreateCall(generator.printf, llvm::makeArrayRef(params), "printf");
}

llvm::Value *SysProcedureCall::SysProcRead(CodeGenerator & generator)
{
    string formatStr = "";
    vector<llvm::Value*> params;
    auto arg = this->args->front();
    llvm::Value *argAddr, *argValue;
    //Just common variable
    argAddr = generator.findValue(dynamic_cast<Identifier*>(arg)->getName());
    argValue = arg->codeGen(generator);
    if (argValue->getType() == TheBuilder.getInt32Ty())
    {
        formatStr += "%d";
    }
    else if (argValue->getType() == TheBuilder.getInt8Ty())
    {
        formatStr += "%c";
    }
    else if (argValue->getType() == TheBuilder.getInt1Ty())
    {
        formatStr += "%d";
    }
    else if (argValue->getType()->isDoubleTy())
    {
        formatStr += "%lf";
    }
    else
    {
        throw logic_error("[ERROR]Invalid type to read.");
    }
    params.push_back(argAddr);
    params.insert(params.begin(), TheBuilder.CreateGlobalStringPtr(formatStr));
    return TheBuilder.CreateCall(generator.scanf, params, "scanf");
}

llvm::Value *SysProcedureCall::codeGen(CodeGenerator & generator) {
    LOG_I("System Procedure Call");
    llvm::Value *res = nullptr;
    this->forward(generator);
    switch (this->procedure) {
        case SPL_READ: res = this->SysProcRead(generator); break;
        case SPL_WRITE: res = this->SysProcWrite(generator, false); break;
        case SPL_WRITELN: res = this->SysProcWrite(generator, true); break;
        case SPL_ERROR_PROCEDURE: throw domain_error("[ERROR]Unknown System Procedure");
    }
    this->backward(generator);
    return res;
}

llvm::Value *IfStatement::codeGen(CodeGenerator & generator) {
    LOG_I("If Statement");
    this->forward(generator);
    
    llvm::Value *condValue = this->condition->codeGen(generator), *thenValue = nullptr, *elseValue = nullptr;
    condValue = TheBuilder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(TheContext), 0, true), "ifCond");

    llvm::Function *TheFunction = generator.getCurFunction();
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(TheContext, "then", TheFunction);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(TheContext, "else", TheFunction);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(TheContext, "merge", TheFunction);

    //Then
    auto branch = TheBuilder.CreateCondBr(condValue, thenBB, elseBB);
    TheBuilder.SetInsertPoint(thenBB);
    thenValue = this->thenStatement->codeGen(generator);
    TheBuilder.CreateBr(mergeBB);
    thenBB = TheBuilder.GetInsertBlock();

    //Else
//    if (TheFunction != generator.mainFunction)
//    {
//        TheFunction->getBasicBlockList().push_back(elseBB);
//    }
    TheBuilder.SetInsertPoint(elseBB);
    if (this->elseStatement != nullptr)
    {
        elseValue = this->elseStatement->codeGen(generator);
    }
    TheBuilder.CreateBr(mergeBB);
    elseBB = TheBuilder.GetInsertBlock();

    //Merge
//    if (TheFunction != generator.mainFunction)
//    {
//        TheFunction->getBasicBlockList().push_back(mergeBB);
//    }
    TheBuilder.SetInsertPoint(mergeBB);
    
    this->backward(generator);
    return branch;
}

llvm::Value *RepeatStatement::codeGen(CodeGenerator & generator) {
    LOG_I("Repeate Statement");
    this->forward(generator);
    
    llvm::Function *TheFunction = generator.getCurFunction();
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(TheContext, "cond", TheFunction);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(TheContext, "loop", TheFunction);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(TheContext, "afterLoop", TheFunction);
    
    //Loop
    TheBuilder.CreateBr(loopBB);
    TheBuilder.SetInsertPoint(loopBB);
    for (auto & stmt : *(this->repeatStatement))
    {
        stmt->codeGen(generator);
    }
    TheBuilder.CreateBr(condBB);
    loopBB = TheBuilder.GetInsertBlock();
    
    //Cond
    TheBuilder.SetInsertPoint(condBB);
    llvm::Value *condValue = this->condition->codeGen(generator);
    condValue = TheBuilder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(TheContext), 0, true), "repeateCond");
    auto branch = TheBuilder.CreateCondBr(condValue, loopBB, afterBB);
    
    //After
    TheBuilder.SetInsertPoint(afterBB);
    this->backward(generator);
    return branch;
}

llvm::Value *WhileStatement::codeGen(CodeGenerator & generator) {
    LOG_I("While Statement");
    this->forward(generator);
    llvm::Function *TheFunction = generator.getCurFunction();
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(TheContext, "cond", TheFunction);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(TheContext, "loop", TheFunction);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(TheContext, "afterLoop", TheFunction);
    
    //Cond
    TheBuilder.CreateBr(condBB);
    TheBuilder.SetInsertPoint(condBB);
    llvm::Value *condValue = this->condition->codeGen(generator);
    condValue = TheBuilder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(TheContext), 0, true), "whileCond");
    auto branch = TheBuilder.CreateCondBr(condValue, loopBB, afterBB);
    condBB = TheBuilder.GetInsertBlock();
    
    //Loop
    TheBuilder.SetInsertPoint(loopBB);
    this->stmt->codeGen(generator);
    TheBuilder.CreateBr(condBB);
    
    //After
    TheBuilder.SetInsertPoint(afterBB);
    this->backward(generator);
    return branch;
}

llvm::Value *ForStatement::codeGen(CodeGenerator & generator) {
    LOG_I("For Statement");
    this->forward(generator);
    //Init
    llvm::Function *TheFunction = generator.getCurFunction();
    llvm::Value* startValue = this->value->codeGen(generator);
    llvm::Value* endValue = this->step->codeGen(generator);
    llvm::Value *condValue = nullptr, *curValue = nullptr, *varValue = generator.findValue(this->var->getName());
    TheBuilder.CreateStore(startValue, varValue);
    
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(TheContext, "cond", TheFunction);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(TheContext, "loop", TheFunction);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(TheContext, "afterLoop", TheFunction);
    
    //Cond
    TheBuilder.CreateBr(condBB);
    TheBuilder.SetInsertPoint(condBB);
//    curValue = TheBuilder.CreateLoad(varValue, this->var->getName());
    curValue = this->var->codeGen(generator);
    if (this->isAdd)
    {
        condValue = TheBuilder.CreateICmpSLE(curValue, endValue);
    }
    else
    {
        condValue = TheBuilder.CreateICmpSGE(curValue, endValue);
    }
    condValue = TheBuilder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(TheContext), 0, true), "forCond");
    auto branch = TheBuilder.CreateCondBr(condValue, loopBB, afterBB);
    condBB = TheBuilder.GetInsertBlock();
    
    //Loop
    TheBuilder.SetInsertPoint(loopBB);
    this->stmt->codeGen(generator);
    llvm::Value *tmpValue = TheBuilder.CreateAdd(curValue, TheBuilder.getInt32(this->isAdd ? 1 : -1));
    TheBuilder.CreateStore(tmpValue, varValue);
    TheBuilder.CreateBr(condBB);
    loopBB = TheBuilder.GetInsertBlock();
    
    //After
    TheBuilder.SetInsertPoint(afterBB);
    this->backward(generator);
    return branch;
}

llvm::Value *CaseStatement::codeGen(CodeGenerator & generator) {
    LOG_I("Case Statement");
    this->forward(generator);
    
    llvm::Value *cmpValue = this->value->codeGen(generator), *condValue = nullptr;
    llvm::Function *TheFunction = generator.getCurFunction();
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(TheContext, "afterCase", TheFunction);
    vector<llvm::BasicBlock*> switchBBs, caseBBs;
    for (int i = 1; i <= this->caseExprList->size(); i++)
    {
        switchBBs.push_back(llvm::BasicBlock::Create(TheContext, "switch", TheFunction));
        caseBBs.push_back(llvm::BasicBlock::Create(TheContext, "case", TheFunction));
    }
    TheBuilder.CreateBr(switchBBs[0]);
    for (int i = 0; i < this->caseExprList->size(); i++)
    {
        //Switch
        TheBuilder.SetInsertPoint(switchBBs[i]);
        condValue = BinaryOp(cmpValue, BinaryExpression::SPL_EQUAL, (*caseExprList)[i]->value->codeGen(generator));
        if (i < this->caseExprList->size() - 1)
        {
            TheBuilder.CreateCondBr(condValue, caseBBs[i], switchBBs[i + 1]);
        }
        else
        {
            TheBuilder.CreateCondBr(condValue, caseBBs[i], afterBB);
        }
        
        //Case
        TheBuilder.SetInsertPoint(caseBBs[i]);
        (*caseExprList)[i]->codeGen(generator);
        TheBuilder.CreateBr(afterBB);
    }
    
    //After
    TheBuilder.SetInsertPoint(afterBB);
    this->backward(generator);
    return nullptr;
}

llvm::Value *CaseExpression::codeGen(CodeGenerator & generator) {
    LOG_I("Case Expression");
    return this->stmt->codeGen(generator);
}

void Statement::forward(CodeGenerator & generator)
{
    llvm::Function *TheFunction = generator.getCurFunction();
    if (this->label >= 0)
    {
        if (generator.labelBlock[this->label] == nullptr)
        {
            generator.labelBlock[this->label] = llvm::BasicBlock::Create(TheContext, "Label_" + to_string(label), TheFunction);
        }
        if (this->afterBB == nullptr)
        {
            this->afterBB = llvm::BasicBlock::Create(TheContext, "afterLabel_" + to_string(this->label), TheFunction);
        }
        TheBuilder.CreateBr(generator.labelBlock[this->label]);
        TheBuilder.SetInsertPoint(generator.labelBlock[this->label]);
    }
}

void Statement::backward(CodeGenerator & generator)
{
    if (this->label >= 0 && afterBB != nullptr)
    {
        TheBuilder.SetInsertPoint(generator.labelBlock[this->label]);
        TheBuilder.CreateBr(this->afterBB);
        TheBuilder.SetInsertPoint(this->afterBB);
    }
}

llvm::Value *GotoStatement::codeGen(CodeGenerator & generator) {
    LOG_I("Goto Statement");
    this->forward(generator);
    llvm::Value *res = nullptr;
    if (generator.labelBlock[this->toLabel] == nullptr)
    {
        generator.labelBlock[this->toLabel] = llvm::BasicBlock::Create(TheContext, "Label_" + to_string(this->toLabel), generator.getCurFunction());
    }
    res = TheBuilder.CreateBr(generator.labelBlock[this->toLabel]);
//    if (this->afterBB == nullptr)
//    {
//        this->afterBB = llvm::BasicBlock::Create(TheContext, "afterLabel_" + to_string(this->toLabel), generator.getCurFunction());
//    }
//    TheBuilder.SetInsertPoint(this->afterBB);
    this->backward(generator);
    return res;
}

llvm::Value *CompoundStatement::codeGen(CodeGenerator & generator) {
    LOG_I("CompoundStatement");
    this->forward(generator);
    llvm::Value *lastValue = nullptr;
    for (auto & stmt : *(this->stmtList))
    {
        lastValue = stmt->codeGen(generator);
    }
    this->backward(generator);
    return lastValue;
}

string getJsonString(string name) {
    return "{ \"name\" : \"" + name + "\" }";
}

string getJsonString(string name, vector<string> children) {
    string result = "{ \"name\" : \"" + name + "\", \"children\" : [ ";
    int i = 0;
    for(auto &child : children) {
        if(i != children.size() - 1)
            result += child + ", ";
        else 
            result += child + " ";
        i++;
    }
    return result + " ] }";
}

string getJsonString(string name, string value) {
    return getJsonString(name, vector<string>{value});
}

string getJsonString(string name, string value, vector<string> children) {
    string result = "{ \"name\" : \"" + name + "\", \"value\" : \"" + value + "\", \"children\" : [ ";
    int i = 0;
    for(auto &child : children) {
        if(i != children.size() - 1)
            result += child + ", ";
        else 
            result += child + " ";
        i++;
    }
    return result + " ] }";
}

string Program::getJson() {
    return getJsonString("program", *programID, vector<string>{routine->getJson()});
}

string Identifier::getJson() {
    return getJsonString("Identifier", getJsonString(*name));
}

string Integer::getJson() {
    return getJsonString("Integer", getJsonString(to_string(value)));
}

string Real::getJson() {
    return getJsonString("Real", getJsonString(to_string(value)));
}

string Char::getJson() {
    return getJsonString("Char", getJsonString(string(1, value)));
}

string Boolean::getJson() {
    return getJsonString("Boolean", getJsonString((value ? "true" : "false")));
}

string ConstDeclaration::getJson() {
    vector<string> children;
    children.push_back(name->getJson());
    children.push_back(value->getJson());
    return getJsonString("ConstDeclaration", children);
}

string EnumType::getJson() {
    vector<string> children;
    for(auto &name : *enumList) {
        children.push_back(name->getJson());
    }
    return getJsonString("EnumType", children);
}

string AstArrayType::getJson() {
    vector<string> children;
    children.push_back(getJsonString("range", range->getJson()));
    children.push_back(getJsonString("type", type->getJson()));
    return getJsonString("AstArrayType", children);
}

string ConstRangeType::getJson() {
    vector<string> children;
    children.push_back(getJsonString("lowBound", lowBound->getJson()));
    children.push_back(getJsonString("upBound", upBound->getJson()));
    return getJsonString("ConstRangeType", children);
}

string EnumRangeType::getJson() {
    vector<string> children;
    children.push_back(getJsonString("lowBound", lowBound->getJson()));
    children.push_back(getJsonString("upBound", upBound->getJson()));
    return getJsonString("EnumRangeType", children);
}

string FieldDeclaration::getJson() {
    vector<string> children;
    vector<string> nameListJson;
    for(auto &name : *nameList) {
        nameListJson.push_back(name->getJson());
    }
    children.push_back(getJsonString("nameList", nameListJson));
    children.push_back(getJsonString("type", type->getJson()));
    return getJsonString("FieldDeclaration", children);
}

string RecordType::getJson() {
    vector<string> children;
    for(auto &field : *fieldList) {
        children.push_back(field->getJson());
    }

    return getJsonString("RecordType", children);
}

string AstType::getJson() {
    switch (type)
    {
    case SPL_ARRAY:
        return arrayType->getJson();
        break;
    case SPL_RECORD:
        return recordType->getJson();
        break;
    case SPL_ENUM:
        return enumType->getJson();
        break;
    case SPL_CONST_RANGE:
        return constRangeType->getJson();
        break;
    case SPL_ENUM_RANGE:
        return enumRangeType->getJson();
        break;
    case SPL_BUILD_IN:
        switch (buildInType)
        {
        case SPL_INTEGER:
            return getJsonString("BuildInType", getJsonString("Integer"));
            break;
        case SPL_REAL:
            return getJsonString("BuildInType", getJsonString("Real"));
            break;
        case SPL_CHAR:
            return getJsonString("BuildInType", getJsonString("Char"));
            break;
        case SPL_BOOLEAN:
            return getJsonString("BuildInType", getJsonString("Boolean"));
            break;
        default:
            break;
        }
        break;
    case SPL_USER_DEFINE:
        return getJsonString("UserDefineType", getJsonString(userDefineType->getName()));
        break;
    default:
        return getJsonString("ErrorType");
        break;
    }
}

string TypeDeclaration::getJson() {
    vector<string> children;
    children.push_back(name->getJson());
    children.push_back(type->getJson());
    return getJsonString("TypeDeclaration", children);
}

string VarDeclaration::getJson() {
    vector<string> children;
    vector<string> nameListJson;
    for(auto &name : *nameList) {
        nameListJson.push_back(name->getJson());
    }
    children.push_back(getJsonString("Type", type->getJson()));
    children.push_back(getJsonString("NameList", nameListJson));
    return getJsonString("VarDeclaration", children);
}

string FuncDeclaration::getJson() {
    vector<string> children;
    children.push_back(name->getJson());
    vector<string> paraListJson;
    for(auto &para : *paraList)
        paraListJson.push_back(para->getJson());
    children.push_back(getJsonString("ParaList", paraListJson));
    children.push_back(returnType->getJson());
    children.push_back(subRoutine->getJson());

    return getJsonString("FuncDeclaration", children);
}

string Parameter::getJson() {
    vector<string> children;
    vector<string> nameListJson;
    for(auto &name : *nameList) {
        nameListJson.push_back(name->getJson());
    }
    children.push_back(getJsonString("Type", type->getJson()));
    children.push_back(getJsonString("NameList", nameListJson));
    children.push_back(getJsonString("isVar", getJsonString((isVar ? "true" : "false" ))));
    return getJsonString("Parameter", children);
}

string Routine::getJson() {
    vector<string> children;
    vector<string> constJson, varJson, typeJson, routineJson;
    for(auto constDecl : *constDeclList)
        constJson.push_back(constDecl->getJson());
    for(auto varDecl : *varDeclList)
        varJson.push_back(varDecl->getJson());
    for(auto typeDecl : *typeDeclList)
        typeJson.push_back(typeDecl->getJson());
    for(auto routineDecl : *routineList)
        routineJson.push_back(routineDecl->getJson());
    children.push_back(getJsonString("ConstDeclList", constJson));
    children.push_back(getJsonString("VarDeclList", varJson)); 
    children.push_back(getJsonString("TypeDeclList", typeJson));
    children.push_back(getJsonString("RoutineDeclList", routineJson));
    children.push_back(routineBody->getJson());
    return getJsonString("Routine", children);
}

string AssignStatement::getJson() {
    vector<string> children;
    switch (type)
    {
    case ID_ASSIGN:
        children.push_back(lhs->getJson());
        children.push_back(rhs->getJson());
        break;
    case ARRAY_ASSIGN:
        children.push_back(lhs->getJson());
        children.push_back(sub->getJson());
        children.push_back(rhs->getJson());
        break;
    case RECORD_ASSIGN:
        children.push_back(lhs->getJson());
        children.push_back(field->getJson());
        children.push_back(rhs->getJson());
        break;
    default:
        break;
    }    

    return getJsonString("AssignStatement", children);
}

string BinaryExpression::getJson() {
    vector<string> children;
    children.push_back(lhs->getJson());
    children.push_back(getJsonString(opString[op]));
    children.push_back(rhs->getJson());

    return getJsonString("BinaryExpression", children);
}

string ArrayReference::getJson() {
    vector<string> children;
    children.push_back(array->getJson());
    children.push_back(index->getJson());

    return getJsonString("ArrayReference", children);
}

string RecordReference::getJson() {
    vector<string> children;
    children.push_back(record->getJson());
    children.push_back(field->getJson());

    return getJsonString("RecordReference", children);
}

string FunctionCall::getJson() {
    vector<string> children;
    children.push_back(function->getJson());
    vector<string> argsJson;
    for(auto &arg : *args) {
        argsJson.push_back(arg->getJson());
    }
    children.push_back(getJsonString("ArgList", argsJson));
    return getJsonString("FunctionCall", children);
}

string ProcedureCall::getJson() {
    vector<string> children;
    children.push_back(function->getJson());
    vector<string> argsJson;
    for(auto &arg : *args) {
        argsJson.push_back(arg->getJson());
    }
    children.push_back(getJsonString("ArgList", argsJson));
    return getJsonString("ProcedureCall", children);
}

string SysFunctionCall::getJson() {
    vector<string> children;
    children.push_back(getJsonString("SysFunction", getJsonString(*name)));
    vector<string> argsJson;
    for(auto &arg : *args) {
        argsJson.push_back(arg->getJson());
    }
    children.push_back(getJsonString("ArgList", argsJson));
    return getJsonString("SysFunctionCall", children);
}

string SysProcedureCall::getJson() {
    vector<string> children;
    children.push_back(getJsonString("SysProcedure", getJsonString(*name)));
    vector<string> argsJson;
    for(auto &arg : *args) {
        argsJson.push_back(arg->getJson());
    }
    children.push_back(getJsonString("ArgList", argsJson));
    return getJsonString("SysProcedureCall", children);
}

string IfStatement::getJson() {
    vector<string> children;
    children.push_back(condition->getJson());
    children.push_back(thenStatement->getJson());
    if(elseStatement != nullptr)
        children.push_back(elseStatement->getJson());

    return getJsonString("IfStatement", children);
}

string RepeatStatement::getJson() {
    vector<string> children;
    children.push_back(condition->getJson());
    vector<string> repeatStmtJson;
    for(auto &stmt : *repeatStatement)
        repeatStmtJson.push_back(stmt->getJson());
    children.push_back(getJsonString("RepeatStatement", repeatStmtJson));

    return getJsonString("RepeatStatement", children);
}

string WhileStatement::getJson() {
    vector<string> children;
    children.push_back(condition->getJson());
    children.push_back(stmt->getJson());

    return getJsonString("WhileStatement", children);
}

string ForStatement::getJson() {
    vector<string> children;
    children.push_back(var->getJson());
    children.push_back(value->getJson());
    children.push_back(getJsonString("isAdd", getJsonString((isAdd ? "true" : "false"))));
    children.push_back(step->getJson());
    children.push_back(stmt->getJson());

    return getJsonString("ForStatement", children);
}  

string CaseStatement::getJson() {
    vector<string> children;
    children.push_back(value->getJson());
    vector<string> caseExprJsonList;
    for(auto &caseExpr : *caseExprList) {
        caseExprJsonList.push_back(caseExpr->getJson());
    }
    children.push_back(getJsonString("CaseExprList", caseExprJsonList));

    return getJsonString("CaseStatement", children);
}    

string CaseExpression::getJson() {
    vector<string> children;
    children.push_back(value->getJson());
    children.push_back(stmt->getJson());

    return getJsonString("CaseExpression", children);
}

string GotoStatement::getJson() {
    return getJsonString("GotoStatement", getJsonString(to_string(toLabel)));
}    

string CompoundStatement::getJson() {
    vector<string> stmtJsonList;
    for(auto &stmt : *stmtList) {
        stmtJsonList.push_back(stmt->getJson());
    }
    return getJsonString("CompoundStatement", stmtJsonList);
}

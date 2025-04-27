#include"../Parser/Parser.h"
#include"../Semantic/Scope/Scope.h"
#include"../Semantic/SymbolTable/SymbolTable.h"
#include"CodeGenerator_Base.h"


typedef struct Register
{
    // the name of the register, r11, rax, etc
    char* registerName;

    // is this register is currently being use?
    int inuse;
}Register;


typedef struct RegisterPool
{
    // pointer into  register
    Register* registers[GLOBAL_USE_REGISTERS_NUM];

    // the number of registers
    int number_of_registers;
}RegisterPool;


// Function declarations
void CodeGenerator_InititeRegistersPool();
Register* allocate_register();
void free_register(Register* reg);
char* CodeGenerator_Create_Label();
int CodeGenerator_GetTypeSize(TreeNode* AST);
int CodeGenerator_GetNumberOfVariable(Scope* scope);
Symbol_Table_Function_Entry* CodeGenerator_FetchFunctionEntry(SymbolTable* globalSymbolTable, char* func_name);
Symbol_Table_Variable_Entry* CodeGenerator_FetchVariableEntry(Scope* scope, char* var_name);
void CodeGenerator_Generate(TreeNode* AST, Scope* scope, FILE* dest_file);
void CodeGenerator_GenerateProgram(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateFunctions();
void CodeGenerator_GenerateFunction(TreeNode* fnNode, Scope* globalScope);
void CodeGenerator_GenerateBody(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateStatements(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateStatement(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateDeclaration(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateAssignment(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateExpression(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateLogicalOr(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateLogicalAnd(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateRelationalTerm(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateAdditiveTerm(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateMultiplicativeTerm(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateFactor(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateFunctionCall(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateLiteral(TreeNode* AST, Scope* scope);
Register* CodeGenerator_GenerateIdentifier(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateIf(TreeNode* AST, Scope* scope);

Register* CodeGenerator_GenerateGetchar(TreeNode* AST, Scope* scope);

void CodeGenerator_GeneratePutchar(TreeNode* AST, Scope* scope);

void CodeGenerator_GenerateReturn(TreeNode* AST, Scope* scope);

void CodeGenerator_GenerateWhile(TreeNode* AST, Scope* scope);
void CodeGenerator_GenerateFunctionCall_SetParamsRegisters(TreeNode* AST, Scope* scope);
void CodeGenerator_PrintHeader();
void CodeGenerator_write_asm_functions_to_file();
char* CodeGenerator_GetParameterRegister(int index);


void CodeGenerator_PushParamRegis();
void CodeGenerator_PopParamRegis();

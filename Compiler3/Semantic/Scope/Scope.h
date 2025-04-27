#pragma once
#include"../SymbolTable/SymbolTable.h"
#include"../../Parser/Parser.h"


typedef struct Scope
{
    // pointer into the current scope symbol table
    SymbolTable* symbol_table;

    // number of children
    int number_of_childrens;

    // pointer to childrens
    struct Scope** childrens;

    // pointer to scope's parent
    struct Scope* parent;

    // this is a pointer to prototype of the current function
    Symbol_Table_Function_Entry* functionPrototype;

    // count the number of declared variables(not matter if char or int)
    int variables_c;

}Scope;


void set_scope_function_prototype(Scope* scope, Symbol_Table_Function_Entry* func_proto);
Symbol_Table_Function_Entry* get_scope_function_prototype(Scope* scope);

Function_Parameter* create_Functionn_Params(TreeNode* AST, int max, int* more_them_max);
Function_Parameter* create_Functionn_Call_Args(TreeNode* AST);

Scope* create_global_scope();
Scope* create_local_scope();

char* extract_variable_name(TreeNode* node);
Variable_And_Return_Type extract_variable_type(TreeNode* node);
Variable_And_Return_Type extract_function_return_type(TreeNode* AST);

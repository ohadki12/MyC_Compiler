#include"../SymbolTable/SymbolTable.h"
#include"../../Parser/Parser.h"
#include"Scope.h"
#include"../../ErrorReporting/ErrorReporting.h"


// Function declarations
void process_program(TreeNode* AST, Scope** scope);
void process_functions(TreeNode* AST, Scope* scope);
void process_function_declaration(TreeNode* AST, Scope* global_scope);
void process_body(TreeNode* AST, Scope* parentScope);
void process_statement(TreeNode* AST, Scope* scope);
void process_declaration(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_assignment(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_expression(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_logical_or(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_logical_and(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_relational_term(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_additive_term(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_multiplicative_term(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_factor(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_literal(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_variable_identifier(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type process_function_call(TreeNode* AST, Scope* currentScope);
void check_Function_Call_Arguments(TreeNode* AST, Scope* scope, Function_Parameter* cur_param, char*);
void process_if(TreeNode* AST, Scope* scope);
void process_while(TreeNode* AST, Scope* scope);
void process_else_if_or_else(TreeNode* AST, Scope* scope);
void process_putchar(TreeNode* AST, Scope* scope);
void process_getchar(TreeNode* AST, Scope* scope);
void process_return(TreeNode* AST, Scope* currentScope);
Variable_And_Return_Type variable_declaration_lookup(Scope* scope, char* var_name);

void process_statements(TreeNode* AST, Scope* scope);

void freeScopeTree(Scope* scope);
char* extract_variable_name_lookdown(TreeNode* AST);
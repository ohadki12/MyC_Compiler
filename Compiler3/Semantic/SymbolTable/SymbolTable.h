#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef enum{
    VARIABLE_AND_RETURN_TYPE_CHAR,
    VARIABLE_AND_RETURN_TYPE_INT,
    VARIABLE_AND_RETURN_TYPE_VOID
}Variable_And_Return_Type;

typedef enum {
    VARIABLE_PARAM, 
    VARIABLE_LOCAL    
} Variable_Origin;

typedef enum{
    LOCAL_SYMBOL_TABLE,
    GLOBAL_SYMBOL_TABLE
}Symbol_Table_Type;

typedef struct Function_Parameter{
    // the type of the variable expected for the corresponding function
    Variable_And_Return_Type variable_type;

    // pointer into the next parameter in the linked list
    struct Function_Parameter* next_param;

}Function_Parameter;

typedef struct Symbol_Table_Function_Entry{
    // the name of the function stored in the symbol table
    char* name;

    // the return type of the function
    Variable_And_Return_Type return_type;

    // the amount of parameters its gets
    int number_of_parameters;

    // pointer into linked list of the parameters in order
    Function_Parameter* params;

    // pointer into the next entry in the symbol table linked list
    struct Symbol_Table_Function_Entry* next_entry;

    // pointer into the previes entry in the symbol table linked list
    struct Symbol_Table_Function_Entry* prev_entry;

    // the index in the gloval scope children where the function scope in
    int function_index;

    // the name of the label which we uses for returning from the function
    char* function_return_label;

    // dynamic counter on how many variables been declared so far
    // used while processing code generation
    int declared_vars;

}Symbol_Table_Function_Entry;

typedef struct Symbol_Table_Variable_Entry{
    // the name of the variable
    char* name;

    // the type of the variable
    Variable_And_Return_Type variable_type;

    // the origin of the varible, was declared or was provided as a parameter, used in code generation
    Variable_Origin variable_origin;

    // the possititon relate to bp, used in code generation to acess the memory address of that variable
    int index_bp_relative;

    // the position of precedence of all local/params variables
    int variable_position;

    // pointer into the next entry in the symbol table linked list
    struct Symbol_Table_Variable_Entry* next_entry;

}Symbol_Table_Variable_Entry;

typedef struct SymbolTable
{
    // hold two types of symbol types entrires
    // one is for global scope only, the second for all non_global symbol table
    // there is no case where poth pointers will not be null
    Symbol_Table_Function_Entry* Function_Entry;
    Symbol_Table_Variable_Entry* Variable_Entry;
    
    // hold the number of records in the table
    int number_of_records;

    Symbol_Table_Type symbol_table_type;
}SymbolTable;


SymbolTable* create_symbol_table(Symbol_Table_Type type);
Symbol_Table_Function_Entry* add_function_record_into_global_symbol_table(SymbolTable* symbol_table, Variable_And_Return_Type return_type, char* func_name, Function_Parameter* params, int number_of_params);
void add_variable_record_into_local_symbol_table(SymbolTable* symbol_table, Variable_And_Return_Type var_type, char* var_name, Variable_Origin var_origin);
int isVariableExistsInTable(SymbolTable* table, char* var_name);
int isFunctionExistsInTable(SymbolTable* table, char* var_name);
Variable_And_Return_Type get_Varaible_Type_From_Table(SymbolTable* table, char* var_name);
void print_symbol_table(SymbolTable* table);
Function_Parameter* create_parameter(Variable_And_Return_Type type);
int count_number_of_params(Function_Parameter* param);

void freeSymbolTable(SymbolTable* symbolTable);
void freeSymbolTableFunctionEntries(Symbol_Table_Function_Entry* func_entry);
void freeSymbolTableVariableEntires(Symbol_Table_Variable_Entry* var_entry);


#include"SymbolTable.h"

Function_Parameter* create_parameter(Variable_And_Return_Type type){
    // allocating new memory for the new parameter
    Function_Parameter* newParam = (Function_Parameter*)malloc(sizeof(Function_Parameter));
    if(newParam == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }

    // setting the intiall values
    newParam->next_param = NULL;
    newParam->variable_type = type;

    // returning the new parameter
    return newParam;
}

SymbolTable* create_symbol_table(Symbol_Table_Type type){
    SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if(newTable == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }

    // setting the intial attrubutes values
    newTable->symbol_table_type = type;
    newTable->number_of_records = 0;

    newTable->Variable_Entry = NULL;  
    newTable->Function_Entry = NULL;

    return newTable;
}

Symbol_Table_Function_Entry* add_function_record_into_global_symbol_table(SymbolTable* symbol_table, Variable_And_Return_Type return_type, char* func_name, Function_Parameter* params, int number_of_params){
    // define local variables
    Symbol_Table_Function_Entry* tmp = NULL;


    // making sure that the symbol table is the global one
    if(symbol_table->symbol_table_type != GLOBAL_SYMBOL_TABLE){
        printf("COMPILER ERROR!_130\n");
        exit(EXIT_FAILURE);
    }
    // allocating  new memory for the new symbol table entry
    Symbol_Table_Function_Entry* NewEntry = (Symbol_Table_Function_Entry*)malloc(sizeof(Symbol_Table_Function_Entry));
    if(NewEntry == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }

    NewEntry->name = strdup(func_name);
    NewEntry->return_type = return_type;
    NewEntry->next_entry = NULL;
    NewEntry->params = params;
    NewEntry->number_of_parameters = number_of_params;
    NewEntry->function_return_label = NULL;
    NewEntry->declared_vars = 0;
    

    // increasing the counter of records in the gloval scope
    (symbol_table->number_of_records)++;

    // setting the function index 
    NewEntry->function_index = symbol_table->number_of_records - 1;

    // handling first entry cace
    if(symbol_table->Function_Entry == NULL){
        // setting the new function entry as the head of the linked list
        symbol_table->Function_Entry = NewEntry;
        
        // setting the privies node to be null
        NewEntry->prev_entry = NULL;

        return NewEntry;
    }

    // going to the last record in the linked list
    tmp = symbol_table->Function_Entry;
    while (tmp->next_entry != NULL) tmp = tmp->next_entry;
    
    // add the new record at the end of the list
    tmp->next_entry = NewEntry;

    // setting the privies of the new entry to be tmp
    NewEntry->prev_entry = tmp;



    // RETURNING THE NEW ENTRY
    return NewEntry;
}

void add_variable_record_into_local_symbol_table(SymbolTable* symbol_table, Variable_And_Return_Type var_type, char* var_name, Variable_Origin var_origin){
    // define loca variables
    Symbol_Table_Variable_Entry* tmp;
    int prev_vars_c = 0;
    
    // Creating new record
    Symbol_Table_Variable_Entry* NewEntry = (Symbol_Table_Variable_Entry*)malloc(sizeof(Symbol_Table_Variable_Entry));
    if(NewEntry == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }

    NewEntry->name = _strdup(var_name);  //Copy name 
    NewEntry->variable_type = var_type;
    NewEntry->variable_origin = var_origin;
    NewEntry->next_entry = NULL;
    

    // Handle first entry case
    if (symbol_table->Variable_Entry == NULL) {
        symbol_table->Variable_Entry = NewEntry;
    
        
    }
    else{
        
    

        // Traverse to last node and count the amount of variables previesly declared 
        // at this symbol table in the same way(local var/ param var)
        tmp = symbol_table->Variable_Entry;
        while (tmp->next_entry != NULL){

            // checking if the origin match the new var origin
            if(tmp->variable_origin == var_origin){
                // increase the counter
                prev_vars_c++;
            }

            // stepping the iterator
            tmp = tmp->next_entry;
        } 

        // checking last time
        if(tmp != NULL && tmp->variable_origin == var_origin){
            prev_vars_c++;
        }

        // Adding the new entry at end
        tmp->next_entry = NewEntry;
    }  
    // setting the position of the new entry
    NewEntry->variable_position = prev_vars_c;

}
int isVariableExistsInTable(SymbolTable* table, char* var_name){
    if(table == NULL){
        printf("COMPILER ERROR!_140\n");
        exit(EXIT_FAILURE);
    }
    //print_symbol_table(table);
    // checking if the name of the variable exists in this symbol table
    Symbol_Table_Variable_Entry* tmp = table->Variable_Entry;

    // looping for every entry in the current scope
    while(tmp != NULL){
        
        // checking if the current record match the name
        if(strcmp(var_name, tmp->name) == 0){
            return 1;
        }
       
        // stepping the tmp to the next node in the linked list
        tmp = tmp->next_entry;
    }

    // returning false
    return 0;
}


int isFunctionExistsInTable(SymbolTable* table, char* var_name){
    // this function checks if there is function entry in the symbol table
    if(table == NULL){
        printf("COMPILER ERROR!_150\n");
        exit(EXIT_FAILURE);
    }


    // checking if the name of the variable exists in this symbol table
    // getting the first entry
    Symbol_Table_Function_Entry* tmp = table->Function_Entry;

    // looping for every entry in the current scope
    while(tmp != NULL){
        
        // checking if the current record match the name
        if(strcmp(var_name, tmp->name) == 0){
            return 1;
        }
       
        // stepping the tmp to the next node in the linked list
        tmp = tmp->next_entry;
    }

    // returning false
    return 0;
}

Variable_And_Return_Type get_Varaible_Type_From_Table(SymbolTable* table, char* var_name){
    // this function assume that the variable do exists in the symbol table
    Symbol_Table_Variable_Entry* tmp = table->Variable_Entry;
    while(tmp != NULL){
        
        // checking if the current record match the name
        if(strcmp(var_name, tmp->name) == 0){
            
            return tmp->variable_type;
        }

        // stepping the tmp to the next node in the linked list
        tmp = tmp->next_entry;
    }

    // if in here, name isnt exists, return null
    return -1;
    //printf("COMPILER ERROR!_160\n");
    //exit(EXIT_FAILURE);
}


void print_symbol_table(SymbolTable* table) {
    if (table == NULL) {
        printf("Symbol Table is NULL\n");
        return;
    }

    printf("=== Symbol Table (%s) ===\n", 
           table->symbol_table_type == GLOBAL_SYMBOL_TABLE ? "GLOBAL" : "LOCAL");
    
    if (table->symbol_table_type == GLOBAL_SYMBOL_TABLE) {
        // Global symbol table holds only functions
        Symbol_Table_Function_Entry* func_entry = table->Function_Entry;
        while (func_entry != NULL) {
            printf("Function: %s, Return Type: %d, Parameters: %d\n", 
                   func_entry->name, func_entry->return_type, func_entry->number_of_parameters);
            
            Function_Parameter* param = func_entry->params;
            printf("  Parameters: ");
            while (param != NULL) {
                printf("%d ", param->variable_type);
                param = param->next_param;
            }
            printf("\n");

            func_entry = func_entry->next_entry;
        }
    } else {
        // Local symbol table holds only variables
        Symbol_Table_Variable_Entry* var_entry = table->Variable_Entry;
        while (var_entry != NULL) {
            printf("Variable: %s, Type: %d, position: %d\n", var_entry->name, var_entry->variable_type, var_entry->variable_position);
            var_entry = var_entry->next_entry;
        }
    }

   
}



void freeSymbolTable(SymbolTable* symbolTable) {
    if (symbolTable == NULL) return;
    
    freeSymbolTableFunctionEntries(symbolTable->Function_Entry);
    
    freeSymbolTableVariableEntires(symbolTable->Variable_Entry);
    
    free(symbolTable);
    
}

void freeSymbolTableFunctionEntries(Symbol_Table_Function_Entry* func_entry) {
    if (func_entry == NULL) return;

    freeSymbolTableFunctionEntries(func_entry->next_entry);
    
    free(func_entry->name);
    
    free(func_entry);
    
}

void freeSymbolTableVariableEntires(Symbol_Table_Variable_Entry* var_entry) {
    if (var_entry == NULL) return;
    
    freeSymbolTableVariableEntires(var_entry->next_entry);

    free(var_entry->name);
    free(var_entry);
}

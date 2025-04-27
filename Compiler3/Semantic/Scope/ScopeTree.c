#include"ScopeTree.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define global variable that tell if we are in an panic mode
int panicMode = 0;

char* Semantic_PrepareErrorMessage(int code, char* str, int index, char* str2) {
    char buffer[256];

    switch (code) {
        case 1:
            snprintf(buffer, sizeof(buffer), "variable '%s' cannot be declared with type 'void'", str);
            break;
        case 2:
            snprintf(buffer, sizeof(buffer), "redeclaration of variable name '%s' in the same scope", str);
            break;
        case 3:
            snprintf(buffer, sizeof(buffer), "function parameter '%s' cannot have type 'void'", str);
            break;
        case 4:
            snprintf(buffer, sizeof(buffer), "call to undefined function '%s'", str);
            break;
        case 5:
            snprintf(buffer, sizeof(buffer), "calling function '%s' without required arguments", str);
            break;
        case 6:
            snprintf(buffer, sizeof(buffer), "too many arguments in call to function '%s'", str);
            break;
        case 7:
            snprintf(buffer, sizeof(buffer), "too few arguments in call to function '%s'", str);
            break;
        case 8:
            snprintf(buffer, sizeof(buffer), "invalid argument type at index %d in function call '%s'", index, str);
            break;
        case 9:
            snprintf(buffer, sizeof(buffer), "trying to use an undeclared variable '%s'", str);
            break;
        case 10:
            snprintf(buffer, sizeof(buffer), "invalid return type according to function declaration");
            break;
        case 11:
            snprintf(buffer, sizeof(buffer), "function '%s' already exists", str);
            break;
        case 12:
            snprintf(buffer, sizeof(buffer), "duplicate declration of parameter name: '%s' in index: %d in function call '%s'", str, index, str2);
            break;
        case 13:
            snprintf(buffer, sizeof(buffer), "no main function detected '%s'", str);
            break;
        case 14:
            snprintf(buffer, sizeof(buffer), "trying to use function: '%s' inside expression, but function return 'void'", str);
            break;
        case 15:
            snprintf(buffer, sizeof(buffer), "function '%s' has %d parameters, but the maximun allowed is 4", str, index);
            break;
        case 16:
            snprintf(buffer, sizeof(buffer), "can't assign variable '%s' with expression evaluate type void", str);
            break;


        default:
            snprintf(buffer, sizeof(buffer), "unknown semantic error");
    }

    return _strdup(buffer); // returning the new buffer content in dynamic memory
}
void Semantic_LookForMainFunction(SymbolTable* symbolTable){
    // this function checks for the presidence of the function with the saed name 'main'.
    // define varialbes
    Symbol_Table_Function_Entry* entry;
    int mainFound = 0; //is main found?
    char* errorMsg;
   
    // setting the entry to the first entry in the symbol table
    entry = symbolTable->Function_Entry;

    // looping untill the current entry is null
    while (entry != NULL)
    {
        
        // checking for the name 'main'
        if(strcmp(entry->name, "main") == 0){
            // setting the flag to found
            mainFound = 1;
        }
        
        // stepping the node into the next entry
        entry = entry->next_entry;
    }
   

    // checking if the flag is off
    if(!mainFound){
        
        // print the base error message
        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, 0,0);

        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(13, "main", -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);

        // setting panic mode to 1
        panicMode = 1;
    }
    
}
void process_program(TreeNode* AST, Scope** scope) {

    // creating the global scope
    *scope = create_global_scope();

    // calling process functions
    process_functions(AST->childrens[0], *scope);

   
    // looking for main
    Semantic_LookForMainFunction((*scope)->symbol_table);

   
    // checking if we went into panic mode
    if(panicMode == 1){
        // freeing the scope tree
      
        freeScopeTree(*scope);
      
        *scope = NULL;
    }
}
void process_functions(TreeNode* AST, Scope*scope){
    // the current node shuld be non terminal type, checking it
    if(AST == NULL || AST->node_type != NON_TERMINAL|| AST->numberOfChildrens == 0 || AST->Kind.nonTerminalType != NON_TERMINAL_FUNCTIONS){
        printf("COMPILER ERRORL_12!\n");
        exit(EXIT_FAILURE);
    }

    if(AST->numberOfChildrens == 1){
        process_function_declaration(AST->childrens[0], scope);
    }
    if(AST->numberOfChildrens == 2){
        process_function_declaration(AST->childrens[0], scope);
        process_functions(AST->childrens[1], scope);
    }
}
void process_body(TreeNode* AST, Scope* scope) {
    // checking that the current ast node is non terminal body
    if(AST == NULL || AST->numberOfChildrens != 1 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_BODY){
        printf("COMPILER ERRORL_14!\n");
        exit(EXIT_FAILURE);
    }
    if(scope == NULL){
        printf("SEMNATIC GOT NULL SCOPE ERROR");
        exit(EXIT_FAILURE);
    }
   
    // saving the scope of the body
    AST->scope = (void*)scope;

    // calling process statemets
    process_statements(AST->childrens[0], scope);
    
}
void process_statements(TreeNode* AST, Scope* scope){
    // firstly, checking that the current node is non terminal statements
    if(AST == NULL || AST->numberOfChildrens == 0 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_STATEMENTS){
        printf("COMPILER ERRORL_13!\n");
        exit(EXIT_FAILURE);
    }

    // cheking the number of childrens
    if(AST->numberOfChildrens == 1){
        // calling process statement
        return process_statement(AST->childrens[0], scope);
    }
    if(AST->numberOfChildrens == 2){
        // calling process statement
        process_statement(AST->childrens[0], scope);

        // calling process statements
        process_statements(AST->childrens[1], scope);
    }
}
void process_statement(TreeNode* AST, Scope* scope){
    // processing statement
    if(AST == NULL || AST->numberOfChildrens != 1 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_STATEMENT){
        printf("COMPILER ERRORL_1!\n");
        exit(EXIT_FAILURE);
    }

    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_ASSIGNMENT){
        process_assignment(AST->childrens[0], scope);
        return;
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_DECLARATION){
        return process_declaration(AST->childrens[0], scope);
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_RETURN){
        return process_return(AST->childrens[0], scope);
        
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_FUNCTION_CALL){
        process_function_call(AST->childrens[0], scope);
        return;
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_IF_STATEMENT){
        return process_if(AST->childrens[0], scope);
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_WHILE_LOOP){
        return process_while(AST->childrens[0], scope);
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_EXPRESSION){
        process_expression(AST->childrens[0], scope);
        return;
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_PUTCHAR){
        return process_putchar(AST->childrens[0], scope);
    }
    if(AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_GETCHAR){
        return process_getchar(AST->childrens[0], scope);
    }
}
void process_declaration(TreeNode* AST, Scope* currentScope) {
    // declare local variable
    char* errorMsg;
    
    if (AST->numberOfChildrens != 2) {
        printf("Unexpected declaration structure!\n");
        exit(1);
    }

    char* varName = extract_variable_name(AST->childrens[1]);
    Variable_And_Return_Type varType = extract_variable_type(AST->childrens[0]);

    // checking if the code attempted to declare a variable with type void
    if(varType == VARIABLE_AND_RETURN_TYPE_VOID){
        // in this position, code attempted to declare a variable with type void

        // print the base error message
        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine);

        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(1, varName, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);

        // setting panic mode to 1
        panicMode = 1;

        //return;

        // setting the type to int for avoiding more errors relating to the void problem, since we have already reported that
        varType = VARIABLE_AND_RETURN_TYPE_INT;
    }

    // check if variable already exists in the scope's symbol table
    if(isVariableExistsInTable(currentScope->symbol_table, varName)){
        // deplicated declaration of variable in the same scope error

        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[1]->SourceCodeLine, AST->childrens[1]->CharacterInSourceCodeLine);
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(2, varName, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);
        
        // setting panic mode to 1
        panicMode = 1;

        return;
    }
    // adding the new variable into the scope's symbol table
    add_variable_record_into_local_symbol_table(currentScope->symbol_table, varType, varName, VARIABLE_LOCAL);

    // increasing the counter in the current scoope
    currentScope->variables_c++;
}
void process_if(TreeNode* AST, Scope* scope){
    // define local variables
    Scope* inner_scope;

    // making sure that we are on an if node
    if(AST == NULL || AST->numberOfChildrens == 0 || AST->node_type != NON_TERMINAL|| AST->Kind.nonTerminalType != NON_TERMINAL_IF_STATEMENT ){
        printf("COMPILER ERRORL");
        exit(EXIT_FAILURE);
    }
    // creating new scope for the inside of the if
    inner_scope = create_local_scope();

    // setting scope's parent
    inner_scope->parent = scope;

    // setting the pointer to the current function prototype
    set_scope_function_prototype(inner_scope, get_scope_function_prototype(scope));

    // increasing the size of the parent childrens array
    scope->childrens = (Scope**)realloc(scope->childrens, sizeof(Scope) * ++scope->number_of_childrens);
    if(scope->childrens == NULL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    // setting the son to be in the children array of the parent
    scope->childrens[scope->number_of_childrens - 1] = inner_scope;


    if(AST->numberOfChildrens == 2){
        // check valid expression
        process_expression(AST->childrens[0], scope);

        // handling new if body
        process_body(AST->childrens[1], inner_scope);

        return;
    }
    if(AST->numberOfChildrens == 3){
        // check valid expression
        process_expression(AST->childrens[0], scope);

        // handling new if body
        process_body(AST->childrens[1], inner_scope);

        // handling else_if_of else
        process_else_if_or_else(AST->childrens[2], scope); // not exists
    }
}
void process_while(TreeNode* AST, Scope* scope){
    // define variables
    Scope* inner_scope;
    //printf("num: %d\n", AST->numberOfChildrens);

    // checking we are in non terminal node type while
    if(AST == NULL || AST->numberOfChildrens != 2 || AST->node_type != NON_TERMINAL|| AST->Kind.nonTerminalType != NON_TERMINAL_WHILE_LOOP){
        printf("COMPILER ERRORL");
        exit(EXIT_FAILURE);
    }

    // checking valid expression
    process_expression(AST->childrens[0], scope);

    // creating new scope for the inside of the if
    inner_scope = create_local_scope();

    // setting scope's parent
    inner_scope->parent = scope;

    // setting the pointer to the current function prototype
    set_scope_function_prototype(inner_scope, get_scope_function_prototype(scope));

    // increasing the size of the parent childrens array
    scope->childrens = (Scope**)realloc(scope->childrens, sizeof(Scope) * ++scope->number_of_childrens);
    if(scope->childrens == NULL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    // setting the son to be in the children array of the parent
    scope->childrens[scope->number_of_childrens - 1] = inner_scope;

    // handling creating new body
    process_body(AST->childrens[1], inner_scope);
}

void process_else_if_or_else(TreeNode* AST, Scope* scope){
    // define varaibles
    Scope* inner_scope;

    // making sure we are in else_if_or_else non terminal
    if(AST == NULL || AST->numberOfChildrens != 2 || AST->node_type != NON_TERMINAL|| AST->Kind.nonTerminalType != NON_TERMINAL_ELSE_IF_OR_ELSE){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    if(AST->childrens[0]->node_type != TERMINAL || AST->childrens[0]->Kind.TerminalType != ELSE_TOKEN || AST->childrens[1]->node_type != NON_TERMINAL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    if(AST->childrens[1]->Kind.nonTerminalType == NON_TERMINAL_IF_STATEMENT){
        // process if in else
        return process_if(AST->childrens[1], scope);
    }
    if(AST->childrens[1]->Kind.nonTerminalType == NON_TERMINAL_BODY){
        // creating new scope for the inside of the if
        inner_scope = create_local_scope();

        // setting scope's parent
        inner_scope->parent = scope;

        // setting the pointer to the current function prototype
        set_scope_function_prototype(inner_scope, get_scope_function_prototype(scope));

        // increasing the size of the parent childrens array
        scope->childrens = (Scope**)realloc(scope->childrens, sizeof(Scope) * ++scope->number_of_childrens);
        if(scope->childrens == NULL){
            printf("COMPILER ERROR");
            exit(EXIT_FAILURE);
        }
        // setting the son to be in the children array of the parent
        scope->childrens[scope->number_of_childrens - 1] = inner_scope;

        // processing that body
        process_body(AST->childrens[1], inner_scope);
    }

}

void process_getchar(TreeNode* AST, Scope* scope){
    // define local variables
    Variable_And_Return_Type VarType;

    // firstly, making sure we are on an getchar non terminal
    if(AST == NULL || AST->numberOfChildrens != 1 || AST->node_type != NON_TERMINAL|| AST->Kind.nonTerminalType != NON_TERMINAL_GETCHAR 
    || AST->childrens[0]->node_type != TERMINAL || AST->childrens[0]->Kind.nonTerminalType != IDENTIFIER_TOKEN){
        printf("COMPILER ERRORL_9!\n");
        exit(EXIT_FAILURE);
    }

    // checking the type of the identifier
    VarType = process_variable_identifier(AST->childrens[0], scope);
    
   return;

}
void process_putchar(TreeNode* AST, Scope* scope){
    // define local variables
    Variable_And_Return_Type expressionType;

    // firstly, making sure we are on an putchar non terminal
    if(AST == NULL || AST->numberOfChildrens != 1 || AST->node_type != NON_TERMINAL|| AST->Kind.nonTerminalType != NON_TERMINAL_PUTCHAR){
        printf("COMPILER ERRORL_10!\n");
        exit(EXIT_FAILURE);
    }

    // checking valid expression
    expressionType = process_expression(AST->childrens[0], scope);
    
    return;
}

void process_function_declaration(TreeNode* AST, Scope* global_scope){
    // the function responsible for creating the new function prototype in the global scope
    // and also create new scope for the function with the params

    // define local variables
    Function_Parameter* params_head = NULL;
    char* func_name = NULL;
    Variable_And_Return_Type return_type;
    Symbol_Table_Function_Entry* newEntry;
    Scope* newScope;

    TreeNode* tmpTreeNodeParams;
    TreeNode* tmpParam;
    char* tmp_param_name;
    Variable_And_Return_Type tmp_param_type;

    int c = 0;
    int paramIndex = 0;
    char* errorMsg;
    //int errorFlag = 0;

    // make sure we are in an function node
    if(AST == NULL|| AST->node_type != NON_TERMINAL || (AST->numberOfChildrens != 4 && AST->numberOfChildrens != 3) || AST->Kind.nonTerminalType != NON_TERMINAL_FUNCTION){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    // get the function name
    if(AST->childrens[0]->node_type != NON_TERMINAL || AST->childrens[0]->Kind.nonTerminalType != NON_TERMINAL_TYPE || AST->childrens[0]->numberOfChildrens != 1 || AST->childrens[0]->childrens[0]->node_type != TERMINAL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    
    // getting the function name
    func_name = extract_variable_name(AST->childrens[1]);

    // check if the function name already exists
    if(isFunctionExistsInTable(global_scope->symbol_table, func_name)){
        // in here, there is a duplicate of function declaration

        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, tmpParam->childrens[0]->SourceCodeLine, tmpParam->childrens[0]->CharacterInSourceCodeLine); 
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(11, func_name, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);
        
        // setting panic mode to 1
        panicMode = 1;

        return;
    }
    
    // getting the function return type
    return_type = extract_function_return_type(AST->childrens[0]);
    
    // creating new local scope for that function
    newScope = create_local_scope();

    // setting the new scope parent
    newScope->parent = global_scope;
    
    // check if there is parameters for this function
    if(AST->numberOfChildrens == 4){
  
        //create the params list
        params_head = create_Functionn_Params(AST->childrens[2], 4, &c);
        
        // checking if overriden the maxmum parameters
        if(c > 4){ 
            // report an error
            
            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->SourceCodeLine, AST->CharacterInSourceCodeLine); //15
                
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(15, func_name, c, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;

            // setting the number of parameter to 4 and continue in panic mode
            c = 4;
        }
    
        //// counting the amount of the parameters in the list, if bigger them 4, it il be 4 in panic mode
        /////c = count_number_of_params(params_head);

        // now add the parameters which we have got to add to the function scope as paraneters
        tmpTreeNodeParams = AST->childrens[2];
        
        while (tmpTreeNodeParams != NULL)
        {
           
            tmpParam = tmpTreeNodeParams->childrens[0];
           
            if(tmpParam == NULL || tmpParam->numberOfChildrens != 2 || 
            tmpParam->node_type != NON_TERMINAL || tmpParam->Kind.nonTerminalType != NON_TERMINAL_PARAMETER){
                printf("COMPILER ERROR");
                exit(EXIT_FAILURE);
            }
            
            
            tmp_param_name = extract_variable_name(tmpParam->childrens[1]);
            tmp_param_type = extract_variable_type(tmpParam->childrens[0]);
            // handling the case where there is parameter with type void
            if(tmp_param_type == VARIABLE_AND_RETURN_TYPE_VOID){
                // in this position, source code contains an parameter variable with type void
                ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, tmpParam->childrens[0]->SourceCodeLine, tmpParam->childrens[0]->CharacterInSourceCodeLine); //15
                
                // gett the error details
                errorMsg = Semantic_PrepareErrorMessage(3, tmp_param_name, -1, NULL);

                // outputting the error message
                ErrorReporting_ReportError(errorMsg);

                // free the message
                free(errorMsg);

                // setting panic mode to 1
                panicMode = 1;
                
                //return;
            }
            // check the case if the parameter name already exists(in case where multiple parameters with the same name)
            if(isVariableExistsInTable(newScope->symbol_table, tmp_param_name)){
                // in this position, source code has two(or possibly more) parameters with the same name
                ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, tmpParam->childrens[1]->SourceCodeLine, tmpParam->childrens[1]->CharacterInSourceCodeLine); //15
                
                // gett the error details
                errorMsg = Semantic_PrepareErrorMessage(12, tmp_param_name, paramIndex, func_name);

                // outputting the error message
                ErrorReporting_ReportError(errorMsg);

                // free the message
                free(errorMsg);

                // setting panic mode to 1
                panicMode = 1;
                
                //return;
            }

            // adding a new record to the symbol tree
            add_variable_record_into_local_symbol_table(newScope->symbol_table, tmp_param_type, tmp_param_name, VARIABLE_PARAM);
            //printf("()");
            if(tmpTreeNodeParams->numberOfChildrens == 1 || tmpTreeNodeParams->childrens == NULL){
                // if reached the end of the tree of params
                break;
            }
            // else, continue
            tmpTreeNodeParams = tmpTreeNodeParams->childrens[1];

            // indecreasing the counter of params
            paramIndex++;

        }

    }
   
    // adding the new function record to the global scope
    newEntry = add_function_record_into_global_symbol_table(global_scope->symbol_table, return_type, func_name, params_head, c);
    

    // setting this entry to be the pointer to the function prototype in the newScope scope
    set_scope_function_prototype(newScope, newEntry);


    // setting the new scope as the children of the global 
    // increasing the size of pointer to the children which is on the global scope
    //global_scope->childrens = (Scope**)realloc(global_scope->childrens, sizeof(Scope*) *global_scope->symbol_table->number_of_records);
    global_scope->childrens = (Scope**)realloc(global_scope->childrens, sizeof(Scope*) *(++global_scope->number_of_childrens));

    if(global_scope->childrens == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }
    if(global_scope->number_of_childrens - 1 != newEntry->function_index){
        printf("need to think");
        exit(0);
    }
    
    // adding the children in the right place
    global_scope->childrens[newEntry->function_index] = newScope;
    

    // calling process body for the functions body
    if(AST->numberOfChildrens == 4){
        process_body(AST->childrens[3], newScope);
    }
    else if(AST->numberOfChildrens == 3){
        process_body(AST->childrens[2], newScope);
    }

}
Variable_And_Return_Type process_expression(TreeNode* AST, Scope* currentScope){
    // checking if current node isnt an expression, yield an error
    if(AST == NULL || AST->numberOfChildrens != 1|| AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_EXPRESSION){
        printf("COMPILER ERROR1!\n");
        exit(EXIT_FAILURE);
    }
    
    // calling handle logical or function    
    return process_logical_or(AST->childrens[0], currentScope);
}
Variable_And_Return_Type process_logical_or(TreeNode* AST, Scope* currentScope){
    // define local variables 
    Variable_And_Return_Type left;
    Variable_And_Return_Type right;
    char* errorMsg;

    // checking if current node isnt an logical or
    if(AST == NULL || AST->numberOfChildrens == 0 || AST->node_type == TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_LOGICAL_OR_TERM){
        printf("COMPILER ERROR2!\n");
        exit(EXIT_FAILURE);
    }
    // checking if this node has only one child, if yes, this node can be reduce to just logical and
    if(AST->numberOfChildrens == 1){
        process_logical_and(AST->childrens[0], currentScope);
    }
    else if(AST->numberOfChildrens == 3){
        if(AST->childrens[1]->node_type != TERMINAL || AST->childrens[1]->Kind.TerminalType != LOGICAL_OR_TOKEN){
            printf("COMPILER ERROR3!\n");
            exit(EXIT_FAILURE);
        }
        // checking both sides has the same type
        left = process_logical_or(AST->childrens[0], currentScope);
        right = process_logical_and(AST->childrens[2], currentScope);
        

        // checking to see that none are type void(cant have void inside expression)
        if(left == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;

        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
        }

        return VARIABLE_AND_RETURN_TYPE_INT;
    }

}
Variable_And_Return_Type process_logical_and(TreeNode* AST, Scope* currentScope){
    // define local variables 
    Variable_And_Return_Type left;
    Variable_And_Return_Type right;
    char* errorMsg;

    if(AST == NULL || AST->numberOfChildrens == 0 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_LOGICAL_AND_TERM){
        printf("COMPILER ERROR4!\n");
        exit(EXIT_FAILURE);
    }

    if(AST->numberOfChildrens == 1){
        return process_relational_term(AST->childrens[0], currentScope);
    }

    // checking both sides to see if they have a matching types
    if(AST->numberOfChildrens == 3){
        if(AST->childrens[1]->node_type != TERMINAL || AST->childrens[1]->Kind.TerminalType != LOGICAL_AND_TOKEN){
            printf("COMPILER ERROR5!\n");
            exit(EXIT_FAILURE);
        }
        left = process_logical_and(AST->childrens[0], currentScope);
        right = process_relational_term(AST->childrens[2], currentScope);
       
        
        // checking to see that none are type void(cant have void inside expression)
        if(left == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;

        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
        }

        return VARIABLE_AND_RETURN_TYPE_INT;
    }

}
Variable_And_Return_Type process_relational_term(TreeNode* AST, Scope* currentScope){
    // define local variables 
    Variable_And_Return_Type left;
    Variable_And_Return_Type right;
    char* errorMsg;

    if(AST ==NULL || AST->numberOfChildrens == 0 || AST->node_type == TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_RELATIONAL_TERM){
        printf("COMPILER ERROR6!\n");
        exit(EXIT_FAILURE);
    }
    if(AST->numberOfChildrens == 1){
        return process_additive_term(AST->childrens[0], currentScope);
    }
    if(AST->numberOfChildrens == 3){
        if(AST->childrens[1]->node_type != TERMINAL || (AST->childrens[1]->Kind.TerminalType != EQUAL_TO_TOKEN && AST->childrens[1]->Kind.TerminalType != NOT_EQUAL_TO_TOKEN
            && AST->childrens[1]->Kind.TerminalType != LESS_THEN_TOKEN && AST->childrens[1]->Kind.TerminalType != GREATER_THEN_TOKEN && AST->childrens[1]->Kind.TerminalType != LESS_THEN_OR_EQUAL_TO_TOKEN 
            && AST->childrens[1]->Kind.TerminalType != GREATER_THEN_OR_EQUAL_TO_TOKEN)){
            
            printf("COMPILER ERROR7!\n");
            exit(EXIT_FAILURE);
        } 
        
        // checking ig both side of the retional term are the same type
        left = process_additive_term(AST->childrens[0], currentScope);
        right = process_additive_term(AST->childrens[2], currentScope);
        
        // checking to see that none are type void(cant have void inside expression)
        if(left == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;

        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
        }
        
        return VARIABLE_AND_RETURN_TYPE_INT;

    }
}
Variable_And_Return_Type process_additive_term(TreeNode* AST, Scope* currentScope){
    // define local variables 
    Variable_And_Return_Type left;
    Variable_And_Return_Type right;
    char* errorMsg;

    if(AST ==NULL || AST->numberOfChildrens == 0 || AST->node_type == TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_ADDITIVE_TERM){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    if(AST->numberOfChildrens == 1){
        return process_multiplicative_term(AST->childrens[0], currentScope);
    }

    if(AST->numberOfChildrens == 3){
        if(AST->childrens[1]->node_type != TERMINAL || (AST->childrens[1]->Kind.TerminalType != PLUS_TOKEN && AST->childrens[1]->Kind.TerminalType != MINUS_TOKEN)){
            printf("COMPILER ERROR");
            exit(EXIT_FAILURE);
        }

        // checking if both sides of the additiive expresion are type matching
        left = process_multiplicative_term(AST->childrens[0], currentScope);
        right = process_additive_term(AST->childrens[2], currentScope);
        
        // checking to see that none are type void(cant have void inside expression)
        if(left == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;

        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
        }

        return VARIABLE_AND_RETURN_TYPE_INT;
    }

}
Variable_And_Return_Type process_multiplicative_term(TreeNode* AST, Scope* currentScope){
    // define local variables 
    Variable_And_Return_Type left;
    Variable_And_Return_Type right;
    char* errorMsg;

    if(AST ==NULL || AST->numberOfChildrens == 0 || AST->node_type == TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_MULTIPLICATIVE_TERM){
        printf("COMPILER ERROR10!\n");
        exit(EXIT_FAILURE);
    }
    
    if(AST->numberOfChildrens == 1){
        return process_factor(AST->childrens[0], currentScope);
    }
    
    if(AST->numberOfChildrens == 3){
        if(AST->childrens[1]->node_type != TERMINAL || (AST->childrens[1]->Kind.TerminalType != MULTIPLY_TOKEN && AST->childrens[1]->Kind.TerminalType != DIVIDE_TOKEN 
        && AST->childrens[1]->Kind.TerminalType != MODULO_TOKEN)){
            printf("COMPILER ERROR");
            exit(EXIT_FAILURE);
        }
        
        // checking if both sides of the additiive expresion are type matching
        left = process_factor(AST->childrens[0], currentScope);
        
        right = process_multiplicative_term(AST->childrens[2], currentScope);
        
        // checking to see that none are type void(cant have void inside expression)
        if(left == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;

        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){
            // ALLERT ERROR

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
    
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(14, extract_variable_name_lookdown(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
        }

        return VARIABLE_AND_RETURN_TYPE_INT;
    }
    
}
Variable_And_Return_Type process_factor(TreeNode* AST, Scope* currentScope){
    // define local variables
    Variable_And_Return_Type retType;
    char* errorMsg; // IN CASE OF AN ERROR

    
    if(AST == NULL || AST->node_type == TERMINAL || AST->numberOfChildrens == 0){
        printf("COMPILER ERROR12!\n");
        exit(EXIT_FAILURE);
    }
   
    // checking the amount of childres
    if(AST->numberOfChildrens == 1){
        
        // checking if it is an expression
        if(AST->childrens[0]->node_type == NON_TERMINAL && AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_EXPRESSION){
            // calling handle expression
            return process_expression(AST->childrens[0], currentScope);
        }
        
        // checking if it is an function call
        if(AST->childrens[0]->node_type == NON_TERMINAL && AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_FUNCTION_CALL){
            // calling handle expression
            
            retType = process_function_call(AST->childrens[0], currentScope);
            
            return retType;
            
        }
        
        // CHECKINg if it is an literal value
        if(AST->childrens[0]->node_type == NON_TERMINAL && AST->childrens[0]->Kind.nonTerminalType == NON_TERMINAL_LITERAL){
            // CALLING HANDLING LITERAL EXPRESSION
             return process_literal(AST->childrens[0], currentScope);  
        }
        if(AST->childrens[0]->node_type == TERMINAL && AST->childrens[0]->Kind.TerminalType == IDENTIFIER_TOKEN){
           
            return process_variable_identifier(AST->childrens[0], currentScope);
        }
    }
    // IF HAS TWO RHS
    if(AST->numberOfChildrens == 2){
        // can be - or ! in first place, after factor

        // calling process factor recursivly
        return process_factor(AST->childrens[1], currentScope);
    }
}
Variable_And_Return_Type process_function_call(TreeNode* AST, Scope* currentScope){
    // define local variables
    char* func_name;
    char* errorMsg;

    Symbol_Table_Function_Entry* tmpFunc;
    Function_Parameter* func_Params;
    Variable_And_Return_Type funcRetType;
    int funcExists = 0;

    // firstly, checking that the current ast node is fnuction call
    if(AST == NULL || (AST->numberOfChildrens != 1 && AST->numberOfChildrens != 2) || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_FUNCTION_CALL){
        printf("COMPILER ERROR!&()\n");
        exit(EXIT_FAILURE);
    }

    // check that there is a function declared with the same name
    func_name = extract_variable_name(AST->childrens[0]);

    // checking to see if it exists
    tmpFunc = currentScope->functionPrototype;

    // looping from the current node till the beginning
    while (tmpFunc)
    {

        // checking if the current function prototype is the wanted one
        if(strcmp(tmpFunc->name, func_name) == 0){
            // setting is exists to be true
            funcExists = 1;
            func_Params = tmpFunc->params;
            funcRetType = tmpFunc->return_type;
        }

        // iterating to the prev entry
        tmpFunc = tmpFunc->prev_entry;
    }
    if(!funcExists){
        // in this position, the source code called an unknown or defined later function
        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); // 10
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(4, func_name, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);

        // setting panic mode to 1
        panicMode = 1;
        
        return -1;
    }
    // checking if we have arguments
    if(AST->numberOfChildrens == 2){
        // checking that the arguments that were given matching the parameters and the types
        // calling check function call arguments

        check_Function_Call_Arguments(AST->childrens[1], currentScope, func_Params, func_name);
    }
    else if(AST->numberOfChildrens == 1 && func_Params != NULL){
        // in this position, source code called a function without providing any arguments
        //printf("\ndetected an semantic error, need to handle in error handling\ncase: 17");
        //exit(EXIT_FAILURE);
        // TODO, MAKE THIS NOT EXIT BUT TO BE IN PANIC MODE FOR THAT PHASE

        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); // 17
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(5, func_name, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);
        
        // setting panic mode to 1
        panicMode = 1;

        return -1;
    }

    // returning the return type of the called function
    return funcRetType;
    
}
void check_Function_Call_Arguments(TreeNode* AST, Scope* scope, Function_Parameter* cur_param, char* func_name){
    // define local variable
    Variable_And_Return_Type argumentType;
    char* errorMsg;
    int static index = 0;

    // firstly, making sure that the ast node is a non terminal arguments type
    if(AST == NULL || AST->node_type != NON_TERMINAL || (AST->numberOfChildrens != 2 && AST->numberOfChildrens != 1)|| AST->Kind.nonTerminalType != NON_TERMINAL_ARGUMENTS){
        printf("COMPILER ERROR!\n");
        exit(EXIT_FAILURE);
    }

    // checking if arguments dismatch parametes
    if(cur_param->next_param == NULL && AST->numberOfChildrens == 2){
        // in this position, the source code called a function with more arguments as expected
        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); // 26
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(6, func_name, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);

        // setting panic mode to 1
        panicMode = 1;

    }
    else if(AST->numberOfChildrens == 1 && cur_param->next_param != NULL){
        // in this position, the cource code called a function with less arguments as expected
        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); 
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(7, func_name, -1, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);

        // setting panic mode to 1
        panicMode = 1;

    }
    
    // processing and getting the type of the 
    argumentType = process_expression((AST->childrens[0])->childrens[0], scope);
    
    if(argumentType != cur_param->variable_type){
        // in this position, the argument in index index is type incorrect according to the function prototype
        ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, (AST->childrens[0])->childrens[0]->SourceCodeLine, (AST->childrens[0])->childrens[0]->CharacterInSourceCodeLine); 
        
        // gett the error details
        errorMsg = Semantic_PrepareErrorMessage(8, func_name, index, NULL);

        // outputting the error message
        ErrorReporting_ReportError(errorMsg);

        // free the message
        free(errorMsg);

        // setting panic mode to 1
        panicMode = 1;
        
        return;
    }
    
    // if not at the last argument, call this function recursivly, checking on the next argument
    if(AST->numberOfChildrens == 2 && cur_param->next_param != NULL){
        check_Function_Call_Arguments(AST->childrens[1], scope, cur_param->next_param, func_name);
    }
}
Variable_And_Return_Type process_variable_identifier(TreeNode* AST, Scope* currentScope){
    // define local variables
    Variable_And_Return_Type varType;
    char* errorMsg;

    
    // looking in the symbol tables for the defination of the identifier, and its type
    // if we went able to find the declaraiton, meanign the code trying to use undeclared variable, and the compiler will yield an error

    if(AST == NULL || AST->node_type != TERMINAL || AST->Kind.TerminalType != IDENTIFIER_TOKEN || AST->value == NULL){
        printf("COMPILER ERROR13!\n");
        exit(EXIT_FAILURE);
    }
    
    // looking for the type of variable, if not exists, the called function will handle the error
    varType =  variable_declaration_lookup(currentScope, (char*)(AST->value));
   
    if(varType != -1) return varType;

    // in this position, variable dont exists, report and error and entering panic mode
    ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->SourceCodeLine, AST->CharacterInSourceCodeLine);
    
    // gett the error details
    errorMsg = Semantic_PrepareErrorMessage(9, (char*)(AST->value), -1, NULL);

    // outputting the error message
    ErrorReporting_ReportError(errorMsg);

    // free the message
    free(errorMsg);

    // setting panic mode to 1
    panicMode = 1;

    return -1;
}
Variable_And_Return_Type process_literal(TreeNode* AST, Scope* currentScope){
    // cheking valid ast position
    if(AST ==NULL || AST->numberOfChildrens != 1 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_LITERAL){
        printf("COMPILER ERROR14!\n");
        exit(EXIT_FAILURE);
    }
   
    if(AST->childrens[0]->node_type != TERMINAL || (AST->childrens[0]->Kind.TerminalType != CHAR_LITERAL_TOKEN && AST->childrens[0]->Kind.TerminalType != DIGIT_TOKEN)) {
        printf("COMPILER ERROR15!\n");
        exit(EXIT_FAILURE);
    }
   
    // RETURNIGN THE TYPE THIS LITERAL HOLDS
    if(AST->childrens[0]->Kind.TerminalType == CHAR_LITERAL_TOKEN){
       
        return VARIABLE_AND_RETURN_TYPE_CHAR;
    }
    else{
        
        return VARIABLE_AND_RETURN_TYPE_INT;
    }
}
Variable_And_Return_Type process_assignment(TreeNode* AST, Scope* currentScope){
    // define local variables
    Variable_And_Return_Type left;
    Variable_And_Return_Type right;
    char* errorMsg;

    // checking valid ast position
    if(AST ==NULL || AST->numberOfChildrens != 2 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_ASSIGNMENT){
        printf("COMPILER ERROR15!\n");
        exit(EXIT_FAILURE);
    }

    // processing left and right
    left = process_variable_identifier(AST->childrens[0], currentScope);
    right = process_expression(AST->childrens[1], currentScope);
    

    // checking if the identifier is int
    if(left == VARIABLE_AND_RETURN_TYPE_INT){
        
        // checking if right is int
        if(right == VARIABLE_AND_RETURN_TYPE_INT) return right;

        // checking if right type char
        if(right == VARIABLE_AND_RETURN_TYPE_CHAR){
            // in this position the compiler will promote the unsign char into integer
            
            //todo, print a warning saying we promote the char to int
            printf("converting unsign character to be type int");

            // returning int type
            return left;
        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){

            // canot assign variable with value type void
            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); //14
            
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(16, extract_variable_name(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
            
            return VARIABLE_AND_RETURN_TYPE_INT;
        }
    }
    if(left == VARIABLE_AND_RETURN_TYPE_CHAR){
        // checking if the right side is character
        if(right == VARIABLE_AND_RETURN_TYPE_CHAR) return right;

        if(right == VARIABLE_AND_RETURN_TYPE_INT){
            return VARIABLE_AND_RETURN_TYPE_INT;
        }
        if(right == VARIABLE_AND_RETURN_TYPE_VOID){
            // canot assign variable with value type void
            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); //14
            
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(16, extract_variable_name(AST->childrens[0]), -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
            
            return VARIABLE_AND_RETURN_TYPE_CHAR;
        }
    }

}
void process_return(TreeNode* AST, Scope* currentScope){
    // define local variables
    Variable_And_Return_Type retType;
    char* errorMsg;

    // firstly, cheeck that the current node in th ast is non terminal return type
    if(AST == NULL || (AST->numberOfChildrens != 0 && AST->numberOfChildrens != 1) || AST->node_type != NON_TERMINAL|| AST->Kind.nonTerminalType != NON_TERMINAL_RETURN){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    // checking if we return an expresion
    if(AST->numberOfChildrens == 1){
        // getting the valeu we are getting from the return expression
        retType = process_expression(AST->childrens[0], currentScope);

        // checking if we returning the declared type
        if((currentScope->functionPrototype)->return_type != retType){
            // in this position, the type we are returning isnt equal the declared returned type

            ErrorReporting_ReportError_Base(ERROR_ON_SEMANTIC_PHASE, AST->childrens[0]->SourceCodeLine, AST->childrens[0]->CharacterInSourceCodeLine); //14
            
            // gett the error details
            errorMsg = Semantic_PrepareErrorMessage(10, NULL, -1, NULL);

            // outputting the error message
            ErrorReporting_ReportError(errorMsg);

            // free the message
            free(errorMsg);

            // setting panic mode to 1
            panicMode = 1;
            
            return;
        }
    }
    
}
Variable_And_Return_Type variable_declaration_lookup(Scope* scope, char* var_name){
    
    // looking for the declaration of the variable name in the current scope, or parents one
    Scope* tmp = scope;
    while(tmp->symbol_table->symbol_table_type != GLOBAL_SYMBOL_TABLE){
        
        // checking if the name exists in the current scope
        if(isVariableExistsInTable(tmp->symbol_table, var_name)){
            //printf("\nman\n");
            return get_Varaible_Type_From_Table(tmp->symbol_table, var_name);
        }
        
        // going to the parent scope
        tmp = tmp->parent;
    }

    // if we are here, we didnt found the variabel, return -1
    return -1;

}
char* extract_variable_name_lookdown(TreeNode* AST){
    // THIS FUNCTION GOES DOWN THE TREE UNTILL REACHING AN IDENTIFIER, OR FUNCTION CALL 
    // THIS FUNCTION ASSUME THAT THERE IS ONE CHILD EXACTLY EVERY ITERATION

    // GO DOWN
    while (AST->numberOfChildrens == 1)
    {
        AST = AST->childrens[0];
    }

    // CHECK THE TYPE
    if(AST->node_type == NON_TERMINAL && AST->Kind.nonTerminalType == NON_TERMINAL_FUNCTION_CALL){
        // RETURN FUNCTION NAME
        return extract_variable_name(AST->childrens[0]);
    }
    if(AST->node_type == TERMINAL && AST->Kind.TerminalType == IDENTIFIER_TOKEN){
        // IDENTIFIER TOKEN
        return extract_variable_name(AST);
    }

    // in this position, we called this function in an invalid position, compiler error
    printf("COMPILER ERROR");
    exit(EXIT_FAILURE);
}

void freeScopeTree(Scope* scope){
    
    if(scope == NULL) return;
    for(int i = 0; i < scope->number_of_childrens; i++){
        freeScopeTree(scope->childrens[i]);
    }
   
    free(scope->childrens);
   
    freeSymbolTable(scope->symbol_table);
    
    free(scope);
    
}



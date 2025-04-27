#include"Scope.h"

void set_scope_function_prototype(Scope* scope, Symbol_Table_Function_Entry* func_proto){
    // this function set the pointer to the record which there is the prototype of the function

    // setting 
    scope->functionPrototype = func_proto;
}
Symbol_Table_Function_Entry* get_scope_function_prototype(Scope* scope){
    // return the pointer to the record which there is the prototype of the function
    return scope->functionPrototype;
}

Function_Parameter* create_Functionn_Params(TreeNode* AST, int max, int* more_them_max){
    // define local variables
    TreeNode* tmpTreeNode = AST;
    Function_Parameter* ParamsHead = NULL;
    Function_Parameter* ParamsTmp = NULL;
    Variable_And_Return_Type tmpType;

    int count = 0;
    char* errorMsg;

    // firstly, making sure that the ast node is type PARAMETERS
    if(AST == NULL || AST->node_type != NON_TERMINAL|| AST->numberOfChildrens == 0 || AST->Kind.nonTerminalType != NON_TERMINAL_PARAMETERS){
        printf("COMPILER ERRORN!\n");
        exit(EXIT_FAILURE);
    }

    while (tmpTreeNode!= NULL)
    {
        // increasing the parmas counter
        count += 1;
        

        if(tmpTreeNode->childrens[0]->node_type != NON_TERMINAL || tmpTreeNode->childrens[0]->Kind.nonTerminalType != NON_TERMINAL_PARAMETER || ((tmpTreeNode->childrens[0])->childrens[0])->node_type != NON_TERMINAL ||
            ((tmpTreeNode->childrens[0])->childrens[0])->Kind.nonTerminalType != NON_TERMINAL_TYPE){
            printf("COMPILER ERRORO!\n");
            exit(EXIT_FAILURE);
        }

        // processing the max first parameters
        if(count <= max){
            
            // GETTING THE TYPE OF THE CURRENT PARAMETER(here we dont address its name)
            tmpType = extract_variable_type((tmpTreeNode->childrens[0])->childrens[0]);
        
            // creaitng new paraneter strcut and adding it to the linked list
        
            // adding to the linked list
            if(ParamsHead == NULL){
                ParamsHead = create_parameter(tmpType);
                ParamsTmp = ParamsHead;
            }
            else{
                ParamsTmp->next_param = create_parameter(tmpType);
                ParamsTmp = ParamsTmp->next_param;
            }

            
        }
        // checking if we have only one child(meaning we are at the last iteration)
        if(tmpTreeNode->numberOfChildrens == 1){
            
            // checking if the counter greater them max
            if(count > max){
                
                // setting the flag that indicate that we overriden the max allowed params
                *more_them_max = count;
            }

            // returning the head of the linked params list
        
            return ParamsHead;
        }

        // stepping the tmpASTnode
        tmpTreeNode = tmpTreeNode->childrens[1];
    }

    // shoud never be here
    printf("COMPILER ERROR");
    exit(EXIT_FAILURE);
}

Scope* create_global_scope(){
    // creating a glocal scope object
    Scope* newScope = (Scope*)malloc(sizeof(Scope));
    SymbolTable* newSymbolTable;
    if(newScope == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }
    
   // setting the diafult attributes for the scope
   newScope->parent = NULL;
   newScope->childrens = NULL;
   newScope->number_of_childrens = 0;
   newScope->symbol_table = create_symbol_table(GLOBAL_SYMBOL_TABLE);
   newScope->variables_c = 0;
    return newScope;
}
Scope* create_local_scope(){
    Scope* newScope = (Scope*)malloc(sizeof(Scope));
    if(newScope == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }
   
    // setting the diafult attributes for the scope
    newScope->parent = NULL;
    newScope->childrens = NULL;
    newScope->number_of_childrens = 0;
    newScope->symbol_table = create_symbol_table(LOCAL_SYMBOL_TABLE);
    newScope->variables_c = 0;

    return newScope;
}

char* extract_variable_name(TreeNode* node) {
    if (node == NULL || node->node_type != TERMINAL || node->Kind.TerminalType != IDENTIFIER_TOKEN || node->value == NULL) {
        printf("Invalid variable name!\n");
        exit(1);
    }
    return node->value;
}

Variable_And_Return_Type extract_variable_type(TreeNode* node) {
    if (node == NULL || node->numberOfChildrens != 1 || node->node_type == TERMINAL || node->Kind.nonTerminalType != NON_TERMINAL_TYPE) {
        printf("Invalid type declaration!\n");
        exit(1);
    }

    TreeNode* typeNode = node->childrens[0];
    if (typeNode == NULL || typeNode->node_type != TERMINAL) {
        printf("Unknown situation!\n");
        exit(1);
    }

    switch (typeNode->Kind.TerminalType) {
        case INT_TOKEN: return VARIABLE_AND_RETURN_TYPE_INT;
        case CHAR_TOKEN: return VARIABLE_AND_RETURN_TYPE_CHAR;
        case VOID_TOKEN: return VARIABLE_AND_RETURN_TYPE_VOID;
        default:
            printf("Unknown type!\n");
            exit(1);
    }
}
Variable_And_Return_Type extract_function_return_type(TreeNode* AST){
    // define local variables
    TreeNode* tmp  = NULL;

    // the current node shuld be non terminal type, checking it
    if(AST == NULL || AST->node_type != NON_TERMINAL|| AST->numberOfChildrens != 1 || AST->Kind.nonTerminalType != NON_TERMINAL_TYPE){
        printf("COMPILER ERRORL!\n");
        exit(EXIT_FAILURE);
    }

    tmp = AST->childrens[0];
    if(tmp->Kind.TerminalType == INT_TOKEN){
        return VARIABLE_AND_RETURN_TYPE_INT;
    }
    if(tmp->Kind.TerminalType == CHAR_TOKEN){
        return VARIABLE_AND_RETURN_TYPE_CHAR;
    }
    if(tmp->Kind.TerminalType == VOID_TOKEN){
        return VARIABLE_AND_RETURN_TYPE_VOID;
    }

    // IF here, there is an compiler error
    printf("COMPILER ERRORM!\n");
        exit(EXIT_FAILURE);

}
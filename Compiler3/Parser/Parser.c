#include"Parser.h"

Parser* parser_init() {
    // Determine the size of the Parser object
    size_t size = sizeof(Parser); // Adjust the size to match your Parser structure

    // Use VirtualAlloc to allocate the memory for the parser
    Parser* parser = (Parser*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (parser == NULL) {
        // If allocation fails, print error and exit
        printf("VirtualAlloc failed with error code: %lu\n", GetLastError());
        exit(1);
    }
    if (!HeapValidate(GetProcessHeap(), 0, NULL)) {
        printf("Heap is corrupted!\n");
        exit(10);
    }
    
    // Set the allocated memory to zero
    memset(parser, 0, size);  
    
    // Return the pointer to the allocated memory for the parser object
    return parser;
}
Stack_Element* create_stack_element(TreeNode* tree, int state){
    
    Stack_Element* newElement = (Stack_Element*)malloc(sizeof(Stack_Element));
    if(newElement == NULL){
        printf("MALLOC ERROR");
        exit(1);
    }
    newElement->state = state;
    newElement->node = tree;
    return newElement;
}

Action parser_get_action(Parser* parser, int CurrentState, Terminals CurrentTerminal){
    return parser->parse_table.ActionMatrix[CurrentState][get_index_from_terminal_parser(CurrentTerminal)];
}
int parser_get_goto(Parser* parser, int CurrentState, Non_Terminals CnonTerminal){
    return parser->parse_table.GotoMatrix[CurrentState][get_index_from_Non_terminal_parser(CnonTerminal)];
}
void parser_add_token_Attr_to_treeNode(TreeNode* tree, Token* token){
    parser_treeNode_set_type(tree, TERMINAL);
    parser_treeNode_set_terminal(tree, (*token).type);
    if((*token).type == DIGIT_TOKEN){
        int* iPtr = (int*)malloc(sizeof(int));
        if(iPtr == NULL){
            perror("[ERROR]");
            exit(EXIT_FAILURE);
        }
        *iPtr = token->value.integer;
        tree->value = (void*)(iPtr);
    } 
    else if((*token).type == CHAR_LITERAL_TOKEN){
        char* cPtr =  (char*)malloc(sizeof(char));
        if(cPtr == NULL){
            perror("[ERROR]");
            exit(EXIT_FAILURE);
        }
        *cPtr = token->value.ch;
        tree->value = (void*)(cPtr);
    }
    else if((*token).type == IDENTIFIER_TOKEN){
        tree->value = (void*)((token->value.identifer));
    }

    // adding the token line and char in line for reporting errros
    tree->SourceCodeLine = token->SourceCodeLine;
    tree->CharacterInSourceCodeLine = token->CharacterInSourceCodeLine;

    //printf("so far, so good...");
}
int should_add_to_abstact_syntax_tree(Terminals token) {
    // Tokens that have semantic meaning and should be added to the stack
    switch (token) {
        case IDENTIFIER_TOKEN:
        case DIGIT_TOKEN:
        case CHAR_LITERAL_TOKEN:
        //case GETCHAR_TOKEN:
        //case PUTCHAR_TOKEN:
        case INT_TOKEN:
        case CHAR_TOKEN:
        case VOID_TOKEN:
        //case RETURN_TOKEN:
        //case IF_TOKEN:
        case ELSE_TOKEN:
        //case FOR_TOKEN:
        //case WHILE_TOKEN:
        case LOGICAL_OR_TOKEN:
        case LOGICAL_AND_TOKEN:
        case EQUAL_TO_TOKEN:
        case NOT_EQUAL_TO_TOKEN:
        case LESS_THEN_TOKEN:
        case GREATER_THEN_TOKEN:
        case LESS_THEN_OR_EQUAL_TO_TOKEN:
        case GREATER_THEN_OR_EQUAL_TO_TOKEN:
        case PLUS_TOKEN:
        case MINUS_TOKEN:
        case MULTIPLY_TOKEN:
        case DIVIDE_TOKEN:
        case MODULO_TOKEN:
        case LOGICAL_NOT_TOKEN:
            return 1; // Add to stack
        default:
            return 0; // Do not add to stack
    }
}

const char* actionTypeToString(ActionType type) {
    switch (type) {
        case SHIFT:
          return "SHIFT";
        case REDUCE:
         return "REDUCE";
        case ACCEPT: return "ACCEPT";
        
        default: return "UNKNOWN";
    }
}
int isValidSyncronizationToken(Token* token) {
    // this function return true if the given token can be used 
    // as a sybcronization token while doing error recovrry
    if (token == NULL) {
        return 0; // Cannot synchronize on NULL
    }

    switch (token->type) {
        // --- Reliable tokens, 
        case SEMICOLON_TOKEN:       
        case CLOSE_PARENTHESES_TOKEN:
        case CLOSE_BRACE_TOKEN:     
        case IF_TOKEN:             
        case ELSE_TOKEN:           
        case WHILE_TOKEN:           
        case RETURN_TOKEN:         
        case GETCHAR_TOKEN:      
        case PUTCHAR_TOKEN:      
        case INT_TOKEN:            
        case CHAR_TOKEN:           
        case VOID_TOKEN:            
            return 1;

        default:
            // Any other token is not considered a primary synchronization point.
            return 0;
    }
}

Action panicMode_Adjust_Parser_Stack(Parser* parser, Parser_Stack* stack, Token* token){
    // This function is used when entered panic mode.
    // It adjusts the stack by popping elements until it finds a state
    // that can perform a valid action (usually shift) on the synchronization token.

    Action RecoveryAction;
    int CurrentState;
    Stack_Element* tmpStackElement;
    
    // Loop until we find a state that can handle the synchronization token
    // or the stack becomes empty.
    while (!parser_stack_isEmpty(*stack)) {
        CurrentState = parser_stack_top(*stack)->state;
        RecoveryAction = parser_get_action(parser, CurrentState, token->type);
        //printf("looping..");

        // Check if the current state on top of the stack can handle
        // the synchronization token (i.e., the action is NOT an error action).
        // An error action is typically represented by { type=0, rule_or_state=0 }
        // or some specific ERROR type if you defined one.
        if ((RecoveryAction.typeOfAction == SHIFT && RecoveryAction.rule_or_state != 0) || RecoveryAction.typeOfAction == REDUCE){
            
            // Found a state that can handle the synchronization token.
            // Return the state we need to shift *to* based on the recovery action.
            // For panic mode recovery, we usually expect to SHIFT the sync token.
            
            return RecoveryAction;
        }

        // If the current state cannot handle the sync token, pop it.
        tmpStackElement = parser_stack_pop(stack);

        if (tmpStackElement) {
             // Free the associated AST node if it exists
             // Avoid freeing the initial NULL node pushed at state 0
            if(tmpStackElement->node != NULL) {
                
                free(tmpStackElement->node->value); // Free value if allocated
                free(tmpStackElement->node->childrens); // Free children array if allocated
                free(tmpStackElement->node); // Free the node itself
            }
            free(tmpStackElement); // Free the stack element wrapper
        }
    }

    // If the stack becomes empty, we couldn't find a recovery state.
    //printf("Panic Mode Recovery Failed: Stack emptied.\n");
    return (Action){-1, -1}; // Indicate failure to recover
}
void panicMode_Skip_Unsynchronization_Points(Token** tokens){
    // this function is used in panic mode to skipp unsynchronize points

    // firstly, discarding the first token, which causes the eror
    (*tokens)++;
    
    // skipping tokens
    while((*tokens)->type != DOLLAR_TOKEN && !isValidSyncronizationToken(*tokens)){
        (*tokens)++;
    }
    
}
void convertTokenFormat(Token* tokens){
    while(tokens[0].type != END_OF_TOKENS_TOKEN) tokens++;
    tokens[0].type = DOLLAR_TOKEN;
}

void parser_shift(Parser_Stack* stack, Token* token, int state){
    // this function responsible for shifting action in the parsing algorithm
    // Shift action: create new tree from the token and push it onto the stack

    // define local variables
    TreeNode* tempTreeNode;
    Stack_Element* stack_element;
    

    // creating new tree node
    tempTreeNode = parser_treeNode_create_TreeNode();
    
    // adding right attributes for the new node
    parser_add_token_Attr_to_treeNode(tempTreeNode, token);
    
    // creating new stack element with the new tree node and the corresponding valu
    stack_element = create_stack_element(tempTreeNode, state);
    
    // pusing the new element to the stack
    parser_stack_push(stack, stack_element);

}
void parser_reduce(Parser* parser, Parser_Stack* stack, int rule){
    // this function is preforming the reduce action in the parsing alorithm
    // Reduce action: create new non-terminal node and reduce the stack elements

    // define local variables
    TreeNode* tempTreeNode;
    TreeNode* lastSon;


    Stack_Element* tempStackElement;

    int numerOfProductions;
    int numerOfProductionsInAst;
    int i;
    int nextState;
            
    tempTreeNode = parser_treeNode_create_TreeNode();
           
    parser_treeNode_set_type(tempTreeNode, NON_TERMINAL);
    
    numerOfProductions = parser->Production_Rules[rule].numberOfRightProductions;
    parser_treeNode_set_non_terminal(tempTreeNode, parser->Production_Rules[rule].Production_Rule_Type);
    
    // checking how many childes should be in the tree
    numerOfProductionsInAst = countMagnitudedElementsInTheStack(stack, numerOfProductions);
    
    // Allocate memory for children of the new tree
    tempTreeNode->childrens = (TreeNode**)malloc(sizeof(TreeNode*) * numerOfProductionsInAst);
    if(tempTreeNode->childrens == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < numerOfProductions; i++) {
         
        tempStackElement = parser_stack_pop(stack);
        if(tempStackElement == NULL){
            printf("COMPILER ERROR");
            exit(EXIT_FAILURE);
        }
         
        // checking if element has magnitude for the AST
        if (!((tempStackElement->node)->node_type == TERMINAL && !should_add_to_abstact_syntax_tree((tempStackElement->node)->Kind.TerminalType))) {
            // adding the element tree node as child of the new node
            parser_treeNode_addChild(tempTreeNode, tempStackElement->node, numerOfProductionsInAst);

            // saving the last sone that were added as a child in the ast
            lastSon = tempStackElement->node;
        }
        
        // Free the stack element after processing,
        free(tempStackElement);  
    }

    tempTreeNode->SourceCodeLine = lastSon->SourceCodeLine;
    tempTreeNode->CharacterInSourceCodeLine = lastSon->CharacterInSourceCodeLine;

    // Go to next state using the "goto" table
    nextState = parser_get_goto(parser, (*parser_stack_top(*stack)).state, tempTreeNode->Kind.nonTerminalType);
    tempStackElement = create_stack_element(tempTreeNode, nextState);
    
    // pusing the nre element to the stack
    parser_stack_push(stack, tempStackElement);
}
TreeNode* parser_accept(Parser_Stack* stack){
    // Accept action: pop the top of the stack and return the tree node

    // define local variables
    Stack_Element* tempStackElement;
    TreeNode* tempTreeNode;

    // poping the top element, this element containg the souce code full AST
    tempStackElement = parser_stack_pop(stack);

    tempTreeNode = tempStackElement->node;
    free(tempStackElement);  // Free the stack element after processing
    
    // destroying the stack
    parser_stack_destroyStack(stack);

    // returning the ast top node
    return tempTreeNode;
}
void parser_print_parser_stack(Parser_Stack* stack){
    printf("Stack: ");
    Parser_Stack *tmp_stack = stack;
    int bla = 0;
    printf("[ ");
    while(bla < stack->size){
        printf(" %d,", stack->arr[bla]->state);
        bla++;
    }
    printf("]\n");
}

void parser_recovery_error(Parser* parser, Parser_Stack* stack, Token** tokens){
    // define local variables
    Action recoveryAction; // THIS WILL BE THE FIRST VALID ACTION WHICH WILL BE USED FOR THE RECOVERY

    //printf("Entering Panic Mode Recovery...\n"); // Debugging

    // Skip input tokens until a synchronization token or EOF is found.
    panicMode_Skip_Unsynchronization_Points(tokens);

    // Check if we hit the end of the input stream.
    if((*tokens)->type == DOLLAR_TOKEN){
        // Cannot recover if the sync point is the end of the file.
        // The error likely involves expecting something before EOF.
        printf("Syntax Error: Unexpected end of file during error recovery.\n");
        // Depending on requirements, you might exit, return NULL, or try a final reduction.
        exit(EXIT_FAILURE); // Simplest approach for now
    }

    // Adjust the parser stack to find a state that can handle the sync token.
    recoveryAction = panicMode_Adjust_Parser_Stack(parser, stack, *tokens);

    // Check if stack adjustment was successful.
    if(recoveryAction.rule_or_state == -1 && recoveryAction.typeOfAction == -1){
        // Stack adjustment failed (e.g., stack became empty). Recovery failed.
        printf("Fatal Error: Cannot recover from syntax error near line %d. Stack adjustment failed.\n", (*tokens)->SourceCodeLine);
        // Report error more formally if needed
        exit(EXIT_FAILURE); // Cannot continue parsing
    }

    // Perform the recovery action: Shift the synchronization token, or reduce it
    if(recoveryAction.typeOfAction == SHIFT){
        //printf("Recovered: Shifting token "); // Debugging
        //printTokenTypeName((*tokens)->type);
        //printf(" into state %d\n", recoveryAction.rule_or_state);

        parser_shift(stack, *tokens, recoveryAction.rule_or_state);

        // getting the next token
        (*tokens)++;

    }
    else if(recoveryAction.typeOfAction == REDUCE){
        //printf("Recovered: reduce token\n");
        parser_reduce(parser, stack, recoveryAction.rule_or_state);
    }
}

char* Parser_PrepareErrorMessage(Parser* parser, int state ,Token* invalidToken){
    // define local variables

    // temporery buffers
    char msg[1000];
    char msg2[1000];

    // integer for iteration
    int i;

    // tell if we are in the first token
    int isFirstValidToken = 1;

    // temp structures
    Action tmpAction;
    Token tmpToken;

    // getting the name of the invalid token
    char *token_name = Lexer_GetTokenName(invalidToken);
    //printf("token is %s\n", (token_name == NULL)?"NULL":"NOT NULL");
    

    // setting the message with using convinient format
    sprintf(msg, "unexpected token: '%s', expecting: ", token_name);

    //looking for all valid tokens at current state
    for(i = 0 ; i < END_OF_TOKENS_TOKEN; i++){
        // getting the action of the current index
        tmpAction = parser->parse_table.ActionMatrix[state][i];

        // checking if the current index(which represen a terminal) is valid
        if((tmpAction.typeOfAction == SHIFT && tmpAction.rule_or_state != 0) || tmpAction.typeOfAction == REDUCE){
            // the current index represent a valid token!
            // adding the token into the error message as vaild possible token

            // setting the temp token type 
            tmpToken.type = i;

            // getting the token name
            token_name = Lexer_GetTokenName(&tmpToken);

            // checking if this is the first iteration
            if(isFirstValidToken){
                sprintf(msg2, "'%s'", token_name);
                isFirstValidToken = 0;
            }
            else{
                sprintf(msg2, " or '%s'", token_name);
            }
            
            // concating the messages
            strcat(msg, msg2);
            
        }
    }

    // returning an dynamic memory version of the buffer msg
    return _strdup(msg);

}
TreeNode* create_abstact_syntax_tree(Parser* parser, Token* tokens){
    // ... (setup code remains the same) ...
    Parser_Stack *stack = parser_stack_createStack();
    int CurrentState;
    Action CurrentAction;
    int panicMode = 0; 
    char* errorMsg; // hold the error message before outputing

    convertTokenFormat(tokens);

    Stack_Element* initialElement = create_stack_element(NULL, 0); // State 0, no node
    parser_stack_push(stack, initialElement);

    while (1) {
        if (parser_stack_isEmpty(*stack)) {
            printf("Fatal Error: Parser stack became empty unexpectedly.\n");
            // Handle this error case, maybe return NULL or exit
            // This shouldn't happen in a correct SLR(1) implementation unless recovery fails badly
            exit(EXIT_FAILURE);
        }
        CurrentState = parser_stack_top(*stack)->state;

        // Debugging: Print stack and current token
        // parser_print_parser_stack(stack);
        //printf("Current Token: "); printTokenTypeName(tokens[0].type); printf("\n");

        CurrentAction = parser_get_action(parser, CurrentState, tokens[0].type);

        // Check for Syntax Error (Invalid entry in Action Table)
        // Assuming Error is marked by type=0, rule_or_state=0
        if(CurrentAction.typeOfAction == 0 && CurrentAction.rule_or_state == 0){
            // --- Syntax Error Detected ---
            panicMode = 1; // Set flag 

            // outputing the base error message
            ErrorReporting_ReportError_Base(ERROR_ON_PARSER_PHASE,
                tokens[0].SourceCodeLine,
                tokens[0].CharacterInSourceCodeLine);
            
            // getting the detailed error messge 
            errorMsg = Parser_PrepareErrorMessage(parser, CurrentState, tokens);
            
            // outputing the error message
            ErrorReporting_ReportError(errorMsg);
            
            // freeing the error msg
            free(errorMsg);

            // Attempt Panic Mode Recovery
            parser_recovery_error(parser, stack, &tokens);

            // After recovery, the stack and current token are adjusted.
            // Continue the loop to process the (potentially new) current token
            // with the state now on top of the stack.
        }
        else if (CurrentAction.typeOfAction == SHIFT) {
            parser_shift(stack, tokens, CurrentAction.rule_or_state);
            tokens++; // Consume token only on successful shift
        }
        else if (CurrentAction.typeOfAction == REDUCE) {
            parser_reduce(parser, stack, CurrentAction.rule_or_state);
            // Do NOT advance token on reduce
        }
        else if (CurrentAction.typeOfAction == ACCEPT) {
            // Check if the ACCEPT happens on the DOLLAR_TOKEN
            if (tokens[0].type == DOLLAR_TOKEN) {
                if(panicMode){
                    freeAst(parser_accept(stack));
                    return NULL;
                }
                else{
                    //printf("Parse Successful (ACCEPT)\n");
                    return parser_accept(stack);
                }
                 
            } else {
                 // Should not ACCEPT before reaching end of input ($)
                ErrorReporting_ReportError_Base(ERROR_ON_PARSER_PHASE,
                                           tokens[0].SourceCodeLine,
                                           tokens[0].CharacterInSourceCodeLine); // Error: ACCEPT before $
                printf("Error: ACCEPT action occurred before end of input ($).\n");
                // Attempt recovery? Or treat as fatal? Let's try recovery.
                parser_recovery_error(parser, stack, &tokens);
                continue;
            }
        }
        
    }
    // Should not reach here if ACCEPT works correctly
    return NULL;
}

int countMagnitudedElementsInTheStack(Parser_Stack *stack, int num){
    // checking the amount of magnituded elemtns in the first num elenents
    int c = 0;
    int i;
    Stack_Element *tempElem = NULL;
    // creating new temp stack
    Parser_Stack *temp = parser_stack_createStack();

    // making sure num lower then the stack elements
    if(stack->size < num){
        printf("this is good??? found new problem");
        exit(1);
    }
    for(i = 0; i < num; i++){

        tempElem = parser_stack_pop(stack);
        parser_stack_push(temp, tempElem);

        // check if not an inportent node and can be not add
        if ((tempElem->node)->node_type == TERMINAL && !should_add_to_abstact_syntax_tree((tempElem->node)->Kind.TerminalType)){
            continue;
        }

        // increase the counter
        c += 1;
    }

    // returning the element to the intiall stack
    for(i = 0; i < num; i++){
        tempElem = parser_stack_pop(temp);
        parser_stack_push(stack, tempElem);
    }
    parser_stack_destroyStack(temp);
    return c;
}

void freeAst(TreeNode* root){
    
    // function for recursivly freeing the ast nodes
    if(root == NULL) return;
    for(int i = 0 ; i < root->numberOfChildrens; i++){
        freeAst(root->childrens[i]);
    }
    
    // freeing this node and its dynamic attributes

    // freeing the array of node's pointers
    free(root->childrens);
    
    // freeing the node value(if not null)
    if(root->value != NULL){
        free(root->value);
    }
    
    // freeing the node itself
    free(root);
    
}
// Recursive function to print the AST
void printAST(TreeNode *node, int indent) {
    if (node == NULL) return;

    // Print indentation dynamically
    for (int i = 0; i < indent; i++) {
        printf("__"); // Indentation for clarity
    }

    printf("Node: "); // Indicating a new node

    // Print the node value
    if (node->node_type == TERMINAL) {
        printf("[TERMINAL] ");

        // Debug print before calling printTokenTypeName
        printf("Token Type: ");
        printTokenTypeName(node->Kind.TerminalType);
        
        // Ensure node->value is not NULL before printing
        
        // Print the terminal value based on its type
        if (node->Kind.TerminalType == IDENTIFIER_TOKEN) {
            printf("| Identifier: %s", (char*)node->value);
        } 
        else if (node->Kind.TerminalType == CHAR_LITERAL_TOKEN) {
            printf("| Char: '%c'",*((char*)(node->value)));
        } 
        else if (node->Kind.TerminalType == DIGIT_TOKEN) {
           printf("| Integer: %d", *((int*)(node->value)));
        }

        printf("\n"); // Newline for clarity
    } 
    else { // Non-Terminal node
        printf("[NON-TERMINAL] Type: %d", node->Kind.nonTerminalType);
        printf("\n");

        // Debug print for number of children
        printf("Child Count: %d\n", node->numberOfChildrens);

        // Recursively print all children
        for (int i = 0; i < node->numberOfChildrens; i++) {
            if (node->childrens[i] == NULL) {
                printf("Warning: NULL child at index %d\n", i);
            } else {
                printAST(node->childrens[i], indent - 5);
            }
        }
    }
}
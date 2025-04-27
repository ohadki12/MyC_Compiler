#include"CodeGenerator.h"


// global variable, registers pool
RegisterPool registersPool;
FILE* OUTPUT_FILE;

Register* CodeGenerator_CreateRegister(char* name, int inuse){
    Register* reg = (Register*)malloc(sizeof(Register));
    if(reg == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }
    reg->inuse = inuse;
    reg->registerName = _strdup(name);
}

void CodeGenerator_InititeRegistersPool(){
    // define local variables
    int reg = 0;
    
    // setting the registers pool attributes
    registersPool.number_of_registers = GLOBAL_USE_REGISTERS_NUM;

    // setting the global registers in the pool
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R8, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R9, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R10, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R11, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R12, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R13, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R14, 0);
    registersPool.registers[reg++] =  CodeGenerator_CreateRegister(R15, 0);

}

Register* allocate_register() {
    for (int i = registersPool.number_of_registers - 1; i >= 0; --i) {
        if (!registersPool.registers[i]->inuse) {
            registersPool.registers[i]->inuse = 1;
            return registersPool.registers[i];
        }
    }
    printf("COMPILER ERROR: No available registers\n");
    return NULL;
}

void free_register(Register* reg) {
    reg->inuse = 0;
}
char* CodeGenerator_Create_Label(){
    // this function generate unique label names

    // define static variable
    static int label_c = 1;

    // define local variables
    char buf[15];

    sprintf(buf, "%s%d", LABEL_BASE_NAME, label_c++);
    return _strdup(buf);
}


int CodeGenerator_GetTypeSize(TreeNode* AST){
    // checking that the current ast node is terminal 
    if(AST->node_type == NON_TERMINAL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }

    // cheking if int
    if(AST->Kind.TerminalType == INT_TOKEN){
        return 8; // 8 *8 = 64 bits
    }
    // cheking if char
    if(AST->Kind.TerminalType == CHAR_TOKEN){
        return 1; // 1 * 8 = 8  bits
    }

    // if here, error
    printf("COMPILER ERROR");
    exit(EXIT_FAILURE);
}
char* CodeGenerator_GetRegisterLowestByteRegister(const char* reg) {
    if (strcmp(reg, RAX) == 0) return _strdup(AL);
    if (strcmp(reg, RBX) == 0) return _strdup(BL);
    if (strcmp(reg, RCX) == 0) return _strdup(CL);
    if (strcmp(reg, RDX) == 0) return _strdup(DL);
    if (strcmp(reg, RSI) == 0) return _strdup(SIL);
    if (strcmp(reg, RDI) == 0) return _strdup(DIL);
    if (strcmp(reg, RBP) == 0) return _strdup(BPL);
    if (strcmp(reg, RSP) == 0) return _strdup(SPL);

    if (strcmp(reg, R8)  == 0) return _strdup(R8B);
    if (strcmp(reg, R9)  == 0) return _strdup(R9B);
    if (strcmp(reg, R10) == 0) return _strdup(R10B);
    if (strcmp(reg, R11) == 0) return _strdup(R11B);
    if (strcmp(reg, R12) == 0) return _strdup(R12B);
    if (strcmp(reg, R13) == 0) return _strdup(R13B);
    if (strcmp(reg, R14) == 0) return _strdup(R14B);
    if (strcmp(reg, R15) == 0) return _strdup(R15B);

    return NULL; // Unknown register
}
int CodeGenerator_GetNumberOfVariable(Scope* scope){
    // this function count and return the number of declared varialbles
    // in this current scope and in all the inner scopes, not addressing variable type

    // define loacal variables
    int prev_c = 0;
    int i;

    // counting the amount of declared variables in the inner scopes fiest
    // if we are in a leaf scope, thats mean that the number of childrens is zero and this 
    // for loop will not call the function recursivly and behave as an exit condition
    for(i = 0; i < scope->number_of_childrens; i++){
        prev_c += CodeGenerator_GetNumberOfVariable(scope->childrens[i]);
    }

    // returning the prev count + the count in this scope
    return prev_c + scope->variables_c;
}
Symbol_Table_Function_Entry* CodeGenerator_FetchFunctionEntry(SymbolTable* globalSymbolTable, char* func_name){
    // this function returning the entry which beling to the function with the given name

    // define local variables
    Symbol_Table_Function_Entry* tmp;

    // getting the first entry
    tmp = globalSymbolTable->Function_Entry;

    // looping for all nodes in the list
    while (tmp != NULL)
    {
        if(strcmp(tmp->name, func_name) == 0){
            // return the current entry
            return tmp;
        }

        // iterating to the next entry in the list
        tmp = tmp->next_entry;
    }
    return NULL;
    
}
Symbol_Table_Variable_Entry* CodeGenerator_FetchVariableEntry(Scope* scope, char* var_name) {
    Symbol_Table_Variable_Entry* tmp;
    //printf("looking for name: %s\n", var_name);
    if(scope == NULL){
        printf("COMPILER ERROR1");
        exit(EXIT_FAILURE);
    }
    if(scope->parent == NULL){
        printf("COMPILER ERROR2");
        exit(EXIT_FAILURE);
    }
    
    // Walk up the scope hierarchy
    while (scope != NULL && scope->symbol_table != NULL) {
        if (scope->symbol_table->symbol_table_type == GLOBAL_SYMBOL_TABLE) {
            break; // Stop at global scope
        }
        
        tmp = scope->symbol_table->Variable_Entry;
        //print_symbol_table(scope->symbol_table);
        while (tmp != NULL) {
            
            if (strcmp(tmp->name, var_name) == 0) {
                //printf("found");
                return tmp; // Found
            }
            tmp = tmp->next_entry;
        }

        // Move up to parent
        scope = scope->parent;
    }

    return NULL; // Not found
}
void CodeGenerator_PrintHeader() {
    fprintf(OUTPUT_FILE, "BITS 64          ; Specify 64-bit code generation\n");
    fprintf(OUTPUT_FILE, "DEFAULT REL      ; Use RIP-relative addressing by default\n");
    fprintf(OUTPUT_FILE, "global main\n");
    fprintf(OUTPUT_FILE, "extern GetStdHandle\n");
    fprintf(OUTPUT_FILE, "extern WriteConsoleA\n");
    fprintf(OUTPUT_FILE, "extern ReadConsoleA\n");
    fprintf(OUTPUT_FILE, "extern ExitProcess\n");

    fprintf(OUTPUT_FILE, "\n");
    fprintf(OUTPUT_FILE, "section .data\n");
    fprintf(OUTPUT_FILE, "CHAR_BUF db 0\n");
    fprintf(OUTPUT_FILE, "STDOUT_HANDLE dq 0\n");
    fprintf(OUTPUT_FILE, "N_WRITTEN dq 0\n");
    fprintf(OUTPUT_FILE, "STDIN_HANDLE dq 0  ; Declare stdin_handle in the .data section\n");
    fprintf(OUTPUT_FILE, "N_READ dq 0        ; Declare n_read in the .data section\n");
    fprintf(OUTPUT_FILE, "\n");
    fprintf(OUTPUT_FILE, "section .text\n");
}
#include <stdio.h> // Assuming you have this included

// Assuming OUTPUT_FILE is a valid FILE* pointer opened for writing
extern FILE *OUTPUT_FILE;

void CodeGenerator_write_asm_functions_to_file() {
    // Ensure you are in the .text section before writing functions
    // fprintf(OUTPUT_FILE, "section .text\n"); // Uncomment if needed

    fprintf(OUTPUT_FILE, "; Writes a single character (passed in RCX) to the console.\n");
    fprintf(OUTPUT_FILE, "; Uses local stack space for buffer and bytes written variable.\n");
    fprintf(OUTPUT_FILE, "putchar:\n");
    fprintf(OUTPUT_FILE, "    ; ABI: Character expected in CL (lowest byte of RCX)\n");
    fprintf(OUTPUT_FILE, "    PUSH RBP\n");
    fprintf(OUTPUT_FILE, "    MOV RBP, RSP\n");
    fprintf(OUTPUT_FILE, "    ; Allocate 40 bytes: \n");
    fprintf(OUTPUT_FILE, "    ; - Aligns RSP: RBP is (Initial RSP - 8), so RBP - 40 = Initial RSP - 48 (Aligned)\n");
    fprintf(OUTPUT_FILE, "    ; - Provides 32 bytes shadow space for callees below entry RSP.\n");
    fprintf(OUTPUT_FILE, "    ; - Provides space for locals (1 byte char buffer, 8 byte QWORD target for N_WRITTEN).\n");
    fprintf(OUTPUT_FILE, "    SUB RSP, 40\n");
    fprintf(OUTPUT_FILE, "    ; Stack frame layout relative to RBP:\n");
    fprintf(OUTPUT_FILE, "    ; [RBP - 8] .. [RBP - 40] = 32 bytes available below RBP for locals/padding.\n");
    fprintf(OUTPUT_FILE, "    ; Use [RBP - 16] for N_WRITTEN target (needs 8 bytes).\n");
    fprintf(OUTPUT_FILE, "    ; Use [RBP - 24] for char buffer (needs 1 byte).\n");
    fprintf(OUTPUT_FILE, "    \n");
    fprintf(OUTPUT_FILE, "    ; Store the character (from CL) onto the STACK buffer\n");
    fprintf(OUTPUT_FILE, "    MOV BYTE [RBP - 24], CL\n");
    fprintf(OUTPUT_FILE, "    \n");
    fprintf(OUTPUT_FILE, "    ; Get STDOUT handle if not already obtained (still cache globally)\n");
    fprintf(OUTPUT_FILE, "    CMP QWORD [rel STDOUT_HANDLE], 0\n"); // Use rel for global handle cache
    fprintf(OUTPUT_FILE, "    JNE .putchar_have_handle_stack\n");   // Use unique label
    fprintf(OUTPUT_FILE, "    MOV RCX, -11               ; STD_OUTPUT_HANDLE\n");
    fprintf(OUTPUT_FILE, "    CALL GetStdHandle          ; Returns handle in RAX\n");
    fprintf(OUTPUT_FILE, "    ; Optional but Recommended: Check GetStdHandle for errors\n");
    fprintf(OUTPUT_FILE, "    ; Example: CMP RAX, -1 ; or CMP RAX, 0 depending on what errors to check\n");
    fprintf(OUTPUT_FILE, "    ;          JE .putchar_error_handler\n");
    fprintf(OUTPUT_FILE, "    MOV [rel STDOUT_HANDLE], RAX\n");     // Store handle globally
    fprintf(OUTPUT_FILE, ".putchar_have_handle_stack:\n");         // Unique label
    fprintf(OUTPUT_FILE, "    \n");
    fprintf(OUTPUT_FILE, "    ; Prepare arguments for WriteConsoleA using STACK locations\n");
    fprintf(OUTPUT_FILE, "    MOV RCX, [rel STDOUT_HANDLE]    ; 1st: hConsoleOutput (from global cache)\n");
    fprintf(OUTPUT_FILE, "    LEA RDX, [RBP - 24]             ; 2nd: lpBuffer (address of char on STACK)\n");
    fprintf(OUTPUT_FILE, "    MOV R8, 1                       ; 3rd: nNumberOfCharsToWrite\n");
    fprintf(OUTPUT_FILE, "    LEA R9, [RBP - 16]             ; 4th: lpNumberOfCharsWritten (address on STACK for dummy output)\n");
    fprintf(OUTPUT_FILE, "    MOV QWORD [RSP+32], 0           ; 5th: lpReserved (NULL) - Positioned relative to current RSP (accesses caller's arg space)\n");
    fprintf(OUTPUT_FILE, "    CALL WriteConsoleA\n");
    fprintf(OUTPUT_FILE, "    ; Optional but Recommended: Check WriteConsoleA for errors (returns 0 on failure)\n");
    fprintf(OUTPUT_FILE, "    ; Example: TEST RAX, RAX\n");
    fprintf(OUTPUT_FILE, "    ;          JZ .putchar_error_handler\n");
    fprintf(OUTPUT_FILE, "    \n");
    fprintf(OUTPUT_FILE, "    ; CLEAN UP STACK AND RETURN using LEAVE\n");
    fprintf(OUTPUT_FILE, "    LEAVE                    ; Equivalent to: MOV RSP, RBP; POP RBP\n");
    fprintf(OUTPUT_FILE, "    RET\n");
    fprintf(OUTPUT_FILE, "\n");

    
    // --- Write getchar function (Corrected for Win64) ---
    // Start generating the assembly 'getchar' function
    fprintf(OUTPUT_FILE, "; --- getchar (Re-entrant version using stack) ---\n");
    fprintf(OUTPUT_FILE, "; Reads a single character from the console.\n");
    fprintf(OUTPUT_FILE, "; Takes NO parameters.\n"); // Explicitly state no params for assembly func
    fprintf(OUTPUT_FILE, "; Returns character in AL (zero-extended into RAX).\n");
    fprintf(OUTPUT_FILE, "; Uses local stack space for buffer and bytes read variable.\n");
    fprintf(OUTPUT_FILE, "getchar:\n"); // Label for the assembly function

    // Standard Win64 function prologue
    fprintf(OUTPUT_FILE, "    PUSH RBP\n");
    fprintf(OUTPUT_FILE, "    MOV RBP, RSP\n");

    // Allocate stack space: 40 bytes total below saved RBP.
    // Ensures 16-byte alignment for RSP after allocation (RBP-40 is Initial_RSP-48).
    // Provides 32 bytes shadow space for API calls below entry RSP.
    // Provides local space for 1-byte buffer and 8-byte QWORD target.
    fprintf(OUTPUT_FILE, "    SUB RSP, 40\n");
    fprintf(OUTPUT_FILE, "    ; Local stack layout: Use [RBP - 16] for N_READ target, [RBP - 24] for char buffer.\n");
    fprintf(OUTPUT_FILE, "    \n");

    // Get STDIN handle (using global cache)
    fprintf(OUTPUT_FILE, "    ; Get STDIN handle if not already obtained (still cache globally)\n");
    fprintf(OUTPUT_FILE, "    CMP QWORD [rel STDIN_HANDLE], 0\n");
    fprintf(OUTPUT_FILE, "    JNE .getchar_have_handle_stack\n");
    fprintf(OUTPUT_FILE, "    MOV RCX, -10               ; STD_INPUT_HANDLE (Arg 1 for GetStdHandle)\n");
    fprintf(OUTPUT_FILE, "    CALL GetStdHandle          ; Returns handle in RAX\n");
    fprintf(OUTPUT_FILE, "    ; Optional: Check GetStdHandle for errors\n");
    fprintf(OUTPUT_FILE, "    MOV [rel STDIN_HANDLE], RAX\n");
    fprintf(OUTPUT_FILE, ".getchar_have_handle_stack:\n");
    fprintf(OUTPUT_FILE, "    \n");

    // Prepare arguments for ReadConsoleA using stack locations
    fprintf(OUTPUT_FILE, "    ; Prepare arguments for ReadConsoleA using STACK locations\n");
    fprintf(OUTPUT_FILE, "    MOV RCX, [rel STDIN_HANDLE]     ; Arg 1: hConsoleInput (from global cache)\n");
    fprintf(OUTPUT_FILE, "    LEA RDX, [RBP - 24]             ; Arg 2: lpBuffer (address of char buffer on STACK)\n");
    fprintf(OUTPUT_FILE, "    MOV R8, 1                       ; Arg 3: nNumberOfCharsToRead\n");
    fprintf(OUTPUT_FILE, "    LEA R9, [RBP - 16]             ; Arg 4: lpNumberOfCharsRead (address on STACK)\n");
    fprintf(OUTPUT_FILE, "    MOV QWORD [RSP+32], 0           ; Arg 5: pInputControl (NULL) - Placed in caller's arg space\n");

    // Call ReadConsoleA to get the character
    fprintf(OUTPUT_FILE, "    CALL ReadConsoleA\n");
    fprintf(OUTPUT_FILE, "    ; Optional: Check ReadConsoleA for errors (returns 0 on failure)\n");
    fprintf(OUTPUT_FILE, "    \n");

    // Prepare return value
    fprintf(OUTPUT_FILE, "    ; Return character read from STACK buffer in AL (zero-extended to RAX)\n");
    fprintf(OUTPUT_FILE, "    MOVZX EAX, BYTE [RBP - 24]\n"); // Read from stack buffer, zero-extend into EAX/RAX
    fprintf(OUTPUT_FILE, "    \n");

    // Standard Win64 function epilogue
    fprintf(OUTPUT_FILE, "    ; CLEAN UP STACK AND RETURN using LEAVE\n");
    fprintf(OUTPUT_FILE, "    LEAVE                    ; MOV RSP, RBP; POP RBP\n");
    fprintf(OUTPUT_FILE, "    RET\n"); // Return to caller, result is in RAX
    fprintf(OUTPUT_FILE, "\n");
}

void CodeGenerator_Generate(TreeNode* AST, Scope* scope, FILE* dest_file){
    // main function for generating the destination code

    // first of all, set the global variable to teh given file
    OUTPUT_FILE = dest_file;

    // firstly, print the headers and text needed for nasm
    CodeGenerator_PrintHeader();

    // print putchar and getchar io function helpers
    CodeGenerator_write_asm_functions_to_file();

    fflush(OUTPUT_FILE);

    // intiialge the registers pool
    CodeGenerator_InititeRegistersPool();

    // calling generating program
    CodeGenerator_GenerateProgram(AST, scope);
}

void CodeGenerator_GenerateProgram(TreeNode* AST, Scope* scope){
    CodeGenerator_GenerateFunctions(AST->childrens[0], scope);

}

void CodeGenerator_GenerateFunctions(TreeNode* AST, Scope* scope){
    if(AST->numberOfChildrens == 1){
        CodeGenerator_GenerateFunction(AST->childrens[0], scope);
    }
    else if(AST->numberOfChildrens == 2){
        CodeGenerator_GenerateFunction(AST->childrens[0], scope);
        CodeGenerator_GenerateFunctions(AST->childrens[1], scope);
    }
}

void CodeGenerator_GenerateFunction(TreeNode* fnNode, Scope* globalScope) {
    // define local variables
    char *func_name;
    Symbol_Table_Function_Entry* func_entry;
    int vars_count;
    char* finishLabel;

    // this varible hold the size of the amount of bytes we allocated in the stack for local variables
    // this varible need to be divisable by 16 
    int stack_sub; 


    // get the name of the function
    func_name = extract_variable_name(fnNode->childrens[1]);

    // getting the function entry based on its name
    func_entry = CodeGenerator_FetchFunctionEntry(globalScope->symbol_table, func_name);

    // getting the amount of declaration in the whole function(this scope and all inner scopes)
    vars_count = CodeGenerator_GetNumberOfVariable(globalScope->childrens[func_entry->function_index]);

    // setting an label for jumping to the end of the function
    // in case where the function uses the return function somehwere inside
    finishLabel = CodeGenerator_Create_Label();

    // settin the function return label
    func_entry->function_return_label = finishLabel;

    // checking if this function has 3 or 4 parameters, need to mark the global regsiters as used
    if(func_entry->number_of_parameters >= 2){
        // setting r8, which is the fith parameter as inuse
        registersPool.registers[0]->inuse = 1;
    }
    if(func_entry->number_of_parameters == 3){
        // setting r9, which is the sixth parameter as inuse
        registersPool.registers[1]->inuse = 1;
    }

    // calculating the subeed stack size
    stack_sub = LOCAL_VAR_SLOT_SIZE * vars_count + 32;

    
    // checking if stack sub isnt divisable by 16
    if(stack_sub % 16 != 0){
        stack_sub += LOCAL_VAR_SLOT_SIZE;
    }
    
    if(func_name == NULL || func_entry == NULL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }

    // 1) Label & prologue
    fprintf(OUTPUT_FILE, "%s:\n", func_name);
    fprintf(OUTPUT_FILE, "    PUSH RBP\n");
    fprintf(OUTPUT_FILE, "    MOV RBP, RSP\n");
    fprintf(OUTPUT_FILE, "    SUB RSP, %d\n", stack_sub);
    fflush(OUTPUT_FILE);


    // 2) Body
    TreeNode *body = (fnNode->numberOfChildrens == 3)
                      ? fnNode->childrens[2]
                      : fnNode->childrens[3];

    // generating the function body code
    CodeGenerator_GenerateBody(body, globalScope->childrens[func_entry->function_index]);

    fprintf(OUTPUT_FILE, "%s:\n", finishLabel);


    // checking if main function
    if(strcmp(func_entry->name, "main") != 0){
        fprintf(OUTPUT_FILE, "    ADD RSP, %d\n", stack_sub);
            
        //fprintf(OUTPUT_FILE, "    MOV RSP, RBP\n");
        
        fprintf(OUTPUT_FILE, "    pop RBP\n");
        fprintf(OUTPUT_FILE, "    ret\n\n");
    }
    else{
        // if main function, exiting program properly and returning status 0
       
        fprintf(OUTPUT_FILE, "  MOV %s, 0\n", CodeGenerator_GetParameterRegister(0));
        fprintf(OUTPUT_FILE, "  CALL ExitProcess\n");
    }

   
}


void CodeGenerator_GenerateBody(TreeNode* AST, Scope* scope){
    if(AST == NULL || AST->numberOfChildrens != 1 || AST->node_type != NON_TERMINAL || AST->Kind.nonTerminalType != NON_TERMINAL_BODY){
        printf("COMPILER ERRORL NO\n");
        exit(EXIT_FAILURE);
    }
    if(AST->scope == NULL){
        printf("COMPILER ERROR YES");
        exit(EXIT_FAILURE);
    }
    CodeGenerator_GenerateStatements(AST->childrens[0], (Scope*)AST->scope);
}
void CodeGenerator_GenerateStatements(TreeNode* AST, Scope* scope){
    while(AST->numberOfChildrens == 2){
        CodeGenerator_GenerateStatement(AST->childrens[0], scope);

        AST = AST->childrens[1];
    }
    CodeGenerator_GenerateStatement(AST->childrens[0], scope);

}
void CodeGenerator_GenerateStatement(TreeNode* AST, Scope* scope){
    if(AST->node_type != NON_TERMINAL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    switch (AST->childrens[0]->Kind.nonTerminalType)
    {
        case NON_TERMINAL_DECLARATION:
            CodeGenerator_GenerateDeclaration(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_ASSIGNMENT:
            CodeGenerator_GenerateAssignment(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_EXPRESSION:
            CodeGenerator_GenerateExpression(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_IF_STATEMENT:
            //printf("problem start here=========\n");
            CodeGenerator_GenerateIf(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_RETURN:   
            CodeGenerator_GenerateReturn(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_WHILE_LOOP:
            CodeGenerator_GenerateWhile(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_PUTCHAR:
            CodeGenerator_GeneratePutchar(AST->childrens[0], scope);
            break;
        case NON_TERMINAL_GETCHAR:
            CodeGenerator_GenerateGetchar(AST->childrens[0], scope);
            break;

    
    
    default:
        break;
    }

}
void CodeGenerator_GenerateIf(TreeNode* AST, Scope* scope){
    //printf("CodeGenerator_GenerateIf1");
    // define variables
    char* doneLabel = CodeGenerator_Create_Label();
    char* falseLabel = CodeGenerator_Create_Label();
    Register* reg;
    //printf("before");
    // GETTING THE REGISTER HOLDING THE EXPRESSION VALUE
    reg = CodeGenerator_GenerateExpression(AST->childrens[0], scope);
    //printf("here?");
    // comparing to zero
    fprintf(OUTPUT_FILE, "    CMP %s, 0\n", reg->registerName);
    fprintf(OUTPUT_FILE, "    JE %s\n", falseLabel);

    // handling true case body
    CodeGenerator_GenerateBody(AST->childrens[1], scope);
    fprintf(OUTPUT_FILE, "    JMP %s\n", doneLabel);
    fprintf(OUTPUT_FILE, "%s:\n", falseLabel);

    // handling else and else if
    if(AST->numberOfChildrens == 3){
        //printf("inside");
        // handling case of else if, or else
        // checking if it is else if or else
        if(AST->childrens[2]->childrens[0]->node_type == TERMINAL && AST->childrens[2]->childrens[0]->Kind.TerminalType == ELSE_TOKEN &&
            AST->childrens[2]->childrens[1]->node_type == NON_TERMINAL && AST->childrens[2]->childrens[1]->Kind.TerminalType == NON_TERMINAL_IF_STATEMENT){
            // WE ARE IN A ELSE IF
            // calling this function recursivly
            //printf("inside2");
            CodeGenerator_GenerateIf(AST->childrens[2]->childrens[1], scope);

        }
        else if(AST->childrens[2]->childrens[0]->node_type == TERMINAL && AST->childrens[2]->childrens[0]->Kind.TerminalType == ELSE_TOKEN &&
            AST->childrens[2]->childrens[1]->node_type == NON_TERMINAL && AST->childrens[2]->childrens[1]->Kind.TerminalType == NON_TERMINAL_BODY){
                // WE ARE IN ELSE STATE
                //printf("heere");
                CodeGenerator_GenerateBody(AST->childrens[2]->childrens[1], scope);

        }
        
        
    }
    free_register(reg);
    fprintf(OUTPUT_FILE, "%s:\n", doneLabel);


}
void CodeGenerator_GenerateDeclaration(TreeNode* AST, Scope* scope){
    // getting the size of the type of the new var
    //int size = CodeGenerator_GetTypeSize(AST->childrens[0]->childrens[0]);
    int size = 8;
    int flag = 0;
    Scope* func_scope;
    Symbol_Table_Variable_Entry* var_entry;
    Symbol_Table_Function_Entry* func_entry;
    
    // gettin the variable name
    char* var_name = extract_variable_name(AST->childrens[1]);

    // getting the function entry
    func_entry = scope->functionPrototype;
    
    // getting the variable entry
    var_entry = CodeGenerator_FetchVariableEntry(scope, var_name);

    // setting the bp relative of the variable
    var_entry->index_bp_relative = -size *((func_entry->declared_vars++) + 1);

}
void CodeGenerator_GenerateAssignment(TreeNode* AST, Scope* scope){
    // define variables
    Register* reg;
    char* var_name;
    Symbol_Table_Variable_Entry* var_entry;
    

    // firstly, need to evaluate the right side expression
    reg = CodeGenerator_GenerateExpression(AST->childrens[1], scope);
    //printf("alpha");

    // getting the name of the variable
    var_name = extract_variable_name(AST->childrens[0]);
    
    // getting the varaible entry from the scope table
    var_entry = CodeGenerator_FetchVariableEntry(scope, var_name);

    fprintf(OUTPUT_FILE, "    MOV [RBP %d], %s\n", var_entry->index_bp_relative, reg->registerName);

    // freeing the register
    free_register(reg);
}

Register* CodeGenerator_GenerateExpression(TreeNode* AST, Scope* scope){
    Register* temp =  CodeGenerator_GenerateLogicalOr(AST->childrens[0], scope);
    //printf("ret 1\n");
    //if(temp == NULL) printf("null");
    return temp;
}

Register* CodeGenerator_GenerateLogicalOr(TreeNode* AST, Scope* scope){
    //printf("2");
    // define local variables
    Register* regLeft = NULL;
    Register* regRight = NULL;

    char* trueLabel;
    char* falseLabel;

    if(AST->numberOfChildrens == 1){
        regLeft = CodeGenerator_GenerateLogicalAnd(AST->childrens[0], scope);
        
        //printf("ret 2\n");
        
    }
    if(AST->numberOfChildrens == 3){
        trueLabel = CodeGenerator_Create_Label();
        falseLabel = CodeGenerator_Create_Label();
        // handing logical or
        // evaluating logical and and get the return register
        regLeft = CodeGenerator_GenerateLogicalOr(AST->childrens[0], scope);

        fprintf(OUTPUT_FILE, "    CMP %s, 0\n", regLeft->registerName);
        fprintf(OUTPUT_FILE, "    JNE %s\n", trueLabel);

        // evalutgin right side
        regRight = CodeGenerator_GenerateLogicalAnd(AST->childrens[2], scope);

        // do the logical or operation
        //fprintf(OUTPUT_FILE, "    OR %s, %s\n", regLeft->registerName, regRight->registerName);

        


        fprintf(OUTPUT_FILE, "    CMP %s, 0\n", regRight->registerName);
        fprintf(OUTPUT_FILE, "    JNE %s\n", trueLabel);

        fprintf(OUTPUT_FILE, "    MOV %s, 0\n", regRight);
        fprintf(OUTPUT_FILE, "    JMP %s\n", falseLabel);

        fprintf(OUTPUT_FILE, "%s:\n", trueLabel);
        fprintf(OUTPUT_FILE, "    MOV %s, 1\n", regRight);
        fprintf(OUTPUT_FILE, "%s:\n", falseLabel);


        // freeing the right register
        free_register(regRight);

        
    }
    //printf("ret 2\n");
    // returning the left register
    return regLeft;
}
Register* CodeGenerator_GenerateLogicalAnd(TreeNode* AST, Scope* scope){
    //printf("3");
    // define local variables
    Register* regLeft = NULL;
    Register* regRight = NULL;

    char* trueLabel;
    char* falseLabel;

   if(AST->numberOfChildrens == 1){
       regLeft = CodeGenerator_GenerateRelationalTerm(AST->childrens[0], scope);
   
       //printf("ret 3\n");


    }
   if(AST->numberOfChildrens == 3){
        trueLabel = CodeGenerator_Create_Label();
        falseLabel = CodeGenerator_Create_Label();

       // handing logical or
       // evaluating left side
       regLeft = CodeGenerator_GenerateLogicalAnd(AST->childrens[0], scope);

       fprintf(OUTPUT_FILE, "    CMP %s, 0\n", regLeft->registerName);
       fprintf(OUTPUT_FILE, "    JE %s\n", falseLabel);

       // evalutgin right side
       regRight = CodeGenerator_GenerateRelationalTerm(AST->childrens[2], scope);

       fprintf(OUTPUT_FILE, "    CMP %s, 0\n", regRight->registerName);
       fprintf(OUTPUT_FILE, "    JE %s\n", falseLabel);

       // do the logical and operation
       //fprintf(OUTPUT_FILE, "    AND %s, %s\n", regLeft->registerName, regRight->registerName);

       fprintf(OUTPUT_FILE, "    MOV %s, 1\n", regLeft->registerName);
       fprintf(OUTPUT_FILE, "    JMP %s\n", trueLabel);


       fprintf(OUTPUT_FILE, "%s:\n", falseLabel);
       fprintf(OUTPUT_FILE, "    MOV %s, 0\n",regLeft->registerName);

       fprintf(OUTPUT_FILE, "%s:\n", trueLabel);


       // freeing the right register
       free_register(regRight);
    }
    //printf("ret 3\n");
   // returning the left register
   return regLeft;
}
Register* CodeGenerator_GenerateRelationalTerm(TreeNode* AST, Scope* scope){
    //printf("4");
    // define local variables
    // registers
    Register* regLeft = NULL;
    Register* regRight = NULL;
    // labels
    char* trueLabel;
    char* doneLabel;


    if(AST->numberOfChildrens == 1){
        regLeft = CodeGenerator_GenerateAdditiveTerm(AST->childrens[0], scope);
        //printf("ret 4\n");

        
    }
    if(AST->numberOfChildrens == 3){
        //printf("i am do here!\n");
        // evaluate left side
        regLeft = CodeGenerator_GenerateAdditiveTerm(AST->childrens[0], scope);
        //printf("i am still here");
        // evalaute right side
        regRight = CodeGenerator_GenerateAdditiveTerm(AST->childrens[2], scope);
        //printf("still here");
        // getting unique names for the labels
        trueLabel = CodeGenerator_Create_Label();
        doneLabel = CodeGenerator_Create_Label();
        //printf("reg1 is: %s", (regLeft == NULL)? "NULL":"NOT NULL");
        // printf("reg2 is: %s", (regRight == NULL)? "NULL":"NOT NULL");



        //printf("reg1: %d\nreg2: \n", regLeft->registerKind, regRight->registerKind);

        // comparing the two registers
        fprintf(OUTPUT_FILE, "    CMP %s, %s\n", regLeft->registerName, regRight->registerName);

        // do the retional operation
        switch (AST->childrens[1]->Kind.TerminalType)
        {
            case EQUAL_TO_TOKEN:
                fprintf(OUTPUT_FILE, "    JE %s\n", trueLabel);
                break;
            case NOT_EQUAL_TO_TOKEN:
                fprintf(OUTPUT_FILE, "    JNE %s\n", trueLabel);
                break;
            case LESS_THEN_TOKEN:
                fprintf(OUTPUT_FILE, "    JL %s\n", trueLabel);
                break;
            case GREATER_THEN_TOKEN:
                fprintf(OUTPUT_FILE, "    JG %s\n", trueLabel);
                break;
            case LESS_THEN_OR_EQUAL_TO_TOKEN:
                fprintf(OUTPUT_FILE, "    JLE %s\n", trueLabel);
                break;
            case GREATER_THEN_OR_EQUAL_TO_TOKEN:
                fprintf(OUTPUT_FILE, "    JGE %s\n", trueLabel);
                break;
        }

        fprintf(OUTPUT_FILE, "    MOV %s, 0\n", regLeft->registerName);
        fprintf(OUTPUT_FILE, "    JMP %s\n", doneLabel);
        fprintf(OUTPUT_FILE, "%s:\n", trueLabel);
        fprintf(OUTPUT_FILE, "    MOV %s, 1\n", regLeft->registerName);
        fprintf(OUTPUT_FILE, "%s:\n", doneLabel);

        // freeing dynamic strings
        free(trueLabel);
        free(doneLabel);

        // freeing the right register
        free_register(regRight);
    }
    //printf("ret 4");
    // returning the left register
    return regLeft;
}
Register* CodeGenerator_GenerateAdditiveTerm(TreeNode* AST, Scope* scope){
    //printf("5");
    // define local variables
    // registers
    Register* regLeft = NULL;
    Register* regRight = NULL;
   

    if(AST->numberOfChildrens == 1){
        regLeft = CodeGenerator_GenerateMultiplicativeTerm(AST->childrens[0], scope);
        
        //printf("ret 5\n");

    }
    if(AST->numberOfChildrens == 3){
        // evaluate left side
        regLeft = CodeGenerator_GenerateMultiplicativeTerm(AST->childrens[0], scope);

        // evalaute right side
        regRight = CodeGenerator_GenerateAdditiveTerm(AST->childrens[2], scope);

        switch(AST->childrens[1]->Kind.TerminalType){
            case PLUS_TOKEN:
                fprintf(OUTPUT_FILE, "    ADD %s, %s\n", regLeft->registerName, regRight->registerName);
                break;
            case MINUS_TOKEN:
                fprintf(OUTPUT_FILE, "    SUB %s, %s\n", regLeft->registerName, regRight->registerName);
                break;
        }

        // free the right side register
        free_register(regRight);
    }   
    //printf("ret 5\n");
    // returning the left register
    return regLeft;
    
}
Register* CodeGenerator_GenerateMultiplicativeTerm(TreeNode* AST, Scope* scope){
    //printf("6");
    // define local variables
    // registers
    Register* regLeft = NULL;
    Register* regRight = NULL;
    if(AST->numberOfChildrens == 1){
        regLeft = CodeGenerator_GenerateFactor(AST->childrens[0], scope);
        //printf("ret 6\n");
        //printf("%d", regLeft->registerKind);
    }
    if(AST->numberOfChildrens == 3){
        // evaluate left side
        regLeft = CodeGenerator_GenerateFactor(AST->childrens[0], scope);

        // evalaute right side
        regRight = CodeGenerator_GenerateMultiplicativeTerm(AST->childrens[2], scope);

        switch(AST->childrens[1]->Kind.TerminalType){
            case MULTIPLY_TOKEN:
                fprintf(OUTPUT_FILE, "    IMUL %s, %s\n", regLeft->registerName, regRight->registerName);
                break;
            case DIVIDE_TOKEN:
                fprintf(OUTPUT_FILE, "    MOV RDX, 0\n");
                fprintf(OUTPUT_FILE, "    MOV RAX, %s\n", regLeft->registerName);
                fprintf(OUTPUT_FILE, "    IDIV %s\n", regRight->registerName);
                fprintf(OUTPUT_FILE, "    MOV %s, RAX\n", regLeft->registerName);
                break;
            case MODULO_TOKEN:
                fprintf(OUTPUT_FILE, "    MOV RDX, 0\n");
                fprintf(OUTPUT_FILE, "    MOV RAX, %s\n", regLeft->registerName);
                fprintf(OUTPUT_FILE, "    IDIV %s\n", regRight->registerName);
                fprintf(OUTPUT_FILE, "    MOV %s, RDX\n", regLeft->registerName);
                break;
        }

        // freeing the right register
        free_register(regRight);
    }
        // returning the left register
        return regLeft;
    
}
Register* CodeGenerator_GenerateFactor(TreeNode* AST, Scope* scope){
    //printf("7");
    // define variables
    Register* reg = NULL;
    char* trueLabel;
    char* doneLabel;
    //printf("z");
    if(AST->numberOfChildrens == 1){
        //printf("x");
        if(AST->childrens[0]->node_type == NON_TERMINAL){
            switch (AST->childrens[0]->Kind.nonTerminalType)
            {
                case NON_TERMINAL_EXPRESSION:
                    //printf("c");
                    reg =CodeGenerator_GenerateExpression(AST->childrens[0], scope);
                    break;
                case NON_TERMINAL_FUNCTION_CALL:
                    //printf("v");
                    reg = CodeGenerator_GenerateFunctionCall(AST->childrens[0], scope);
                    break;
                case NON_TERMINAL_LITERAL:
                    //printf("b");
                    reg = CodeGenerator_GenerateLiteral(AST->childrens[0], scope);
                    break;
        
            }
        }
        else if(AST->childrens[0]->node_type == TERMINAL){
            //printf("n");
            if(AST->childrens[0]->Kind.TerminalType == IDENTIFIER_TOKEN){
                //printf("m");
                reg = CodeGenerator_GenerateIdentifier(AST->childrens[0], scope);
            }
        }
    }
    if(AST->numberOfChildrens == 2){
        reg = CodeGenerator_GenerateFactor(AST->childrens[1], scope);
        if(AST->childrens[0]->Kind.TerminalType == MINUS_TOKEN){
            fprintf(OUTPUT_FILE, "    NEG %s\n", reg->registerName);
        }
        if(AST->childrens[0]->Kind.TerminalType == LOGICAL_NOT_TOKEN){
            trueLabel = CodeGenerator_Create_Label();
            doneLabel = CodeGenerator_Create_Label();

            fprintf(OUTPUT_FILE, "    CMP %s, 0\n", reg->registerName);
            fprintf(OUTPUT_FILE, "    JE %s\n", trueLabel);
            fprintf(OUTPUT_FILE, "    MOV %s, 0\n", reg->registerName);
            fprintf(OUTPUT_FILE, "    JMP %s\n", doneLabel);
            fprintf(OUTPUT_FILE, "%s:\n", trueLabel);
            fprintf(OUTPUT_FILE, "    MOV %s, 1\n", reg->registerName);
            fprintf(OUTPUT_FILE, "%s:\n", doneLabel);
        }

        
    }
    //printf("try %d", reg->registerKind);
            
    return reg;
}
char CodeGenerator_PushGeneralRegisters(int* c) {
    // this function pushes all the general-purpose registers (r8 - r15) that are currently in use
    char used_mask = 0; // 8-bit mask representing which registers are in use (r8 = LSB, r15 = MSB)
    *c = 0;
    

    int i;
    for(i = 0; i < registersPool.number_of_registers; i++) {
        if(registersPool.registers[i]->inuse) {
            // Set the corresponding bit in the mask
            used_mask |= (1 << i);

            // Emit the PUSH instruction for this register
            fprintf(OUTPUT_FILE, "    PUSH %s\n", registersPool.registers[i]->registerName);

            // setting the register as unuse, since we saving it in the stack
            registersPool.registers[i]->inuse = 0;

            // increasing the counter
            *c += 1;
            
        }
    }

    return used_mask;
}
void CodeGenerator_PopGeneralRegisters(char used_mask) {
    // this function pops all the general-purpose registers (r8 - r15) that were previously pushed,
    // in the reverse order of pushing (from r15 to r8)

    for(int i = 7; i >= 0; i--) {
        if((used_mask >> i) & 1) {
            // Retrieve the register name from the pool and emit POP
            fprintf(OUTPUT_FILE, "    POP %s\n", registersPool.registers[i]->registerName);

            // setting the register as used
            registersPool.registers[i]->inuse = 1;
        }
    }
}

Register* CodeGenerator_GenerateFunctionCall(TreeNode* AST, Scope* scope){
    // define variables
    char pushed_regs;
    int pushed_c;
    char* func_name;
    Symbol_Table_Function_Entry* func_entry;
    //SymbolTable* g_symbol_table;
    Scope* tmp;
    Register* reg = NULL;
    
    // getting the name of the function
    func_name = extract_variable_name(AST->childrens[0]);
    
    // getting the global symbol table
    
    tmp = scope;
    while (tmp->symbol_table->symbol_table_type != GLOBAL_SYMBOL_TABLE)
    {
        tmp = tmp->parent;
    }
    //g_symbol_table = tmp->symbol_table;
    

    // getting the function entry
    func_entry = CodeGenerator_FetchFunctionEntry(tmp->symbol_table, func_name);

    // pushing the registers
    pushed_regs = CodeGenerator_PushGeneralRegisters(&pushed_c);
    //CodeGenerator_PushAllRegs();
    CodeGenerator_PushParamRegis();
    
    // checking if count is off
    if(pushed_c % 2 == 1){
        // keep the stack 16 bytes alignment
        fprintf(OUTPUT_FILE, "    SUB RSP, 8\n");
    }

    // checking if there is params
    if(AST->numberOfChildrens == 2){
        // setting the parameters registers
        CodeGenerator_GenerateFunctionCall_SetParamsRegisters(AST->childrens[1], scope);
    }


    fprintf(OUTPUT_FILE, "    CALL %s\n", func_name);
    
    
    if(pushed_c % 2 == 1){
        // keep the stack 16 bytes alignment
        fprintf(OUTPUT_FILE, "    ADD RSP, 8\n");
    }
    
    CodeGenerator_PopParamRegis();
    CodeGenerator_PopGeneralRegisters(pushed_regs);

    // checking if the function return a value
    if(func_entry->return_type != VARIABLE_AND_RETURN_TYPE_VOID){
        //printf("here10");
        // returning the return value of the function
        reg = allocate_register();
        
        fprintf(OUTPUT_FILE, "    MOV %s, RAX\n", reg->registerName);
    }

    return reg;
}
char* CodeGenerator_GetParameterRegister(int index){
    // this function return the regiter that ack as parameter holder for the [index] place
    
    // cheking that the index is lower them the maximun amount of arguments
    if(index >= 4){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }

    // handling cases
    switch (index)
    {
    case 0:
        return RCX;
    case 1:
        return RDX;
    case 2:
        return R8;
    case 3:
        return R9;
    
    }

    return NULL;
}
void CodeGenerator_GenerateFunctionCall_SetParamsRegisters(TreeNode* AST, Scope* scope){
    // define local variables
    int index = 0;
    TreeNode* tmp;
    Register* reg;

    do
    {
        // gettin ghe current argument
        tmp = AST->childrens[0];

        // evaluating the argument expression
        reg = CodeGenerator_GenerateExpression(tmp->childrens[0], scope);

        // getting the register who hold the parameter in the [index] plase
        fprintf(OUTPUT_FILE, "    MOV %s, %s\n", CodeGenerator_GetParameterRegister(index++), reg->registerName);

        // if there is anoter argument, move to it
        if(AST->numberOfChildrens == 2){
            // iterating to the next argument
            AST = AST->childrens[1];
        }

        free_register(reg);

    }while(AST->numberOfChildrens == 2);
    
}
Register* CodeGenerator_GenerateIdentifier(TreeNode* AST, Scope* scope){
    //printf("8");
    // define varilabes
    Register* reg = NULL;
    
    char* var_name;
    char* lowReg1;
    char* lowReg2;
    Symbol_Table_Variable_Entry* var_entry;

    // getting free register
    reg = allocate_register();

    // getting the variable name
    var_name = extract_variable_name(AST);

    // getting the entry of that variable
    var_entry = CodeGenerator_FetchVariableEntry(scope, var_name);

    // checking if the identifier is from parameter, or localy declared
    if(var_entry->variable_origin == VARIABLE_LOCAL){
        // THIS IS LOCAL DECLARED VARIABLE, ACESSING VIA STACK

        // checking if it is type char or type int
        if(var_entry->variable_type == VARIABLE_AND_RETURN_TYPE_INT){

            fprintf(OUTPUT_FILE, "    MOV %s, [RBP %d]\n", reg->registerName, var_entry->index_bp_relative);
        }
        else if(var_entry->variable_type == VARIABLE_AND_RETURN_TYPE_CHAR){
            // in here, the type is char, so we only care about the lowerest 8 bits

            // getting the name of the low 8 bits register
            CodeGenerator_GetRegisterLowestByteRegister(reg->registerName);

            //fprintf(OUTPUT_FILE, "    XOR %s, %s\n", reg->registerName, reg->registerName);
            //fprintf(OUTPUT_FILE, "    MOV %s, byte [RBP + %d]\n",
            //     CodeGenerator_GetRegisterLowestByteRegister(reg->registerName), var_entry->index_bp_relative);
            fprintf(OUTPUT_FILE, "    MOVZX %s, BYTE [RBP %d]\n", reg->registerName, var_entry->index_bp_relative);

            
        }
    }
    else if(var_entry->variable_origin == VARIABLE_PARAM){
        // this is variable that the function got as a parameter, acessing it via the resired register

        // checking the type of the param
        if(var_entry->variable_type == VARIABLE_AND_RETURN_TYPE_INT){
            fprintf(OUTPUT_FILE, "    MOV %s, %s\n", reg->registerName, CodeGenerator_GetParameterRegister(var_entry->variable_position));
        }
        else if(var_entry->variable_type == VARIABLE_AND_RETURN_TYPE_CHAR){
            
            fprintf(OUTPUT_FILE, "    XOR %s, %s\n", reg->registerName, reg->registerName);
            fprintf(OUTPUT_FILE, "    MOV %s, %s\n", CodeGenerator_GetRegisterLowestByteRegister(reg->registerName), 
            CodeGenerator_GetRegisterLowestByteRegister(CodeGenerator_GetParameterRegister(var_entry->variable_position)));
        }
        
    }


    // returnign the register
    return reg;
}
Register* CodeGenerator_GenerateLiteral(TreeNode* AST, Scope* scope){
    // define local variables
    Register* reg = NULL ;
    int val;

    // getting an unused register
    reg = allocate_register();
    if(AST->childrens[0]->Kind.TerminalType == DIGIT_TOKEN){
        val = *(int*)AST->childrens[0]->value;
        
    }
    if(AST->childrens[0]->Kind.TerminalType == CHAR_LITERAL_TOKEN){
        val = (int)*(char*)AST->childrens[0]->value;
    }

    fprintf(OUTPUT_FILE, "    MOV %s, %d\n", reg->registerName, val);
   
    // returning the register
    return reg;
}

Register* CodeGenerator_GenerateGetchar(TreeNode* AST, Scope* scope){
    // define local variables
    Symbol_Table_Variable_Entry* var_entry;
    char* var_name;
    Register* reg;
    char regis;
    int pushed_c;

    // getting the name of the variable who will hold the goten char
    var_name = extract_variable_name(AST->childrens[0]);

    // getting the variable entry
    var_entry = CodeGenerator_FetchVariableEntry(scope, var_name);

    // saving registers before calling putchar function
    regis = CodeGenerator_PushGeneralRegisters(&pushed_c);
    //CodeGenerator_PushAllRegs();
    CodeGenerator_PushParamRegis();

    
    // checking if count is off
    if(pushed_c % 2 == 1){
        // keep the stack 16 bytes alignment
        fprintf(OUTPUT_FILE, "    SUB RSP, 8\n");
    }


    // getting a free used register
    reg = allocate_register();

    //fprintf(OUTPUT_FILE, "    SUB RSP, 32\n"); // shadow spacing for win64 convention
    fprintf(OUTPUT_FILE, "    CALL getchar\n");
    //fprintf(OUTPUT_FILE, "    ADD RSP, 32\n"); // shadow spacing for win64 convention
    //fprintf(OUTPUT_FILE, "    MOV %s, RAX\n", reg->registerName);
    fprintf(OUTPUT_FILE, "    MOV [RBP %d], %s\n", var_entry->index_bp_relative, RAX);

    // checking if count is off
    if(pushed_c % 2 == 1){
        // keep the stack 16 bytes alignment
        fprintf(OUTPUT_FILE, "    ADD RSP, 8\n");
    }


    // saving th registers
    CodeGenerator_PopParamRegis();
    CodeGenerator_PopGeneralRegisters(regis);

    // returning the register with the input ascii value
    return reg;
    
}
void CodeGenerator_GeneratePutchar(TreeNode* AST, Scope* scope){
    // define loval variables
    Register* reg;
    char regis;
    int pushed_c;

    // saving registers before calling putchar function
    regis = CodeGenerator_PushGeneralRegisters(&pushed_c);
    //CodeGenerator_PushAllRegs();
    CodeGenerator_PushParamRegis();

    
    // checking if count is off
    if(pushed_c % 2 == 1){
        // keep the stack 16 bytes alignment
        fprintf(OUTPUT_FILE, "    SUB RSP, 8\n");
    }

    // evaluating the expresstion to print(lowerst 8 bits);
    reg = CodeGenerator_GenerateExpression(AST->childrens[0], scope);

    fprintf(OUTPUT_FILE, "    MOV %s, %s\n",CodeGenerator_GetParameterRegister(0), reg->registerName);
    //fprintf(OUTPUT_FILE, "    SUB RSP, 32\n"); // shadow spacing for win64 convention
    fprintf(OUTPUT_FILE, "    CALL putchar\n");
    //fprintf(OUTPUT_FILE, "    ADD RSP, 32\n"); // shadow spacing for win64 convention

    // checking if count is off
    if(pushed_c % 2 == 1){
        // keep the stack 16 bytes alignment
        fprintf(OUTPUT_FILE, "    ADD RSP, 8\n");
    }
    //fprintf(OUTPUT_FILE, "    ADD RSP, 64\n");

    // freeing the inuse register
    free_register(reg);

    // saving th registers
    CodeGenerator_PopParamRegis();
    CodeGenerator_PopGeneralRegisters(regis);
}
void CodeGenerator_GenerateReturn(TreeNode* AST, Scope* scope){
    // local variable
    Register* reg;
    Symbol_Table_Function_Entry* func_entry;
    
    // getting the entry of the current function
    func_entry = scope->functionPrototype;
    
    // checking if in here we not retuning any value(returning void)
    if(AST->numberOfChildrens == 0){
        fprintf(OUTPUT_FILE, "    JMP %s\n", func_entry->function_return_label);
    }
    else{
        // evaluating the return expression and getting it in the gotten register
        reg =  CodeGenerator_GenerateExpression(AST->childrens[0], scope);

        fprintf(OUTPUT_FILE, "    MOV RAX, %s\n", reg->registerName);
        fprintf(OUTPUT_FILE, "    JMP %s\n", func_entry->function_return_label);

        free_register(reg);
    }

}
void CodeGenerator_GenerateWhile(TreeNode* AST, Scope* scope){
    // define local variables
    char* whileLabel = CodeGenerator_Create_Label();
    char* doneLabel = CodeGenerator_Create_Label();

    Register* reg;
    // start of the while loop label
    fprintf(OUTPUT_FILE, "%s:\n", whileLabel);

    // evaluate the while expression
    reg = CodeGenerator_GenerateExpression(AST->childrens[0], scope);

    // checking if the expression ivaluate true value
    fprintf(OUTPUT_FILE, "    CMP %s, 0\n", reg->registerName);
    fprintf(OUTPUT_FILE, "    JE %s\n", doneLabel);

    // free reg
    free_register(reg);

    // here is the while scope
    CodeGenerator_GenerateBody(AST->childrens[1], scope);

    // jumping to the begginging of the while loop
    fprintf(OUTPUT_FILE, "    JMP %s\n", whileLabel);

    // the done label
    fprintf(OUTPUT_FILE, "%s:\n", doneLabel);
}

void CodeGenerator_PushParamRegis() {
    // Emit PUSH for each GP register except RSP
    fprintf(OUTPUT_FILE,"    PUSH %s\n", RCX);
    fprintf(OUTPUT_FILE,"    PUSH %s\n", RDX);
    fprintf(OUTPUT_FILE,"    PUSH %s\n", R8);
    fprintf(OUTPUT_FILE,"    PUSH %s\n", R9);
}
void CodeGenerator_PopParamRegis() {
    // Emit PUSH for each GP register except RSP
    fprintf(OUTPUT_FILE,"    POP %s\n", R9);
    fprintf(OUTPUT_FILE,"    POP %s\n", R8);
    fprintf(OUTPUT_FILE,"    POP %s\n", RDX);
    fprintf(OUTPUT_FILE,"    POP %s\n", RCX);
}

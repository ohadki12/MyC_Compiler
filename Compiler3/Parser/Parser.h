
#pragma once
#include <windows.h>
#include"../Token/Token.h"
#include"../ErrorReporting/ErrorReporting.h"
#include<stdlib.h>

// Forward declarations to break dependency loops

// ------------------------------------
// Parser.h structs and costants
// ------------------------------------

#define PARSER_NUMBER_OF_STATES 134
#define PARSER_NUMBER_OF_NON_TERMINALS 40
#define PARSER_NUMBER_OF_TERMINALS 40

typedef enum _non_terminals {
    NON_TERMINAL_THE_PROGRAM,NON_TERMINAL_PROGRAM,NON_TERMINAL_FUNCTIONS,
    NON_TERMINAL_FUNCTION,NON_TERMINAL_ARGUMENTS,NON_TERMINAL_ARGUMENT,
    NON_TERMINAL_PARAMETERS,NON_TERMINAL_PARAMETER,NON_TERMINAL_BODY,
    NON_TERMINAL_STATEMENTS, NON_TERMINAL_STATEMENT,NON_TERMINAL_GETCHAR,
    NON_TERMINAL_PUTCHAR,NON_TERMINAL_DECLARATION,NON_TERMINAL_TYPE,
    NON_TERMINAL_ASSIGNMENT,NON_TERMINAL_RETURN,NON_TERMINAL_FUNCTION_CALL,
    NON_TERMINAL_IF_STATEMENT,NON_TERMINAL_ELSE_IF_OR_ELSE,
    NON_TERMINAL_WHILE_LOOP,NON_TERMINAL_EXPRESSION,
    NON_TERMINAL_LOGICAL_OR_TERM,NON_TERMINAL_LOGICAL_AND_TERM,
    NON_TERMINAL_RELATIONAL_TERM,NON_TERMINAL_ADDITIVE_TERM,
    NON_TERMINAL_MULTIPLICATIVE_TERM,NON_TERMINAL_FACTOR,NON_TERMINAL_LITERAL
} Non_Terminals;

// ------------------------------------
// ParserTable.h structs and costants
// ------------------------------------

typedef enum Action_type { SHIFT, REDUCE, ACCEPT, ERROR_ACTION } ActionType;

typedef struct _Action {
    ActionType typeOfAction;
    int rule_or_state;
} Action;

typedef struct _parse_table {
    Action ActionMatrix[PARSER_NUMBER_OF_STATES][PARSER_NUMBER_OF_TERMINALS];
    int GotoMatrix[PARSER_NUMBER_OF_STATES][PARSER_NUMBER_OF_NON_TERMINALS];
} parse_table;

// productio rules
typedef struct _Production_Rule {
    Non_Terminals Production_Rule_Type;
    int numberOfRightProductions;
} Production_Rule;

typedef struct _parser {
    parse_table parse_table;
    Production_Rule Production_Rules[PARSER_NUMBER_OF_NON_TERMINALS];
} Parser;

// ------------------------------------
// ParserTree.h structs and costants
// ------------------------------------

typedef enum _treeNodeType {
    TERMINAL,
    NON_TERMINAL
} TreeNodeType;

typedef struct _TreeNode {
    // the type of the node
    TreeNodeType node_type;
    union parser {
        Non_Terminals nonTerminalType;
        Terminals TerminalType;
    } Kind;
    
    void* value; // for int, char, identifier terminals

    // for reporting an error
    int SourceCodeLine;  // the line of the error
    int CharacterInSourceCodeLine; // the character of the error

    // pointer to childrens
    struct _TreeNode** childrens;

    // count the number of childrens
    int numberOfChildrens;

    // point into the current scope
    void* scope;
} TreeNode;

// ------------------------------------
// ParserStack.h structs and costants
// ------------------------------------

typedef struct _Stack_Element {
    int state;
    TreeNode *node;
} Stack_Element;

typedef struct _Parser_Stack {
    Stack_Element **arr;
    int size;
    int memorySize;
} Parser_Stack;





// Parser.h function prototypes
Parser* parser_init();
Stack_Element* create_stack_element(TreeNode* tree, int state);
Action parser_get_action(Parser* parser, int CurrentState, Terminals CurrentTerminal);
int parser_get_goto(Parser* parser, int CurrentState, Non_Terminals CnonTerminal);
void parser_add_token_Attr_to_treeNode(TreeNode* tree, Token* token);
int should_add_to_abstact_syntax_tree(Terminals token);
const char* actionTypeToString(ActionType type);
void convertTokenFormat(Token* tokens);
TreeNode* create_abstact_syntax_tree(Parser* parser, Token* tokens);
int countMagnitudedElementsInTheStack(Parser_Stack *stack, int num);

void freeAst(TreeNode* root);
void printAST(TreeNode *node, int indent);


// ------------------------------------
// ParserTree.h contents
// ------------------------------------

// ParserTree function prototypes
void parser_treeNode_addChild(TreeNode *, TreeNode *, int);
TreeNode* parser_treeNode_create_TreeNode();
void parser_treeNode_set_type(TreeNode*, TreeNodeType);
void parser_treeNode_set_terminal(TreeNode* , Terminals );
void parser_treeNode_set_non_terminal(TreeNode*, Non_Terminals);

// ------------------------------------
// ParserStack.h contents
// ------------------------------------


// ParserStack function prototypes
Parser_Stack* parser_stack_createStack();
void parser_stack_push(Parser_Stack *, Stack_Element *);
Stack_Element* parser_stack_pop(Parser_Stack *stack);
Stack_Element* parser_stack_top(Parser_Stack stack);
int parser_stack_isEmpty(Parser_Stack stack);
void parser_stack_destroyStack(Parser_Stack *stack);

// parserTable functions prototype
int get_index_from_terminal_parser(Terminals);
int get_index_from_Non_terminal_parser(Non_Terminals);
void set_parsr_action(Parser*, int, Terminals, Action);
void set_parser_goto(Parser*, int, Non_Terminals, int);
void init_action_table(Parser*);
void init_goto_table(Parser*);
void init_Production_Rules(Parser*);
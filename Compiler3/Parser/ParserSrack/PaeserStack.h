/*#pragma once
#include"../Compiler3/Parser/ParserTree/ParserTree.h"
#include<Windows.h>
// Stack element structure
typedef struct _Stack_Element {
    int state;
    TreeNode *node;
} Stack_Element;

// Stack structure
typedef struct _Parser_Stack {
    Stack_Element **arr;
    int size;
    int memorySize;
} Parser_Stack;

// Function prototypes for Parser
Parser_Stack* parser_stack_createStack();
void parser_stack_push(Parser_Stack *, Stack_Element *);
Stack_Element* parser_stack_pop(Parser_Stack *stack);
Stack_Element* parser_stack_top(Parser_Stack stack);
int parser_stack_isEmpty(Parser_Stack stack);
void parser_stack_destroyStack(Parser_Stack *stack);*/
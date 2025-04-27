//#include"ParserStack.h"
#include"../Parser.h"
// Create a new stack
Parser_Stack* parser_stack_createStack() {
    
    Parser_Stack *stack = (Parser_Stack*) malloc(sizeof(Parser_Stack));
    
    if (!stack) {
        printf("Memory allocation failed! one\n");
        exit(EXIT_FAILURE);
    }
    stack->arr = NULL;  // Initially NULL
    stack->size = 0;    // Stack size
    stack->memorySize = 0;  // Memory allocated for stack elements
    return stack;
}

// Push an element onto the stack
void parser_stack_push(Parser_Stack *stack, Stack_Element *element) {
    // Check if the stack needs more memory (increase memory size)
    if (stack->size >= stack->memorySize) {
        //printf("the stack size: %d the memory size: %d\n    ", stack->size, stack->memorySize);
        
        stack->memorySize = stack->memorySize + 2;
        stack->arr = (Stack_Element**) realloc(stack->arr, stack->memorySize * sizeof(Stack_Element*));
        if (!stack->arr) {
            printf("Memory allocation failed! two\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Add the new element at the top
    stack->arr[stack->size] = element;
    stack->size += 1;
}

// Pop an element from the stack
Stack_Element* parser_stack_pop(Parser_Stack *stack) {
    if (stack->size == 0) {
        printf("Error: Stack underflow!\n");
        exit(EXIT_FAILURE);
    }
    stack->size -= 1;
    Stack_Element* tmp = stack->arr[stack->size];
    stack->arr[stack->size] = NULL;
    return tmp;
}

// Get the top element of the stack without removing it
Stack_Element* parser_stack_top(Parser_Stack stack) {
    if (stack.size == 0) {
        printf("Error: Stack is empty!\n");
        exit(EXIT_FAILURE);
    }
    return (stack.arr[stack.size - 1]);
}

// Check if the stack is empty
int parser_stack_isEmpty(Parser_Stack stack) {
    return stack.size == 0;
}

// Destroy the stack and free memory
void parser_stack_destroyStack(Parser_Stack *stack) {
    // Free each element
    for (int i = 0; i < stack->size; i++) {
        free(stack->arr[i]);
        stack->arr[i] = NULL;
    }
    free(stack->arr);  // Free the array of elements
    stack->arr = NULL;
    free(stack);  // Free the stack structure itself
    stack = NULL;
}

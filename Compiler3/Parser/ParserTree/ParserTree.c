#include"../Parser.h"

void parser_treeNode_addChild(TreeNode *paraent, TreeNode *child, int numberOfChildren){
    //printf("adding children in place: %d out of %d\n",numberOfChildren - paraent->numberOfChildrens - 1, numberOfChildren );
    int index = numberOfChildren - paraent->numberOfChildrens - 1;
    paraent->childrens[index] = child;
    paraent->numberOfChildrens++;    
}   
TreeNode* parser_treeNode_create_TreeNode(){
    // allocating new memory
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    
    // checking malloc sucess
    if(node == NULL){
        printf("MALLOC ERROR");
        //return NULL;
        exit(1);
    }
   
    // setting values to diffault
    node->childrens = NULL;
    node->numberOfChildrens = 0;
    node->value = NULL;
    node->scope = NULL;
    
    // return new node
    return node;
}
void parser_treeNode_set_type(TreeNode* node, TreeNodeType treeType){
    node->node_type = treeType;
}
void parser_treeNode_set_terminal(TreeNode* node, Terminals terminal){
    // setting the type of terminal to the tree node
    // assuming the type is terminal
    node->Kind.TerminalType = terminal;
}
void parser_treeNode_set_non_terminal(TreeNode* node, Non_Terminals nonTerminal){
    // setting the type of the non-terminal 
    // assuming given tree is non terminal type
    node->Kind.nonTerminalType = nonTerminal;
}
#include<xinu.h>
#include<tree.h>


//Function to provide the memory to the node Structure
void tr_createTree(){

    ROOT = getmem(sizeof(struct node));
    ROOT->key = -1;

    nLeaf = getmem(sizeof(struct node));
    nLeaf->key = -1;
    ROOT->left = nLeaf;
    ROOT->right = nLeaf;
 
}
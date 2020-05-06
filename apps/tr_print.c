#include<xinu.h>
#include<tree.h>

// printing value of each node by traversing through the tree
void printTree(struct node *tNode){
	//key value greater than zero to avoid garbage key values 
	if(tNode->key > 0){
	    kprintf("\n Value of the node is %d\t", tNode->key);
		printTree(tNode->left);
		printTree(tNode->right);
	}
}
#include<xinu.h>
#include<tree.h>

struct node *tr_search(int val){

    struct node *tNode;
    tNode = ROOT;

    while( tNode->key != val && tNode->key != -1 ){

        if(val < tNode->key)
        {
            tNode = tNode->left;
        }else
        {
            tNode = tNode->right;
        }
    }
    kprintf("\n Searched value is %d is with colour %d\n\n", tNode->key,tNode->color);
    return tNode;
}
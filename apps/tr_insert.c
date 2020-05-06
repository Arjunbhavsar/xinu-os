#include<xinu.h>
#include<tree.h>

struct node *ROOT;
struct node *nLeaf;

// code is refered from https://en.wikipedia.org/wiki/Red%E2%80%93black_tree

//To maintain the balancing of the tree we need to fix the orientation of the 
//same as we do in AVL tree for balancing
//It's a Right rule fix
void fixRightViolation(struct node *rNode){

    struct node *tempNode;
    tempNode = rNode->left;
    rNode->left = tempNode->right;
    if(tempNode->right->key != -1)
    {
        tempNode->right->parent = rNode;
    }
    tempNode->parent = rNode->parent;

    if( tempNode -> parent -> key == -1)
    {
        ROOT = tempNode;
    }else if(rNode == rNode->parent->left)
    {
        rNode->parent->left = tempNode;
    }else
    {
        rNode->parent->right = tempNode;
    }

    tempNode->right = rNode;

    rNode->parent = tempNode;
}


//To maintain the balancing of the tree we need to fix the orientation of the 
//same as we do in AVL tree for balancing
//It's a left rule fix 
void fixLeftViolation(struct node *lNode){
    struct node *tempNode;

    tempNode = lNode->right;

    lNode->right = tempNode->left;

    if(tempNode->left->key != -1)
    {
        tempNode->left->parent = lNode;
    }

    tempNode->parent = lNode->parent;

    if(tempNode->parent->key == -1)
    {
        ROOT = tempNode;
    }
    else if(lNode == lNode->parent->left)
    {
        lNode->parent->left = tempNode;
    }
    else
    {
        lNode->parent->right = tempNode;
    }

    tempNode->left = lNode;
    lNode->parent = tempNode;

}

//when we Insert or delete the element to or from the tree, We need the crosscheck  
//If the tree is still following the rules of the redBlack tree 
//Some of the rules are given in report
void treeRotationToFixViolation(struct node *nodeToRot){
    while(nodeToRot->parent->color == RED){
        if(nodeToRot->parent->parent->left == nodeToRot->parent)
        {
            if(nodeToRot->parent->parent->right->color == RED)
            {
                nodeToRot->parent->color = BLACK;
                nodeToRot->parent->parent->right->color = BLACK;
                nodeToRot->parent->parent->color = RED;
                nodeToRot = nodeToRot->parent->parent;
            }else
            {
                if(nodeToRot == nodeToRot->parent->right)
                {
                    nodeToRot = nodeToRot->parent;

                    fixLeftViolation(nodeToRot);
                }
                
                nodeToRot->parent->color = BLACK;
                nodeToRot->parent->parent->color = RED;
                
                fixRightViolation(nodeToRot->parent->parent);
            }
        }
        else
        {
            if(nodeToRot->parent->parent->left->color == RED)
            {
                nodeToRot->parent->color = BLACK;
                nodeToRot->parent->parent->color = RED;
                nodeToRot->parent->parent->left->color = BLACK;
                nodeToRot = nodeToRot->parent->parent;

            }else
            {
                if(nodeToRot == nodeToRot->parent->left){

                    nodeToRot = nodeToRot->parent;

                    fixRightViolation(nodeToRot);
                }

                nodeToRot->parent->color = BLACK;
                nodeToRot->parent->parent->color = RED;
                fixLeftViolation(nodeToRot->parent->parent);
            }
        }
    }
    ROOT->color = BLACK;

}

void tr_insert(int val){

    //We will create a temp Node and later we will attach the node to the tree
    struct node *tNode;
    tNode = getmem(sizeof(struct node));
    tNode->key = val;
    tNode->color = RED;
    tNode->left = nLeaf;
    tNode->right = nLeaf;


    struct node *head;
    struct node *sNode;
    head = ROOT;
    sNode = nLeaf;
    //Search the place to insert new value 
    while(head->key != -1){

        sNode = head;
        if(tNode->key <= head->key){
            head = head->left;
        }else{
            head = head->right;
        }
    }

    if(sNode->key == -1){
        ROOT = tNode;
    }
    //If value is less than the node key value, Insert the new value as left child
    else if(tNode->key <= sNode->key){
        sNode->left = tNode;
    }
    //If value to be inserted is greater than the node key value, Insert the new value as right child
    else {
        sNode->right  = tNode;
    }

    tNode->parent = sNode;

    //Once the value is inserted we have the BST,
    //Now our task is to fix the violations of Red black tree rules 
    treeRotationToFixViolation(tNode);

}





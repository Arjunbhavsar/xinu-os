#include<xinu.h>
#include<tree.h>

struct node *ROOT;

// code is refered from https://en.wikipedia.org/wiki/Red%E2%80%93black_tree

//Function to return leftmost element i.e. Smallest element 
struct node *getLeftMostNode(struct node *nodeTodelete){
    while(nodeTodelete->left->key != -1){
        nodeTodelete = nodeTodelete->left;
    }
    return nodeTodelete;
}


//The Tree need to be balanced once the node is deleted 
//so that it follows all the red black tree rules
void fixViolationAfterDelete(struct node *nodeToRot){
    struct node *tNode;
    while(nodeToRot!=ROOT && nodeToRot->color==BLACK)
    {
        if(nodeToRot == nodeToRot->parent->left)
        {
            tNode = nodeToRot->parent->right;
            if(tNode->color == RED)
            {
                tNode->color = BLACK;
                nodeToRot->parent->color = RED;
                fixLeftViolation(nodeToRot->parent);
                tNode = nodeToRot->parent->right;
            }

            if(tNode->left->color == BLACK && tNode->right->color==BLACK)
            {
                tNode->color = RED;
                nodeToRot->parent->color = BLACK;
                nodeToRot = nodeToRot->parent;

            }   
            else
            {
                if(tNode->right->color == BLACK)
                {
                    tNode->color = RED;
                    tNode->left->color = BLACK;
                    fixRightViolation(tNode);
                    tNode = nodeToRot->parent->right;
                
                }

                tNode->color = nodeToRot->parent->color;
                nodeToRot->parent->color = BLACK;
                nodeToRot->right->color = BLACK;
                
                fixLeftViolation(nodeToRot->parent);
                nodeToRot = ROOT;
            }
        }
        else
        {
            tNode = nodeToRot->parent->left;
            if(tNode->color == RED)
            {

                tNode->color = BLACK;
                nodeToRot->parent->color = BLACK;
                fixRightViolation(nodeToRot->parent);
                tNode = nodeToRot->parent->left;

            }
            if(tNode->left->color == BLACK && tNode->right->color == BLACK)
            {

                tNode->color = RED;
                nodeToRot->parent->color = BLACK;
                nodeToRot = nodeToRot->parent;

            }else{
                if(tNode->left->color == BLACK)
                {

                    tNode->color = RED;
                    tNode->right->color = BLACK;
                    fixLeftViolation(tNode);
                    tNode = nodeToRot->parent->left;

                }
                tNode->color = nodeToRot->parent->color;
                nodeToRot->parent->color = BLACK;
                tNode->left->color = BLACK;
                fixRightViolation(nodeToRot->parent);
                nodeToRot = ROOT;
            }
        }
    }

    nodeToRot->color = BLACK;

}

void trans(struct node *nodeTodelete, struct node *deleteChildNode){
    if(nodeTodelete->parent->key == -1){
        ROOT = deleteChildNode;
    }else if(nodeTodelete == nodeTodelete->parent->left){
        nodeTodelete->parent->left = deleteChildNode;
    }else{
        nodeTodelete->parent->right = deleteChildNode;
    }

    deleteChildNode->parent = nodeTodelete->parent;

}

//Function to Delete the Node
void delete(struct node *nodeToDelete)
{
    struct node *tNode;
    struct node *temp;
    int colorTemp;

    temp = nodeToDelete;

    colorTemp = nodeToDelete->color;
    if(nodeToDelete->left->key == -1)
    {
        tNode = nodeToDelete->right;
        trans(nodeToDelete, nodeToDelete->right);
    }else if(nodeToDelete->right->key == -1)
    {
        tNode = nodeToDelete->left;
        trans(nodeToDelete, nodeToDelete->left);
    }else
    {
        temp = getLeftMostNode(nodeToDelete->right);
        colorTemp = temp->color;
        tNode = temp->right;
        if(temp->parent == nodeToDelete)
        {
            tNode->parent = temp;
        }else
        {
            trans(temp, temp->right);
            temp->right = nodeToDelete->right;
            temp->right->parent = temp;
        }

        trans(nodeToDelete, temp);
        temp->left = nodeToDelete->left;
        temp->left->parent = temp;
        temp->color = nodeToDelete->color;

    }

    if(colorTemp == BLACK)
    {
        fixViolationAfterDelete(tNode);
    }

}

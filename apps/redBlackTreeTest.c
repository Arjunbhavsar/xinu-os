#include <tree.h>

struct node *ROOT;

void redBlackTreeTest(int nargs, char *args[]){
    printf("Inside the test function");
    

    //create the tree by calling create tree method and assigning the memory to the tree
    tr_createTree();  
  

    //insert the values in the tree
    tr_insert(7); 
    tr_insert(5);
    tr_insert(15);
    tr_insert(10);
    tr_insert(18);
    tr_insert(20); 
    tr_insert(25);
    tr_insert(150);
    tr_insert(16);
    tr_insert(17);

    //Print the tree after the insertionof elements 
    printTree(ROOT);
    
    //Call search method 
    tr_search(150);

    //Delete the element after searching

    delete(tr_search(15));
    delete(tr_search(18));
    
    // print the values in the tree after deletion
    printTree(ROOT);

}
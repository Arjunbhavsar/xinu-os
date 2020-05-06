#define RED 0
#define BLACK 1

extern struct node{
	int key;
	int color;
	struct node *parent;
	struct node *left;
	struct node *right;
};

extern struct node *ROOT;
extern struct node *nLeaf;

extern void tr_createTree();


//Functions to handle deletion of the node
extern void fixViolationAfterDelete(struct node *nodeToRot);
extern struct node *getLeftMostNode(struct node *deleteNode);
extern void trans(struct node *nodeTodelete, struct node *deleteChildNode);
// Function to delete the Node
extern void delete(struct node *nodeToDel);


//Search the node by Traversing over the tree
extern struct node *tr_search(int val);

//Print the values in the whole tree by traversing 
extern void printTree(struct node *tNode);

// Functions to Fix rotations to maintain the balancing of the tree
extern void treeRotationToFixViolation(struct node *z);
extern void fixRightViolation(struct node * rnode);;
extern void fixLeftViolation(struct node * lnode);

//Function to insert the new value
extern void tr_insert(int val);

//Function to test insert, delete, search and print functions
extern void redBlackTreeTest();
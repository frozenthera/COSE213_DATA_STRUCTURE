#define SHOW_STEP 0 // 제출시 0
#define BALANCING 1 // 제출시 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr);

static NODE *_makeNode( char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	}

	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif		
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif
	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void){
	AVL_TREE *ptr;
	if(ptr = (AVL_TREE *)malloc(sizeof(AVL_TREE))){
		ptr->count = 0;
		ptr->root = NULL;
		return ptr;
	}
	return NULL;
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree){
	_destroy(pTree->root);
	pTree->count = 0;
	free(pTree);
}
static void _destroy( NODE *root){
	if(root == NULL) return;
	free(root->data);
	_destroy(root->left);
	_destroy(root->right);
	root->height = 0;
	free(root);
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data){
	NODE *newPtr;
	if(newPtr = _makeNode(data)){
		pTree->root = _insert(pTree->root, newPtr);
		pTree->count += 1;
		return 1;
	}
	return 0;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr){
	if(BALANCING == 1){
		if(root == NULL) return newPtr;
		if(strcmp(root->data, newPtr->data) > 0) root->left = _insert(root->left, newPtr);
		else root->right = _insert(root->right, newPtr);
		
		root->height = getHeight(root);
		
		int balance = getHeight(root->left) - getHeight(root->right);
		//left of left
		if(balance > 1 && strcmp(root->left->data, newPtr->data) > 0){
			return rotateRight(root);
		}
		//right of left
		else if(balance > 1 && strcmp(root->left->data, newPtr->data) < 0){
			root->left = rotateLeft(root->left);
			return rotateRight(root);
		}
		//right of right
		else if(balance < -1 && strcmp(root->right->data, newPtr->data) < 0){
			return rotateLeft(root);
		}
		//left of right
		else if(balance < -1 && strcmp(root->right->data, newPtr->data) > 0){
			root->right = rotateRight(root->right);
			return rotateLeft(root);
		}
		return root;
	}
	else{
		
		if(root == NULL) return newPtr;
		if(strcmp(root->data, newPtr->data) > 0){
			root->left = _insert(root->left, newPtr);
		}
		else{
			root->right = _insert(root->right, newPtr);
		}
		root->height = getHeight(root);
		return root;
	}
}

static NODE *_makeNode( char *data){
	NODE *ptr = (NODE *)malloc(sizeof(NODE));
	ptr->left = NULL;
	ptr->right = NULL;
	ptr->height = 1;
	ptr->data = strdup(data);
	return ptr;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key){
	NODE *keyPtr = _retrieve(pTree->root, key);
	if(keyPtr == NULL) return NULL;
	return keyPtr->data;
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key){
	if(root == NULL) return NULL;
	if(strcmp(root->data, key) == 0) return root;
	else if(strcmp(root->data, key) > 0) return _retrieve(root->left, key);
	else return _retrieve(root->right, key);
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree){
	_traverse(pTree->root);
}
static void _traverse( NODE *root){
	if(root->left != NULL) _traverse(root->left);
	printf(" %s",root->data);
	if(root->right != NULL) _traverse(root->right);
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree){
	if(pTree->root == NULL) return;
	_infix_print(pTree->root, 0);
}

/* internal traversal function
*/
static void _infix_print( NODE *root, int level){
	if(root->right != NULL) _infix_print(root->right, level+1);
	
	for(int i=0; i<level; i++) printf("\t");
	printf("%s\n", root->data);
	
	if(root->left != NULL) _infix_print(root->left, level+1);
}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root){
	if(root == NULL) return 0;
	int left=0, right=0;
	if(root->left != NULL) left = root->left->height;
	if(root->right != NULL) right = root->right->height;
	return 1 + max(left, right);
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root){
	NODE *newRoot = root->left;
	NODE *temp = root->left->right;
	newRoot->right = root;
	root->left = temp;
	
	root->height = getHeight(root);
	newRoot->height = getHeight(newRoot);
	return newRoot;
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root){
	NODE *newRoot = root->right;
	NODE *temp = newRoot->left;
	newRoot->left = root;
	root->right = temp;
	
	root->height = getHeight(root);
	newRoot->height = getHeight(newRoot);
	return newRoot;
	
}
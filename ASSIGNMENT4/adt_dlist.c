#include <stdlib.h> // malloc

#include "adt_dlist.h"

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){
	NODE *node;
	if(node = (NODE *)malloc(sizeof(NODE))){
		node->dataPtr = dataInPtr;
		//널리스트에 삽입
		if(emptyList(pList) == 1){
			node->llink = node->rlink = NULL;
			pList->head = node;
			pList->rear = node;
		}
		//리스트의 제일 앞에 삽입
		else if(pPre == NULL){
			node->llink = NULL;
			node->rlink = pList->head;
			pList->head->llink = node;
			pList->head = node;
		}
		//리스트 끝에 삽입
		else if(pPre == pList->rear){
			node->rlink = NULL;
			node->llink = pList->rear;
			pList->rear->rlink = node;
			pList->rear = node;
		}
		//리스트 중간에 삽입
		else{
			node->rlink = pPre->rlink;
			pPre->rlink->llink = node;
			pPre->rlink = node;
			node->llink = pPre;
		}
		pList->count += 1;
		return 1;
	}
	return 0;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){
	*dataOutPtr = pLoc->dataPtr;
	if(pLoc == pList->head){
		if(pLoc == pList->rear){
			pList->head = NULL;
			pList->rear = NULL;
		}
		else{
			pList->head = pLoc->rlink;
			pList->head->llink = pPre;
		}
	}
	else if(pLoc == pList->rear){
		pList->rear = pPre;
		pList->rear->rlink = NULL;
	}
	else{
		pPre->rlink = pLoc->rlink;
		pLoc->rlink->llink = pPre;
	}
	free(pLoc);
}

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu){
	NODE *curNode = pList->head;
	while(curNode != NULL){
		if(pList->compare(curNode->dataPtr,pArgu) == 0){ 
			*pPre = curNode->llink;
			*pLoc = curNode;
			return 1;
		}
		curNode = curNode->rlink;
	}
	return 0;
}

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( int (*compare)(const void *, const void *)){
	LIST *ptr = (LIST *)malloc(sizeof(LIST));
	ptr->count = 0;
	ptr->head = NULL;
	ptr->rear = NULL;
	ptr->compare = compare;
	return ptr;
}

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList, void (*callback)(void *)){
	NODE *curNode = pList->head;
	for(int i=0; i<pList->count; i++){
		NODE *temp = curNode->rlink;
		callback(curNode->dataPtr);
		free(curNode);
		curNode = temp;
	}
	free(pList);
}

/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *, const void *)){
	NODE *curNode = pList->head;
	if(curNode == NULL) return _insert(pList,NULL,dataInPtr);
	while(curNode != NULL){
		if(pList->compare(curNode->dataPtr, dataInPtr) == 0) {
			callback(curNode->dataPtr,dataInPtr);
			return 2;
		}
		else if(pList->compare(curNode->dataPtr, dataInPtr) > 0){
			return _insert(pList,curNode->llink,dataInPtr);
		}
		curNode = curNode->rlink;
	}
	return _insert(pList,pList->rear,dataInPtr);
}

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
	NODE *pPre;
	NODE *pLoc;
	if(_search(pList, &pPre, &pLoc, keyPtr) == 0) return 0;
	else{
		_delete(pList,pPre,pLoc,dataOutPtr);
		pList->count--;
		return 1;
	}
}

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, void *pArgu, void **dataOutPtr){
	NODE *pPre;
	NODE *pLoc;
	int temp = _search(pList,&pPre,&pLoc,pArgu);
	if(temp == 0) return 0;
	else{
		*dataOutPtr = pLoc->dataPtr;
		return 1;
	}
}

/* returns number of nodes in list
*/
int countList( LIST *pList){
	return pList->count;
}

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList){
	return pList->count == 0 ? 1 : 0;
}

/* traverses data from list (forward)
*/
void traverseList( LIST *pList, void (*callback)(const void *)){
	NODE *curNode = pList->head;
	for(int i=0; i<pList->count;i++){
		callback(curNode->dataPtr);
		curNode = curNode->rlink;
	}
}

/* traverses data from list (backward)
*/
void traverseListR( LIST *pList, void (*callback)(const void *)){
	NODE *curNode = pList->rear;
	for(int i=0; i<pList->count;i++){
		callback(curNode->dataPtr);
		curNode = curNode->llink;
	}
}
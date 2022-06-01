#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	int index = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{	
		ret = trieInsert( trie, str, index);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
				trieInsert( permute_trie, permuterms[i], index);
		
			clear_permuterms( permuterms, num_p);
			
			dic[index++] = strdup( str);
		}
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			ret = trieSearch( trie, str);
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < index; i++)
		free( dic[i]);
	
	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void){
	TRIE *ptr = (TRIE *)malloc(sizeof(TRIE));
	ptr->index = -1;
	for(int i=0; i<MAX_DEGREE; i++) ptr->subtrees[i] = NULL;
	return ptr;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root){
	if(root == NULL) return;
	for(int i=0; i<MAX_DEGREE; i++){
		trieDestroy(root->subtrees[i]);
	}
	root->index = -1;
	free(root);
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index){
	TRIE *curNode = root;
	for(int i=0; i<strlen(str); i++){
		char cur = str[i];
		if(!(cur == EOW || ('a' <= cur && cur <= 'z') || ('A' <= cur && cur <= 'Z'))) return 0;
		if(cur != EOW) cur = isupper(str[i]) ? tolower(str[i]) : str[i];
		int idx = getIndex(cur);
		if(curNode->subtrees[idx] != NULL) curNode = curNode->subtrees[idx];
		else curNode = curNode->subtrees[idx] = trieCreateNode();
	}
	curNode->index = dic_index;
	return 1;
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str){
	TRIE *curNode = root;
	for(int i=0; i<strlen(str); i++){
		char cur = str[i];
		if(!(cur == EOW || ('a' <= cur && cur <= 'z') || ('A' <= cur && cur <= 'Z'))) return -1;
		if(cur != EOW) cur = isupper(str[i]) ? tolower(str[i]) : str[i];
		int idx = getIndex(cur);
		curNode = curNode->subtrees[idx];
		if(curNode == NULL) return -1;
	}
	return curNode->index;
}

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]){
	if(root == NULL) return;
	if(root->index != -1) printf("%s\n",dic[root->index]);
	for(int i=0; i<MAX_DEGREE; i++){
		trieList(root->subtrees[i], dic);
	}
}

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]){
	TRIE *curNode = root;
	for(int i=0; i<strlen(str); i++){
		char cur = isupper(str[i]) ? tolower(str[i]) : str[i];
		int idx = getIndex(cur);
		if(curNode->subtrees[idx] == NULL) return; 
		curNode = curNode->subtrees[idx];
	}
	trieList(curNode, dic);
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]){
	int size = strlen(str)+1;
	char strcat[size+1];
	strcpy(strcat, str);
	strcat[size-1] = EOW;
	strcat[size] = '\0';
	for(int i=0; i<size; i++){
		char *ptr = (char *)malloc(sizeof(char) * (size+1));
		strcpy(ptr, strcat);
		permuterms[i] = ptr;
		char temp = strcat[0];
		for(int j=0; j<size-1; j++) strcat[j] = strcat[j+1];
		strcat[size-1] = temp;
	}
	return size;
}

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size){
	for(int i=0; i<size; i++) free(permuterms[i]);
}

int strStarCnt(char *str){
	int cnt = 0;
	for(int i=0; i<strlen(str); i++) if(str[i] == '*') cnt++;
	return cnt;
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]){
	int cnt = strStarCnt(str);
	if(cnt > 1){
		//*ab*
		if((str[0] == '*' && str[strlen(str)-1] == '*') && cnt == 2){
			int size = strlen(str);
			char temp[size];
			strcpy(temp, str);
			for(int i=0; i<size-1; i++) temp[i] = temp[i+1];
			temp[size-2] = 0; 
			temp[size-1] = '\0';
			triePrefixList( root, temp, dic);
		}
		else{
			printf("Unavailable Wildcard Pattern!\n");
		}
	}
	//ab*, *ab, a*b
	else{	
		int size = strlen(str);
		char temp[size+2];
		strcpy(temp, str);
		temp[size] = EOW;
		temp[size+1] = '\0';
		while(temp[size] != '*'){
			char first = temp[0];
			for(int j=0; j<size; j++) temp[j] = temp[j+1];
			temp[size] = first;
		}
		temp[size] = 0;
		temp[size+1] = '\0';
		triePrefixList( root, temp, dic);
	}
}


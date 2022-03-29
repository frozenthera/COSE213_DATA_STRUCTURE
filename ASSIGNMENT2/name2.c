#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int start_year, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch를 위한 비교 함수
int compare( const void *n1, const void *n2);

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names()
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}
	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
	FILE *fp;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n\n", argv[0]);
		return 1;
	}

	// 이름 구조체 초기화
	names = create_names();
	
	fp = fopen( argv[1], "r");
	if (!fp)
	{
		fprintf( stderr, "cannot open file : %s\n", argv[1]);
		return 1;
	}

	fprintf( stderr, "Processing [%s]..\n", argv[1]);
		
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose( fp);
	
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);
	
	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}

// 연도별 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int start_year, tNames *names){
	char* fLine = (char *)malloc(100);
	while(!feof( fp)){
		
		int year_index = 0;
		int curFreq = 0;
		tName tempData;
		memset(tempData.freq, 0, sizeof(int) * MAX_YEAR_DURATION);
		
		fgets(fLine, 100 , fp);
		sscanf(fLine,"%d %s %c %d", &year_index, tempData.name, &(tempData.sex), &curFreq);
		
		int idx = binary_search( &tempData, names->data, names->len, sizeof(tName), compare);
		
		//이미 키가 존재할 때
		if(bsearch( &tempData, names->data, names->len, sizeof(tName), compare) != NULL){
			(names->data)[idx].freq[year_index-start_year] = curFreq;
		}
		else{ // idx 위치에 새로 데이터를 끼워넣을 때
			
			//len과 capacity가 같으면 capacity를 두배로 하고 data 배열을 재할당
			if(names->len == names->capacity){
				names->data = (tName *)realloc(names->data,names->capacity * 2 * sizeof(tName));
				names->capacity *= 2;
			}
			
			if(names->len != 0)
				memmove( &(names->data)[idx+1], &(names->data)[idx], sizeof(tName) * ((names->len)-idx));
			
			strcpy((names->data)[idx].name, tempData.name);
			(names->data)[idx].sex = tempData.sex;
			memset((names->data)[idx].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
			(names->data)[idx].freq[year_index-start_year] = curFreq;
			names->len += 1;
		}
	}
	free(fLine);
}

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)){
	
	if(bsearch(key,base,nmemb,size,compare) != NULL) return (bsearch(key,base,nmemb,size,compare)- base) / sizeof(tName) ;
	else{
		for(int i=0; i<nmemb; i++){
			if(compare(key, base+(sizeof(tName) * i)) < 0){
				return i;
			}
		}
	}
	return nmemb;
}

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year){
	
	for(int i = 0; i< names->len ; i++){
		printf("%s\t%c\t", (names->data)[i].name, (names->data)[i].sex);
		printf("%d",(names->data)[i].freq[0]);
		for(int j = 1; j< num_year; j++){
			printf("\t%d",(names->data)[i].freq[j]);
		}
		printf("\n");
	}
}


int compare( const void *n1, const void *n2){
	tName *first = (tName *)n1;
	tName *second = (tName *)n2;
	
	if(strcmp(first->name, second->name) == 0){
		if(first->sex == second->sex) return 0;
		else if(first->sex == 'M'){
			return 1;
		}
		else{
			return -1;
		}
	}
	else{
		return strcmp(first->name, second->name);
	}
}







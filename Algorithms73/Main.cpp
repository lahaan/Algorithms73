#include "stdio.h"
#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"

#pragma warning (disable : 4996)

#define O 3
#define N 25

void PrintObjects(HeaderD* pStruct7) {
	HeaderD* pTempStruct = pStruct7;
	int i = 1, j = 1;

	while (pTempStruct != NULL) {
		// formatting string for printf is "%d) %s %lu %02d:%02d:%02d\n", the result is for example "1) Abcde 100 15:52:07"
		printf("´[nr: %d] %c\n", i,pTempStruct->cBegin);
		Object3* pTempObject = (Object3*)pTempStruct->pObject;
		while (pTempObject != NULL) {
			printf("\t (object %d): %s", j, pTempObject->pID);
			printf("%lu %02d:%02d:%02d", pTempObject->Code, pTempObject->sTime1.Hour, pTempObject->sTime1.Minute, pTempObject->sTime1.Second);
			pTempObject = pTempObject->pNext;
		}
		i++;
	}



	return;
}


int main() {
	HeaderD* pStruct7 = GetStruct7(O, N);
	PrintObjects(pStruct7);
	return 0;
}
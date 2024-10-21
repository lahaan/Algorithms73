#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"

#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"

#pragma warning (disable : 4996)

#define O 3
#define N 25

void PrintObjects(HeaderD* pStruct7) {
	int i = 1, j = 1;
	auto cursor = pStruct7;
	while (cursor != NULL) {
		// formatting string for printf is "%d) %s %lu %02d:%02d:%02d\n", the result is for example "1) Abcde 100 15:52:07"
		printf("´[nr: %d] %c\n", i, cursor->cBegin);
		Object3* pTempObject = (Object3*)cursor->pObject;
		while (pTempObject != NULL) {
			printf("\t (object %d): %s ", j, pTempObject->pID);
			//printf("%lu %02d:%02d:%02d\n", pTempObject->Code, pTempObject->sTime1.Hour, pTempObject->sTime1.Minute, pTempObject->sTime1.Second);
			printf("%lu \n", pTempObject->Code);
			pTempObject = pTempObject->pNext;
			j++;
		}
		i++;
		cursor = cursor->pNext;
	}
}


bool isletters(const char* str) {
	while (*str) {
		if (!isalpha(*str)) {
			return false;
		}
		str++;
	}
	return true;
}

bool lowerAfterFirst(const char* str) {
	if (strlen(str) < 2) {
		return true; // No characters after the first
	}
	str++; // Skip the first character
	while (*str) {
		if (!islower(*str)) {
			return false;
		}
		str++;
	}
	return true;
}

int InsertIntoObject(HeaderD* pStruct7, char* pNewID, int NewCode) {
	Object3* newNode = (Object3*)malloc(sizeof(Object3));
	if (newNode == NULL) {
		return -1;
	}
	newNode->pID = pNewID;
	newNode->Code = NewCode;
	newNode->pNext = NULL;

	printf("obj ID: %c", &newNode->pID);

	Object3* current = (Object3*)pStruct7->pObject;
	Object3* previous = NULL;

	while (current != NULL && strcmp(current->pID, pNewID) < 0) {
		previous = current;
		current = current->pNext;
	}

	if (previous == NULL) {
		// Insert at the beginning
		newNode->pNext = current;
		pStruct7->pObject = newNode;
	}
	else {
		// Insert in the middle or end
		previous->pNext = newNode;
		newNode->pNext = current;
	}

	return 0; // Success
}


int InsertNewObject(HeaderD** pStruct7, char* pNewID, int NewCode) {
	if (!isupper(*pNewID) || !isletters(pNewID) || !lowerAfterFirst(pNewID)) {
		printf("Object is a string of English letters [first letter always capital, rest small]\n");
		return 0;
	}

	auto cursor = *pStruct7;
	while (cursor != NULL) {
		if (*pNewID == cursor->cBegin) {
			break;
		}
		if (*pNewID < cursor->cBegin) {
			HeaderD* newHeader = (HeaderD*)malloc(sizeof(HeaderD));
			newHeader->cBegin = *pNewID;
			newHeader->pPrior = cursor->pPrior;
			newHeader->pNext = cursor;
			cursor->pPrior->pNext = newHeader;
			cursor->pPrior = newHeader;
			InsertIntoObject(*pStruct7, pNewID, NewCode);
			break;
		}
		cursor = cursor->pNext;
	}
	return 1;
}

int main() {
	HeaderD* pStruct7 = GetStruct7(O, N);
	char bruh[] = "Ouck";
	InsertNewObject(&pStruct7, bruh, 1111);
	//PrintObjects(pStruct7);
	return 0;
}
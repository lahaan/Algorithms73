#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"


#pragma warning (disable : 4996)

void setTime(Object3*);

#define O 3
#define N 25

void PrintObjects(HeaderD* pStruct7) {
	int i = 1, j = 1;
	auto cursor = pStruct7;
	while (cursor != NULL) { //parse through headers (if any)
		// formatting string for printf is "%d) %s %lu %02d:%02d:%02d\n", the result is for example "1) Abcde 100 15:52:07"
		printf("�[nr: %d] %c\n", i, cursor->cBegin);
		Object3* pTempObject = (Object3*)cursor->pObject;
		while (pTempObject != NULL) { //parse through objects in header (if any) 
			printf("\t (object %d): %s ", j, pTempObject->pID);
			printf("%lu %02d:%02d:%02d\n", pTempObject->Code, pTempObject->sTime1.Hour, pTempObject->sTime1.Minute, pTempObject->sTime1.Second);
			//printf("%lu \n", pTempObject->Code);
			pTempObject = pTempObject->pNext;
			j++;
		}
		i++;
		cursor = cursor->pNext;
	}
}


bool isletters(const char* str) { //checks if all characters in input are in alphabet
	while (*str) {
		if (!isalpha(*str)) {
			return false;
		}
		str++;
	}
	return true;
}

bool lowerAfterFirst(const char* str) { //checks if letters after the first are lower case
	if (strlen(str) < 2) {
		return true; // No characters after the firstl
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

int InsertIntoObject(HeaderD* pStruct7, char* pNewID, int NewCode) { //when in header, adds an obj/node
	Object3* newNode = (Object3*)malloc(sizeof(Object3));			 //creates a new obj/node w/ pID, Code, pNext & sTime1
	if (newNode == NULL) {
		return -1;
	}
	newNode->pID = pNewID;
	newNode->Code = NewCode;
	newNode->pNext = NULL;
	setTime(newNode);

	Object3* current = (Object3*)pStruct7->pObject;
	Object3* previous = NULL;

	while (current != NULL && strcmp(current->pID, pNewID) < 0) { //relevant pID & find correct spot
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

void setTime(Object3* newNode) { //using time.h gets time of current local systime

	time_t currentTime;
	time(&currentTime);
	struct tm* localTime = localtime(&currentTime);

	newNode->sTime1.Hour = localTime->tm_hour;
	newNode->sTime1.Minute = localTime->tm_min;
	newNode->sTime1.Second = localTime->tm_sec;
}

int InsertNewObject(HeaderD** pStruct7, char* pNewID, int NewCode) {	//1st func - insert a new obj (+ header if needed)
	if (!isupper(*pNewID) || !isletters(pNewID) || !lowerAfterFirst(pNewID)) {
		printf("Object is a string of English letters [first letter always capital, rest small]\n");
		return 0;
	}

	auto cursor = *pStruct7;
	while (cursor != NULL) {	//parse through headers finding the correct placement
		if (*pNewID == cursor->cBegin) {
			InsertIntoObject(cursor, pNewID, NewCode);
			return 1;
		}
		if (*pNewID < cursor->cBegin) {
			HeaderD* newHeader = (HeaderD*)malloc(sizeof(HeaderD));
			newHeader->cBegin = *pNewID;
			newHeader->pPrior = cursor->pPrior;
			newHeader->pNext = cursor;
			newHeader->pObject = NULL; // Initialize the pObject pointer
			if (cursor->pPrior != NULL) {
				cursor->pPrior->pNext = newHeader;
			}
			else {
				*pStruct7 = newHeader; // Update the head of the list
			}
			cursor->pPrior = newHeader;
			InsertIntoObject(newHeader, pNewID, NewCode);
			return 1;
		}
		cursor = cursor->pNext;
	}

	// If we reach here, it means the new header should be added at the end
	HeaderD* newHeader = (HeaderD*)malloc(sizeof(HeaderD));
	newHeader->cBegin = *pNewID;
	newHeader->pPrior = NULL;
	newHeader->pNext = NULL;
	newHeader->pObject = NULL; // Initialize the pObject pointer

	if (*pStruct7 == NULL) {
		*pStruct7 = newHeader;
	}
	else {
		cursor = *pStruct7;
		while (cursor->pNext != NULL) {
			cursor = cursor->pNext;
		}
		cursor->pNext = newHeader;
		newHeader->pPrior = cursor;
	}
	InsertIntoObject(newHeader, pNewID, NewCode);

	return 1;
}

Object3* RemoveExistingObject(HeaderD** pStruct7, char* pExistingID) {
	if (!isupper(*pExistingID) || !isletters(pExistingID) || !lowerAfterFirst(pExistingID)) {
		printf("Object referenced does not exist &or is formatted incorrectly.\n");
		return 0;
	}

}


int main() {
	HeaderD* pStruct7 = GetStruct7(O, N);
	char bruh[] = "Ayz";
	char cuh[] = "Zyadwljhikughfgiuhqawzz";
	InsertNewObject(&pStruct7, bruh, 1111); //a few tests [working 22.10.24]
	InsertNewObject(&pStruct7, cuh, 192929);
	PrintObjects(pStruct7);
	return 0;
}
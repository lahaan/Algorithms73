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
#define N 35

void PrintObjects(HeaderD* pStruct7) {
	int i = 1, j = 1;
	auto cursor = pStruct7;
	while (cursor != NULL) { //parse through headers (if any)
		// formatting string for printf is "%d) %s %lu %02d:%02d:%02d\n", the result is for example "1) Abcde 100 15:52:07"
		printf("¥[nr: %d] %c\n", i, cursor->cBegin);
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


bool isletters(const char* str) {									 //checks if all characters in input are in alphabet
	while (*str) {
		if (!isalpha(*str)) {
			return false;
		}
		str++;
	}
	return true;
}

bool lowerAfterFirst(const char* str) {							     //checks if letters after the first are lower case
	if (strlen(str) < 2) {
		return true;												 //no characters after the firstl
	}
	str++;														     //skip the first character
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

	while (current != NULL && strcmp(current->pID, pNewID) < 0) {    //relevant pID & find correct spot
		previous = current;
		current = current->pNext;
	}

	if (previous == NULL) {											 //insert at the beginning
		newNode->pNext = current;
		pStruct7->pObject = newNode;
	}
	else {															 //insert in the middle or end
		previous->pNext = newNode;
		newNode->pNext = current;
	}

	return 0;
}

void setTime(Object3* newNode) {									 //using time.h gets time of current local systime

	time_t currentTime;
	time(&currentTime);
	struct tm* localTime = localtime(&currentTime);

	newNode->sTime1.Hour = localTime->tm_hour;
	newNode->sTime1.Minute = localTime->tm_min;
	newNode->sTime1.Second = localTime->tm_sec;
}

int InsertNewObject(HeaderD** pStruct7, char* pNewID, int NewCode) {//1st fu	nc - insert a new obj (+ header if needed)
	if (!isupper(*pNewID) || !isletters(pNewID) || !lowerAfterFirst(pNewID)) { //checks if pNewID is "fitting"
		printf("Object is a string of English letters [first letter always capital, rest small]\n");
		return 0;
	}

	auto cursor = *pStruct7;
	while (cursor != NULL) {										//parse through headers finding the correct placement
		if (*pNewID == cursor->cBegin) {
			InsertIntoObject(cursor, pNewID, NewCode);
			return 1;
		}
		if (*pNewID < cursor->cBegin) {
			HeaderD* newHeader = (HeaderD*)malloc(sizeof(HeaderD));
			newHeader->cBegin = *pNewID;
			newHeader->pPrior = cursor->pPrior;
			newHeader->pNext = cursor;
			newHeader->pObject = NULL;								//initialize the pObject pointer
			if (cursor->pPrior != NULL) {
				cursor->pPrior->pNext = newHeader;
			}
			else {
				*pStruct7 = newHeader;								//update the head of the list
			}
			cursor->pPrior = newHeader;
			InsertIntoObject(newHeader, pNewID, NewCode);
			return 1;
		}
		cursor = cursor->pNext;
	}

	HeaderD* newHeader = (HeaderD*)malloc(sizeof(HeaderD));			//last one (end of list)
	newHeader->cBegin = *pNewID;
	newHeader->pPrior = NULL;
	newHeader->pNext = NULL;
	newHeader->pObject = NULL;										//initialize the pObject pointer

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
		printf("Object referenced does not exist or is formatted incorrectly.\n"); //checks if ID that is wanted to remove is valid
		return NULL;
	}

	HeaderD* cursor = *pStruct7;
	HeaderD* prevHeader = NULL;

	while (cursor != NULL) {
		if (cursor->cBegin == pExistingID[0]) {
			Object3* objectToDelete = (Object3*)cursor->pObject;
			Object3* prevObject = NULL;

			while (objectToDelete != NULL && strcmp(objectToDelete->pID, pExistingID) != 0) {
				prevObject = objectToDelete;
				objectToDelete = objectToDelete->pNext;
			}

			if (objectToDelete != NULL) {
				if (prevObject != NULL) {
					prevObject->pNext = objectToDelete->pNext;
				}
				else {
					cursor->pObject = objectToDelete->pNext;
				}

				free(objectToDelete);

				if (cursor->pObject == NULL) {
					if (prevHeader != NULL) {
						prevHeader->pNext = cursor->pNext;
					}
					else {
						*pStruct7 = cursor->pNext;
					}
					if (cursor->pNext != NULL) {
						cursor->pNext->pPrior = prevHeader;
					}
					free(cursor);
				}

				return 0;
			}
		}

		prevHeader = cursor;
		cursor = cursor->pNext;
	}

	printf("Object not found.\n");
	return NULL;
}


Node* createNode(void* pObject) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->pObject = pObject;
	newNode->pLeft = newNode->pRight = NULL;
	return newNode;
}

Node* insertNode(Node* root, void* pObject) {
	Object3* obj = (Object3*)pObject;
	Node* newNode = createNode(pObject);

	if (root == NULL) {
		printf("*Inserted as root node: %s [%d]\n\n", obj->pID, obj->Code);
		return newNode;
	}

	Node* current = root;
	Node* parent = NULL;

	while (current != NULL) {
		parent = current;
		Object3* currentObj = (Object3*)current->pObject;

		if (obj->Code < currentObj->Code) {
			printf("  *Traversing <- from %s (%d)\n", currentObj->pID, currentObj->Code);
			current = current->pLeft;
		}
		else {
			printf("  *Traversing -> from %s (%d)\n", currentObj->pID, currentObj->Code);
			current = current->pRight;
		}
	}

	Object3* parentObj = (Object3*)parent->pObject;
	if (obj->Code < parentObj->Code) {
		parent->pLeft = newNode;
		printf("    *Inserted %s (%d) <- %s (%d)\n\n", obj->pID, obj->Code, parentObj->pID, parentObj->Code);
	}
	else {
		parent->pRight = newNode;
		printf("    *Inserted %s (%d) -> %s (%d)\n\n", obj->pID, obj->Code, parentObj->pID, parentObj->Code);
	}

	return root;
}


Node* CreateBinaryTree(HeaderD* pStruct7) {
	Node* root = NULL;
	HeaderD* currentHeader = pStruct7;

	while (currentHeader != NULL) {
		Object3* currentObject = (Object3*)currentHeader->pObject;
		while (currentObject != NULL) {
			root = insertNode(root, currentObject);
			currentObject = currentObject->pNext;
		}
		currentHeader = currentHeader->pNext;
	}

	return root;
}

void printTree(Node* root) {
	if (root != NULL) {
		printTree(root->pLeft);
		Object3* obj = (Object3*)root->pObject;
		printf("Code: %lu\n", obj->Code);
		printTree(root->pRight);
	}
}

Stack* Push(Stack* pStack, void* pObject) {
	Stack* newNode = (Stack*)malloc(sizeof(Stack));
	newNode->pObject = pObject;
	newNode->pNext = pStack;
	return newNode;
}

Stack* Pop(Stack* pStack, void** pResult) {
	if (!pStack) {
		*pResult = NULL;
		return NULL;
	}
	*pResult = pStack->pObject;
	Stack* nextStack = pStack->pNext;
	free(pStack);
	return nextStack;
}

void TreeTraversal(Node* pTree)
{
	printf("\nPRINTING BINARY TREE:\n\n\n");
	Stack* pStack = NULL;
	Node* p1 = pTree, * p2;
	int nodeCounter = 1;

	if (!pTree) return;

	do
	{
		// Traverse to the leftmost node
		while (p1)
		{
			pStack = Push(pStack, p1);
			p1 = p1->pLeft;
		}

		// Pop from the stack
		pStack = Pop(pStack, (void**)&p2);

		// Process the current node directly here
		if (p2 && p2->pObject)
		{
			Object3* obj = (Object3*)p2->pObject;
			printf("Node %d: %s %lu %02d:%02d:%02d\n",
				nodeCounter++,
				obj->pID,
				obj->Code,
				obj->sTime1.Hour,
				obj->sTime1.Minute,
				obj->sTime1.Second);
		}

		// Move to the right child
		p1 = p2 ? p2->pRight : NULL;

	} while (pStack || p1);
}




/*	ESIMESE OSA TESTIDE KIRJELDUSED
	1. N = 35.
	2. V‰ljastada l‰htestruktuur.
	3. Lisada antud j‰rjekorras objektid identifikaatoritega: Dx, Db, Dz, Dk, Aa, Wu, Wa,
	Zw, Za, wk, Wa, WW, W8, W_  ja v‰ljastada tulemus. Liikme Code v‰‰rtuseks
	vıib valida suvalise postiivse t‰isarvu.  M‰rkus: viie viimase objekti lisamine peab
	andma veateate.
	4. Samas j‰rjekorras eemaldada lisatud objektid ja v‰ljastada muudetud struktuur.
	M‰rkus: viie viimase objekti eemaldamise katse peab andma veateate.
*/


int main() {


	HeaderD* pStruct7 = GetStruct7(O, 10);

	// 4. Create binary tree
	Node* tree = CreateBinaryTree(pStruct7);

	// 5. Traverse and print the binary tree
	printf("\nTREE TRAVERSAL:");
	TreeTraversal(tree);

	printf("\nIn-order Output:\n"); //Unnecessary - just to print out the Codes in order
	printTree(tree);



	return 0;
}


/* 1. OSA TESTID
	HeaderD* pStruct7 = GetStruct7(O, N);
	PrintObjects(pStruct7);	//2. V‰ljastada l‰htestruktuur

	const char* itemIdList[] = { "Dx", "Db", "Dz", "Dk", "Aa", "Wu", "Wa", "Zw", "Za", "wk", "Wa", "WW", "W8", "W_" };
	int itemCodeList[] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113 };
	int size = sizeof(itemIdList) / sizeof(itemIdList[0]);
	for (int i = 0; i < size; i++) {	//3. Lisada objektid + lisada elemendid mis sobivad + veateade + v‰ljastus
		InsertNewObject(&pStruct7, (char*)itemIdList[i], itemCodeList[i]);
	}
	PrintObjects(pStruct7);

	for (int i = 0; i < size; i++) {	//4. Eemaldada lisatud objektid + v‰ljastus
		RemoveExistingObject(&pStruct7, (char*)itemIdList[i]);
	}
	PrintObjects(pStruct7);*/

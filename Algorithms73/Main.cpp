#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "errno.h"

#include "DateTime.h"
#include "Objects.h"
#include "Headers.h"
#include "Structs.h"


#pragma warning (disable : 4996)

void setTime(Object3*);

#define O 3
#define N 35

// ----------------------------------------------------------- 1

void PrintObjects(HeaderD* pStruct7) {
	int i = 1, j = 1;
	auto cursor = pStruct7;
	while (cursor != NULL) { //parse through headers (if any)
		// formatting string for printf is "%d) %s %lu %02d:%02d:%02d\n", the result is for example "1) Abcde 100 15:52:07"
		printf("´[nr: %d] %c\n", i, cursor->cBegin);
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

// ----------------------------------------------------------- 2

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

int InsertNewObject(HeaderD** pStruct7, char* pNewID, int NewCode) {//1st func - insert a new obj (+ header if needed)
	if (!isupper(*pNewID) || !isletters(pNewID) || !lowerAfterFirst(pNewID)) { //checks if pNewID is "fitting"
		printf("ERROR: Object [%s] is formatted incorrectly.\n", pNewID);
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

// ----------------------------------------------------------- 3

Object3* RemoveExistingObject(HeaderD** pStruct7, char* pExistingID) {
	if (!isupper(*pExistingID) || !isletters(pExistingID) || !lowerAfterFirst(pExistingID)) {
		printf("ERROR: Object [%s] referenced does not exist or is formatted incorrectly.\n", pExistingID); //checks if ID that is wanted to remove is valid
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

// 2. Osa
// ----------------------------------------------------------- 4


int compareCodes(const void* pKey, const void* pRecord) {
	return ((Object3*)pKey)->Code - ((Object3*)pRecord)->Code;
}

Node* insertNode(Node* pBinaryTree, void* pObject, int(*pCompare)(const void*, const void*)) {
	Node* newNode = (Node*)malloc(sizeof(Node));				// New node
	if (!newNode) {
		printf("Error: malloc failed!\n");
		exit(EXIT_FAILURE);
	}
	newNode->pObject = pObject;
	newNode->pLeft = newNode->pRight = NULL;

	if (!pBinaryTree) {
		printf("Inserted as root node: %s [%d]\n",
			((Object3*)pObject)->pID,
			((Object3*)pObject)->Code);
		return newNode;											// Tree was empty
	}

	for (Node* current = pBinaryTree; 1;) {
		if (pCompare(pObject, current->pObject) < 0) {
			if (!current->pLeft) {								// Empty spot found
				current->pLeft = newNode;
				printf("Inserted to the left of %s [%d]: %s [%d]\n",
					((Object3*)current->pObject)->pID,
					((Object3*)current->pObject)->Code,
					((Object3*)pObject)->pID,
					((Object3*)pObject)->Code);
				return pBinaryTree;								// Inserting left
			}
			else {
				current = current->pLeft;						// Moving left
			}
		}
		else {
			if (!current->pRight) {								// Empty spot found
				current->pRight = newNode;
				printf("Inserted to the right of %s [%d]: %s [%d]\n",
					((Object3*)current->pObject)->pID,
					((Object3*)current->pObject)->Code,
					((Object3*)pObject)->pID,
					((Object3*)pObject)->Code);
				return pBinaryTree;								// Inserting right
			}
			else {
				current = current->pRight;						// Moving right
			}
		}
	}
}


Node* CreateBinaryTree(HeaderD* pStruct7) {
	printf("\n\n\nCREATING BINARY TREE\n\nWhere pID's are put in place with the key: Code\n\n\n");
	Node* pBinaryTree = NULL;
	HeaderD* pStructTemp = pStruct7;

	while (pStructTemp) { //traverse through nodes
		Object3* currentObject = (Object3*)pStructTemp->pObject;
		while (currentObject) {
			pBinaryTree = insertNode(pBinaryTree, currentObject, compareCodes); //insert a node to binary tree
			currentObject = currentObject->pNext; //set p
		}
		pStructTemp = pStructTemp->pNext;
	}

	return pBinaryTree;
}

// ----------------------------------------------------------- 5

Stack* Push(Stack* pStack, void* pObject) {
	if (!pObject) {
		errno = EINVAL;
		return pStack;
	}

	Stack* newNode = (Stack*)malloc(sizeof(Stack));
	if (!newNode) {
		printf("Push(): malloc failed!\n");
		exit(EXIT_FAILURE);
	}

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

void TreeTraversal(Node* pTree) {
	printf("\n\n\nTRAVERSING BINARY TREE (LEFT-ROOT-RIGHT: IN ORDER)\n\n\n");
	Stack* pStack = NULL;
	Node* p1 = pTree;
	Node* p2;
	int i = 1;

	if (!pTree) {	// checking if the tree is empy or not
		printf("ERROR: Tree is empty\n");
		return;
	}

	do {
		while (p1) {
			pStack = Push(pStack, p1);
			p1 = p1->pLeft;
		}
		pStack = Pop(pStack, (void**)&p2);
		Object3* obj = (Object3*)p2->pObject;
		printf("Node %d: %s [%lu]\n", i++, obj->pID, obj->Code);
		p1 = p2->pRight;
	} while (!(!pStack && !p1));
}

// ----------------------------------------------------------- 6

Node* DeleteTreeNode(Node* pTree, unsigned long int Code) {
	printf("\n\nTRYING TO DELETE: %d\n", Code);

	Node* parent = NULL;
	Node* current = pTree;

	if (!pTree) {	// checking if the tree is empy or not
		printf("ERROR: Tree is empty\n");
		return NULL;
	}

	// finding the child to delete & parent 
	while (current && ((Object3*)current->pObject)->Code != Code) { // as long as pTree exists and traversed node isn't equiv. to Code
		parent = current;
		if (Code < ((Object3*)current->pObject)->Code) {	
			current = current->pLeft;
		}
		else {
			current = current->pRight;
		}
	}

	if (!current) {		// checking node's (code) existence 
		printf("ERROR: Code [%lu] not found in the tree.\n", Code);
		return pTree;
	}

	// Deletion

	// 1. Node to be deleted has no children 
	if (!current->pLeft && !current->pRight) {	// no left or right child
		if (!parent) {	//is it a root node
			// deleting the node with no children
			printf("Deleting the node: %s (leaf node)\n", ((Object3*)current->pObject)->pID);
			pTree = NULL;
		}

		else if (parent->pLeft == current) { //correct pointer
			parent->pLeft = NULL;
		}

		else {
			parent->pRight = NULL;
		}

		printf("Deleting node: %s (leaf node)\n", ((Object3*)current->pObject)->pID);
		free(current->pObject);		//delete pointer to node
		free(current);				//delete node
	}

	// 2. Node to be deleted that has only one child (left or right)
	else if (!current->pLeft || !current->pRight) {	//
		Node* child = current->pLeft ? current->pLeft : current->pRight;

		if (!parent) {
			// Deleting node with one child
			printf("Deleting the root node: %s (single child)\n", ((Object3*)current->pObject)->pID);
			pTree = child;
		}

		else if (parent->pLeft == current) { //correct pointer
			parent->pLeft = child;
		}

		else {
			parent->pRight = child;
		}

		printf("Deleting node: %s (single child)\n", ((Object3*)current->pObject)->pID);
		free(current->pObject);		//delete pointer to node
		free(current);				//delete node
	}

	// 3. Node to be deleted that has two children
	else {
		Node* successorParent = current;
		Node* successor = current->pRight;

		while (successor->pLeft) { //get successor parent (in order)
			successorParent = successor;
			successor = successor->pLeft;
		}

		// replace current node's object with the successor's object
		printf("Replacing node: %s with node: %s [NEW]\n",
			((Object3*)current->pObject)->pID, ((Object3*)successor->pObject)->pID);
		free(current->pObject);		// delete current node obj
		current->pObject = malloc(sizeof(Object3));	// new memory for successor
		memcpy(current->pObject, successor->pObject, sizeof(Object3)); //copy successor obj to current

		// remove in-order successor
		if (successorParent->pLeft == successor) { //check if a left-child & link right child to parent
			successorParent->pLeft = successor->pRight;
		}
		else {
			successorParent->pRight = successor->pRight;
		}

		printf("Deleting in-order successor data: %s [OLD]\n", ((Object3*)successor->pObject)->pID);
		free(successor->pObject);
		free(successor);
	}

	return pTree;
}


//recursive traversal
void printTree(Node* root) {
	if (root != NULL) {
		printTree(root->pLeft);
		Object3* obj = (Object3*)root->pObject;
		printf("Code: %lu\n", obj->Code);
		printTree(root->pRight);
	}
}




/*	ESIMESE OSA TESTIDE KIRJELDUSED
	1. N = 35.
	2. Väljastada lähtestruktuur.
	3. Lisada antud järjekorras objektid identifikaatoritega: Dx, Db, Dz, Dk, Aa, Wu, Wa,
	Zw, Za, wk, Wa, WW, W8, W_  ja väljastada tulemus. Liikme Code väärtuseks
	võib valida suvalise postiivse täisarvu.  Märkus: viie viimase objekti lisamine peab
	andma veateate.
	4. Samas järjekorras eemaldada lisatud objektid ja väljastada muudetud struktuur.
	Märkus: viie viimase objekti eemaldamise katse peab andma veateate.
*/
/*
	Teise osa kontrolltestid:
	1. N=35.
	2. Väljastada lähtestruktuur.
	46
	3. Moodustada kahendpuu ja käies läbi kõik tema tipud väljastada tippude juurde
	kuuluvad objektid.
	4. Eemaldada puu juurtipp ja käies läbi kõik uue puu tipud väljastada tippude juurde
	kuuluvad objektid.
	5. N=10.
	6. Väljastada lähtestruktuur.
	7. Moodustada kahendpuu ja käies läbi kõik tema tipud väljastada tippude juurde
	kuuluvad objektid.
	8. Joonistada saadud kahendpuu paberile, märkides tippude juurde üksnes nende
	võtmed.
	9. Eemaldada kahendpuust juhendaja näidatud võtmetega tipud ja käies läbi kõik
	uue puu tipud väljastada tippude juurde kuuluvad objektid.
	10. Teha katse eemaldada tipp, mida ei olegi olemas.
*/


int main() {
	printf("\n###################### ESIMENE OSA ######################\n");

	HeaderD* pStruct7 = GetStruct7(O, N);
	PrintObjects(pStruct7);	//2. Väljastada lähtestruktuur

	const char* itemIdList[] = { "Dx", "Db", "Dz", "Dk", "Aa", "Wu", "Wa", "Zw", "Za", "wk", "Wa", "WW", "W8", "W_" };
	int itemCodeList[] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113 };
	int size = sizeof(itemIdList) / sizeof(itemIdList[0]);
	for (int i = 0; i < size; i++) {	//3. Lisada objektid + lisada elemendid mis sobivad + veateade + väljastus
		InsertNewObject(&pStruct7, (char*)itemIdList[i], itemCodeList[i]);
	}
	PrintObjects(pStruct7);

	for (int i = 0; i < size; i++) {	//4. Eemaldada lisatud objektid + väljastus
		RemoveExistingObject(&pStruct7, (char*)itemIdList[i]);
	}

	printf("\n###################### TEINE OSA ######################\n");
	// 1. N = 35
	pStruct7 = GetStruct7(O, N);
	// 2. Väljastada lähtestruktuur.
	PrintObjects(pStruct7);
	// 3. Moodustada kahendpuu ja käies läbi kõik tema tipud väljastada tippude juurde kuuluvad objektid
	Node* pBinaryTree = CreateBinaryTree(pStruct7);
	TreeTraversal(pBinaryTree);
	// 4. Eemaldada  puu  juurtipp  ja  käies  läbi  kõik  uue  puu  tipud  väljastada  tippude juurde kuuluvad objektid.
	pBinaryTree = DeleteTreeNode(pBinaryTree, 316985719);
	TreeTraversal(pBinaryTree);
	// 5. N = 10
	pStruct7 = GetStruct7(O, 10);
	// 6. Väljastada lähtestruktuur.
	printf("SHOULD BE PRINTIN O=10\n");
	PrintObjects(pStruct7);
	// 7. Moodustada kahendpuu ja käies läbi kõik tema tipud väljastada tippude juurde kuuluvad objektid.
	pBinaryTree = CreateBinaryTree(pStruct7);
	TreeTraversal(pBinaryTree);
	// 8. Joonistada saadud kahendpuu paberile, märkides tippude juurde üksnes nende võtmed.
	// 9. Eemaldada kahendpuust juhendaja näidatud võtmetega tipud ja käies läbi kõik uue puu tipud väljastada tippude juurde kuuluvad objektid.
	pBinaryTree = DeleteTreeNode(pBinaryTree, 258186307);
	TreeTraversal(pBinaryTree);
	// 10. Teha katse eemaldada tipp, mida ei olegi olemas.
	pBinaryTree = DeleteTreeNode(pBinaryTree, 1234);
	TreeTraversal(pBinaryTree);



	return 0;
}


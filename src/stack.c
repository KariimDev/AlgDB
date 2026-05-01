#include "../include/types.h"
#include "../include/linkedlist.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

TStack* push(struct TStack* stk, const char* name, const char* definition, const char* dob, const char* dod){
    TStack* newNode;
    newNode = (TStack*)malloc(sizeof(TStack));
    
    if (newNode == NULL) {
        return NULL; 
    }
    
    if(name != NULL){
        strcpy(newNode->name, name);
    } else {
        newNode->name[0] = '\0';
    }
    
    if(definition != NULL){
        strcpy(newNode->definition, definition);
    } else {
        newNode->definition[0] = '\0';
    }
    
    if(dob != NULL){
        strcpy(newNode->DoB, dob);
    } else {
        newNode->DoB[0] = '\0';
    }
    
    if(dod != NULL){
        strcpy(newNode->DoD, dod);
    } else {
        newNode->DoD[0] = '\0';
    }

    newNode->next = stk;
    return newNode;
}

struct TStack* pop(struct TStack* stk, struct TStack* poppedData){
    TStack* temp;

    if (stk == NULL) {
        return NULL; 
    }

    if (poppedData != NULL) {
        strcpy(poppedData->name, stk->name);
        strcpy(poppedData->definition, stk->definition);
        strcpy(poppedData->DoB, stk->DoB);
        strcpy(poppedData->DoD, stk->DoD);
    }

    temp = stk;
    stk = stk->next;
    free(temp);
    
    return stk;
}

struct TStack* peek(struct TStack* stk) {
    return stk;
}

bool isEmpty(struct TStack* stk) {
    return stk == NULL;
}

TStack* toStack(struct TList *merged) {
    TStack* stk;
    TList* curr;
    
    stk = NULL;
    curr = merged;

    while (curr != NULL) {
        stk = push(stk, curr->name, curr->definition, curr->DoB, curr->DoD);
        curr = curr->next;
    }
    
    return stk;
}

struct TStack* getInfoPersonality(struct TStack *stk, char *name){
    TStack* curr;
    curr = stk;
    
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            return curr; 
        }
        curr = curr->next;
    }
    return NULL;
}

void stackSwapData(struct TStack* a, struct TStack* b) {
    char tempName[MAX_NAME], tempDef[MAX_DEF], tempDoB[MAX_DATE], tempDoD[MAX_DATE];
    
    strcpy(tempName, a->name); 
    strcpy(tempDef, a->definition); 
    strcpy(tempDoB, a->DoB); 
    strcpy(tempDoD, a->DoD);
    
    strcpy(a->name, b->name); 
    strcpy(a->definition, b->definition); 
    strcpy(a->DoB, b->DoB); 
    strcpy(a->DoD, b->DoD);
    
    strcpy(b->name, tempName); 
    strcpy(b->definition, tempDef); 
    strcpy(b->DoB, tempDoB); 
    strcpy(b->DoD, tempDoD);
}

struct TStack* sortNameStack(struct TStack *s){
    TStack* sorted;
    TStack* curr;
    TStack* i;
    TStack* j;

    sorted = NULL;
    curr = s;

    if (s == NULL) return NULL;
    
    while (curr != NULL) {
        sorted = push(sorted, curr->name, curr->definition, curr->DoB, curr->DoD);
        curr = curr->next;
    }
    
    for (i = sorted; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            /* strcmp(i->name, j->name) > 0 means i->name is greater than j->name, so we swap them */
            if (strcmp(i->name, j->name) > 0) {
                stackSwapData(i, j);
            }
        }
    }
    return sorted;
}

struct TStack* deleteName(struct TStack *stk, char *name) {
    struct TStack temp;
    struct TStack* tempStack;

    tempStack = NULL;
    
    while (!isEmpty(stk)) {
        stk = pop(stk, &temp);
        if (strcmp(temp.name, name) == 0) {
            break; 
        }
        tempStack = push(tempStack, temp.name, temp.definition, temp.DoB, temp.DoD);
    }
    
    while (!isEmpty(tempStack)) {
        tempStack = pop(tempStack, &temp);
        stk = push(stk, temp.name, temp.definition, temp.DoB, temp.DoD);
    }
    
    return stk;
}

TStack* updateStack(TStack *stk, char *name, char *def, char *DoB, char *DoD){
    TStack* curr;
    curr = stk;
    
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            if(def != NULL){
                strcpy(curr->definition, def);
            }
            if(DoB != NULL){
                strcpy(curr->DoB, DoB);
            }
            if(DoD != NULL){
                strcpy(curr->DoD, DoD);
            }
            break; 
        }
        curr = curr->next;
    }
    return stk;
}

/* In the stack use only pop and push and in the queue use only enqueue and dequeue */
TQueue* stackToQueue(TStack *stk){
    TQueue* queue;
    TStack* curr;
    TList* newNode;

    queue = (TQueue*)malloc(sizeof(TQueue));
    if (queue == NULL) {
        return NULL; 
    }
    queue->front = NULL;
    queue->rear = NULL;

    curr = stk;
    while (curr != NULL) {
        newNode = (TList*)malloc(sizeof(TList));
        if (newNode == NULL) {
            return NULL; 
        }
        strcpy(newNode->name, curr->name);
        strcpy(newNode->definition, curr->definition);
        strcpy(newNode->DoB, curr->DoB);
        strcpy(newNode->DoD, curr->DoD);
        newNode->next = NULL;
        newNode->prev = NULL;

        if (queue->rear == NULL) {
            queue->front = newNode;
            queue->rear = newNode;
        } else {
            queue->rear->next = newNode;
            newNode->prev = queue->rear;
            queue->rear = newNode;
        }
        curr = curr->next;
    }
    return queue;
}

/* In the stack use only pop and push and in the queue use only enqueue and dequeue */
TList* stackToList(TStack *stk){
    TList* head;
    TList* tail;
    TStack* curr;
    TList* newNode;

    head = NULL;
    tail = NULL;
    curr = stk;

    while (curr != NULL) {
        newNode = (TList*)malloc(sizeof(TList));
        if (newNode == NULL) {
            return NULL; 
        }
        strcpy(newNode->name, curr->name);
        strcpy(newNode->definition, curr->definition);
        strcpy(newNode->DoB, curr->DoB);
        strcpy(newNode->DoD, curr->DoD);
        newNode->next = NULL;
        newNode->prev = NULL;

        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        curr = curr->next;
    }
    return head;
}

/* this function adds a personality name with definition and dates into a sorted stack. */
TStack* addNameStack(TStack *stk, char *name, char *definition, char *DoB, char *DoD){
    TStack* tempStack;
    struct TStack temp;

    tempStack = NULL;

    while (!isEmpty(stk)) {
        stk = pop(stk, &temp);
        if (strcmp(temp.name, name) > 0) {
            tempStack = push(tempStack, temp.name, temp.definition, temp.DoB, temp.DoD);
        } else {
            stk = push(stk, temp.name, temp.definition, temp.DoB, temp.DoD);
            break; 
        }
    }
    
    stk = push(stk, name, definition, DoB, DoD);
    
    while (!isEmpty(tempStack)) {
        tempStack = pop(tempStack, &temp);
        stk = push(stk, temp.name, temp.definition, temp.DoB, temp.DoD);
    }
    
    return stk;
}

int countword(char* str) {
    int count;
    bool Word;

    count = 0;
    Word = false;

    while (*str) {
        if (*str == ' ') {
            Word = false;
        } else if (!Word) {
            Word = true;
            count++;
        }
        str++;
    }
    return count;
}

/* this function sorts the personality names according to the number of words in their definition */
TStack* definitionStack(TStack *stk){
    struct TStack* tempStack;
    struct TStack currentData;
    struct TStack topTempData;
    int currentWords;

    tempStack = NULL;
    
    while (!isEmpty(stk)) {
        stk = pop(stk, &currentData);
        currentWords = countword(currentData.definition);
        
        while (!isEmpty(tempStack)) {
            tempStack = pop(tempStack, &topTempData);
            if (countword(topTempData.definition) > currentWords) {
                stk = push(stk, topTempData.name, topTempData.definition, topTempData.DoB, topTempData.DoD);
            } else {
                tempStack = push(tempStack, topTempData.name, topTempData.definition, topTempData.DoB, topTempData.DoD);
                break;
            }
        }

        tempStack = push(tempStack, currentData.name, currentData.definition, currentData.DoB, currentData.DoD);
    }
    
    while (!isEmpty(tempStack)) {
        tempStack = pop(tempStack, &currentData);
        stk = push(stk, currentData.name, currentData.definition, currentData.DoB, currentData.DoD);
    }
    
    return stk;
}

/* Sorts the events into short (< 10 words) or long (>= 10 words) */
struct TStack* pronunciationStack(struct TStack *stk) {
    struct TStack* shortStack;
    struct TStack* longStack;
    struct TStack temp;

    shortStack = NULL;
    longStack = NULL;
    
    while (!isEmpty(stk)) {
        stk = pop(stk, &temp);
        if (countword(temp.definition) < 10) {
            shortStack = push(shortStack, temp.name, temp.definition, temp.DoB, temp.DoD);
        } else {
            longStack = push(longStack, temp.name, temp.definition, temp.DoB, temp.DoD);
        }
    }
    
    while (!isEmpty(longStack)) {
        longStack = pop(longStack, &temp);
        stk = push(stk, temp.name, temp.definition, temp.DoB, temp.DoD);
    }
    while (!isEmpty(shortStack)) {
        shortStack = pop(shortStack, &temp);
        stk = push(stk, temp.name, temp.definition, temp.DoB, temp.DoD);
    }
    
    return stk;
}

char* getSmallest(TStack *stk){
    /* use only pop and push to find the smallest name in the stack */
    struct TStack temp;
    char* smallest;

    smallest = NULL;

    while (!isEmpty(stk)) {
        stk = pop(stk, &temp);
        if (smallest == NULL || strcmp(temp.name, smallest) < 0) {
            smallest = temp.name;   
        }
    }
    return smallest;    
}

bool isPersonalityKilled(char *definition) {
    if (definition == NULL) return false;

    if (strstr(definition, "killed") != NULL || strstr(definition, "assassinated") != NULL || 
        strstr(definition, "executed") != NULL || strstr(definition, "murdered") != NULL ||
        strstr(definition, "KILLED") != NULL || strstr(definition, "ASSASSINATED") != NULL || 
        strstr(definition, "EXECUTED") != NULL || strstr(definition, "MURDERED") != NULL) {
        return true;
    }
    return false;
}

void insertAtend(struct TStack **stk, struct TStack data) {
    struct TStack temp;

    if (isEmpty(*stk)) {
        *stk = push(*stk, data.name, data.definition, data.DoB, data.DoD);
        return;
    }
    
    *stk = pop(*stk, &temp);           
    insertAtend(stk, data);       
    *stk = push(*stk, temp.name, temp.definition, temp.DoB, temp.DoD); 
}       

struct TStack* recRevStack(struct TStack *stk) {
    struct TStack temp;

    if (!isEmpty(stk)) {
        stk = pop(stk, &temp);         
        stk = recRevStack(stk);       
        insertAtend(&stk, temp);   
    }
    return stk;
}

/* Helper to build a stack from file for wrappers */
TStack* buildStackFromFile(char *filePath) {
    FILE *file;
    TList *s;
    TList *a;
    TList *merged;
    TStack *stk;

    file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }

    s = getPersonality(file);
    rewind(file);
    a = getDatePersonality(file);
    merged = mergeNodes(s, a);
    stk = toStack(merged);
    
    fclose(file);
    return stk;
}

void _toStack() {
    char filePath[100];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    stk = buildStackFromFile(filePath);
    if (stk != NULL) {
        printf("Successfully converted merged data to a new stack.\n");
    }
}

void _getInfoPersonality() {
    char filePath[100];
    char name[100];
    TStack *stk;
    TStack *result;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    printf("Enter name to search: ");
    scanf(" %99[^\n]", name);
    
    result = getInfoPersonality(stk, name);
    if (result != NULL) {
        printf("Personality Found:\n");
        printf("Name: %s\n", result->name);
        printf("Definition: %s\n", result->definition);
        printf("Born: %s, Died: %s\n", result->DoB, result->DoD);
    } else {
        printf("Error: Personality '%s' not found in stack.\n", name);
    }
}

void _sortNameStack() {
    char filePath[100];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    stk = sortNameStack(stk);
    printf("Stack successfully sorted by name.\n");
}

void _deleteName() {
    char filePath[100];
    char name[100];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    printf("Enter name to delete: ");
    scanf(" %99[^\n]", name);
    stk = deleteName(stk, name);
    printf("Personality '%s' removed from stack (if it existed).\n", name);
}

void _updateStack() {
    char filePath[100];
    char name[100], def[500], dob[20], dod[20];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    printf("Enter name of personality to update: ");
    scanf(" %99[^\n]", name);
    printf("Enter new definition: ");
    scanf(" %499[^\n]", def);
    printf("Enter new Date of Birth (DD-MM-YYYY): ");
    scanf(" %19[^\n]", dob);
    printf("Enter new Date of Death (DD-MM-YYYY or N/A): ");
    scanf(" %19[^\n]", dod);
    
    stk = updateStack(stk, name, def, dob, dod);
    printf("Personality '%s' updated in stack (if it existed).\n", name);
}

void _stackToQueue() {
    char filePath[100];
    TStack *stk;
    TQueue *q;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    q = stackToQueue(stk);
    if (q != NULL) {
        printf("Successfully converted stack to queue.\n");
    }
}

void _stackToList() {
    char filePath[100];
    TStack *stk;
    TList *l;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    l = stackToList(stk);
    if (l != NULL) {
        printf("Successfully converted stack to list.\n");
    }
}

void _addNameStack() {
    char filePath[100];
    char name[100], def[500], dob[20], dod[20];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    printf("Enter name: ");
    scanf(" %99[^\n]", name);
    printf("Enter definition: ");
    scanf(" %499[^\n]", def);
    printf("Enter Date of Birth (DD-MM-YYYY): ");
    scanf(" %19[^\n]", dob);
    printf("Enter Date of Death (DD-MM-YYYY or N/A): ");
    scanf(" %19[^\n]", dod);
    
    stk = addNameStack(stk, name, def, dob, dod);
    printf("Successfully added %s to the stack.\n", name);
}

void _definitionStack() {
    char filePath[100];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    stk = definitionStack(stk);
    printf("Stack successfully sorted by definition word count.\n");
}

void _pronunciationStack() {
    char filePath[100];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    stk = pronunciationStack(stk);
    printf("Stack successfully sorted by pronunciation (short definitions on top).\n");
}

void _getSmallest() {
    char filePath[100];
    TStack *stk;
    char* smallest;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    smallest = getSmallest(stk);
    if (smallest != NULL) {
        printf("Alphabetically smallest name in stack: %s\n", smallest);
    } else {
        printf("Error: Stack is empty.\n");
    }
}

void _continuousSearch() {
    printf("Not yet implemented by Person 2.\n");
}

void _isPersonalityKilled() {
    char filePath[100];
    char name[100];
    TStack *stk;
    TStack *result;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    printf("Enter personality name to check: ");
    scanf(" %99[^\n]", name);

    result = getInfoPersonality(stk, name);
    if (result != NULL) {
        if (isPersonalityKilled(result->definition)) {
            printf("Yes, '%s' was killed/assassinated.\n", name);
        } else {
            printf("No, there is no mention of '%s' being killed.\n", name);
        }
    } else {
        printf("Personality not found in stack.\n");
    }
}

void _recRevStack() {
    char filePath[100];
    TStack *stk;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    stk = buildStackFromFile(filePath);
    if (stk == NULL) return;

    stk = recRevStack(stk);
    printf("Stack successfully reversed recursively.\n");
}

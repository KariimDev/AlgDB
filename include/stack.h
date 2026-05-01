#ifndef STACK_H
#define STACK_H

#include "types.h"
#include <stdbool.h>

TStack* push(struct TStack* stk, const char* name, const char* definition, const char* dob, const char* dod);
struct TStack* pop(struct TStack* stk, struct TStack* poppedData);
struct TStack* peek(struct TStack* stk);
bool isEmpty(struct TStack* stk);
TStack* toStack(struct TList *merged);
struct TStack* getInfoPersonality(struct TStack *stk, char *name);
void swapData(struct TStack* a, struct TStack* b);
struct TStack* sortNameStack(struct TStack *s);
struct TStack* deleteName(struct TStack *stk, char *name);
TStack* updateStack(TStack *stk, char *name, char *def, char *DoB, char *DoD);
TQueue* stackToQueue(TStack *stk);
TList* stackToList(TStack *stk);
TStack* addNameStack(TStack *stk, char *name, char *definition, char *DoB, char *DoD);
int countword(char* str);
TStack* definitionStack(TStack *stk);
struct TStack* pronunciationStack(struct TStack *stk);
char* getSmallest(TStack *stk);
bool isPersonalityKilled(char *definition);
void insertAtend(struct TStack **stk, struct TStack data);
struct TStack* recRevStack(struct TStack *stk);

/* Wrapper functions for CLI */
void _toStack();
void _getInfoPersonality();
void _sortNameStack();
void _deleteName();
void _updateStack();
void _stackToQueue();
void _stackToList();
void _addNameStack();
void _definitionStack();
void _pronunciationStack();
void _getSmallest();
void _continuousSearch();
void _isPersonalityKilled();
void _recRevStack();

#endif

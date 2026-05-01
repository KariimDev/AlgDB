#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "types.h"
#include <stdio.h>

TList*  getPersonality(FILE *f);
TList*  getDatePersonality(FILE *f);
void    getInfoByDates(TList *s, TList *DoB);
void    getInfoByDates2(TList *s, TList *DoD);
TList*  sortWord(TList *syn);
TList*  sortWord2(TList *syn);
TList*  sortPersonality(TList *syn);
TList*  deletepersonality(FILE *f, TList *s, TList *a, char *name);
TList*  updatePersonality(FILE *f, TList *s, TList *a, char *name, char *definition, char *DoB, char *DoD);
TList*  similarPersonality(TList *s, char *word);
TList*  countPersonality(TList *s, date *prt);
TList*  palindromeName(TList *s);
TList*  mergeNodes(TList *s, TList *a);
TList*  merge2Nodes(TList *s, TList *a);
TList*  addPersonality(TList *s, TList *a, char *name, char *DoB, char *DoD);
TList*  addEvents(TEvent *b, char *eventName, char *date);
TQueue* sName(TList *s);
TQueue* ageP(TList *a);
TQueue* toQueue(TList *merged);

/* CLI Wrappers */
void _getPersonality();
void _getDatePersonality();
void _getInfoByDates();
void _getInfoByDates2();
void _sortWord();
void _sortWord2();
void _sortPersonality();
void _deletepersonality();
void _updatePersonality();
void _similarPersonality();
void _countPersonality();
void _palindromeName();
void _mergeNodes();
void _merge2Nodes();
void _addPersonality();
void _addEvents();
void _sName();
void _ageP();
void _toQueue();

#endif
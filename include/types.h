#ifndef TYPES_H
#define TYPES_H

#define MAX_NAME 100
#define MAX_DEF  500
#define MAX_DATE 20

typedef struct {
    int day;
    int month;
    int year;
} date;

typedef struct TList {
    char name[MAX_NAME];
    char definition[MAX_DEF];
    char DoB[MAX_DATE];
    char DoD[MAX_DATE];
    struct TList *next;
    struct TList *prev;
} TList;

typedef struct TEvent {
    char name[MAX_NAME];
    char description[MAX_DEF];
    char date[MAX_DATE];
    struct TEvent *next;
    struct TEvent *prev;
} TEvent;

typedef struct {
    TList *front;
    TList *rear;
} TQueue;

typedef struct TStack {
    char name[MAX_NAME];
    char definition[MAX_DEF];
    char DoB[MAX_DATE];
    char DoD[MAX_DATE];
    struct TStack *next;
} TStack;

typedef struct TTree {
    char name[MAX_NAME];
    char definition[MAX_DEF];
    char DoB[MAX_DATE];
    char DoD[MAX_DATE];
    struct TTree *left;
    struct TTree *right;
} TTree;

#endif

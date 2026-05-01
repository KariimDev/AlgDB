#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tree.h"
#include "../include/stack.h"

TTree* insertTree(TTree *root, TTree *newNode) {
    if (root == NULL) return newNode;
    
    if (strcmp(newNode->name, root->name) < 0) {
        root->left = insertTree(root->left, newNode);
    } else if (strcmp(newNode->name, root->name) > 0) {
        root->right = insertTree(root->right, newNode);
    } else {
        free(newNode);
    }
    
    return root;
}


TTree* toTree(TStack *stk) {
    TTree *root = NULL;
    TTree *newNode;
    TStack *popped;

    while (stk != NULL) {
        popped = pop(&stk);
        
        if (popped != NULL) {
            newNode = (TTree *)malloc(sizeof(TTree));
            strcpy(newNode->name, popped->name);
            strcpy(newNode->definition, popped->definition);
            strcpy(newNode->DoB, popped->DoB);
            strcpy(newNode->DoD, popped->DoD);
            newNode->left = NULL;
            newNode->right = NULL;
            
            root = insertTree(root, newNode);
            free(popped);
        }
    }

    return root;
}
TTree* fillTree(FILE *f) {
    TList *s;
    TList *a;
    TList *merged;
    TStack *stk;

    if (f == NULL) return NULL;

    rewind(f);
    s = getPersonality(f);
    
    rewind(f);
    a = getDatePersonality(f);
    
    merged = mergeNodes(s, a);
    
    stk = toStack(merged);
    
    return toTree(stk);
}

void _fillTree() {
    char filePath[100];
    FILE *file;
    TTree *root;
    
    printf("Enter file path: ");
    scanf("%99s", filePath);
    
    file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    root = fillTree(file);
    fclose(file);
    
    if (root != NULL) {
        printf("Tree built successfully. Root is: %s\n", root->name);
    } else {
        printf("Failed to build tree.\n");
    }
}
TTree* getInfoNameTree(TTree *tr, char *name) {
    int cmp;

    if (tr == NULL) return NULL;

    cmp = strcmp(name, tr->name);

    if (cmp == 0) {
        return tr;
    } else if (cmp < 0) {
        return getInfoNameTree(tr->left, name);
    } else {
        return getInfoNameTree(tr->right, name);
    }
}

void _getInfoNameTree() {
    FILE *file;
    TTree *root;
    TTree *result;
    char nameToSearch[100];

    file = fopen("data/algeria_history.txt", "r");
    if (file == NULL) return;
    
    root = fillTree(file);
    fclose(file);

    printf("Enter name to search: ");
    scanf(" %99[^\n]", nameToSearch);

    result = getInfoNameTree(root, nameToSearch);

    if (result != NULL) {
        printf("Name: %s\n", result->name);
        printf("Definition: %s\n", result->definition);
        printf("Date of Birth: %s\n", result->DoB);
        printf("Date of Death: %s\n", result->DoD);
    } else {
        printf("Not found.\n");
    }
}
TTree* addNameBST(TTree *tr, char *name, char *DoB, char *DoD) {
    TTree *newNode;

    newNode = (TTree *)malloc(sizeof(TTree));
    strcpy(newNode->name, name);
    newNode->definition[0] = '\0';
    strcpy(newNode->DoB, DoB);
    strcpy(newNode->DoD, DoD);
    newNode->left = NULL;
    newNode->right = NULL;

    return insertTree(tr, newNode);
}

void _addNameBST() {
    FILE *file;
    TTree *root = NULL;
    char name[100];
    char dob[40];
    char dod[40];

    file = fopen("data/algeria_history.txt", "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    }

    printf("Enter name: ");
    scanf(" %99[^\n]", name);
    printf("Enter Date of Birth: ");
    scanf(" %39[^\n]", dob);
    printf("Enter Date of Death: ");
    scanf(" %39[^\n]", dod);

    root = addNameBST(root, name, dob, dod);

    if (root != NULL) {
        printf("Successfully added %s to the BST.\n", name);
    }
}
TTree* minValueNode(TTree* node) {
    TTree* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

TTree* deleteNameBST(TTree *tr, char *name) {
    int cmp;
    TTree *temp;

    if (tr == NULL) return tr;

    cmp = strcmp(name, tr->name);

    if (cmp < 0) {
        tr->left = deleteNameBST(tr->left, name);
    } else if (cmp > 0) {
        tr->right = deleteNameBST(tr->right, name);
    } else {
        if (tr->left == NULL) {
            temp = tr->right;
            free(tr);
            return temp;
        } else if (tr->right == NULL) {
            temp = tr->left;
            free(tr);
            return temp;
        }

        temp = minValueNode(tr->right);

        strcpy(tr->name, temp->name);
        strcpy(tr->definition, temp->definition);
        strcpy(tr->DoB, temp->DoB);
        strcpy(tr->DoD, temp->DoD);

        tr->right = deleteNameBST(tr->right, temp->name);
    }
    return tr;
}

void _deleteNameBST() {
    FILE *file;
    TTree *root = NULL;
    char nameToSearch[100];

    file = fopen("data/algeria_history.txt", "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    }

    printf("Enter name to delete: ");
    scanf(" %99[^\n]", nameToSearch);

    root = deleteNameBST(root, nameToSearch);

    printf("Deleted %s from the BST if it existed.\n", nameToSearch);
}

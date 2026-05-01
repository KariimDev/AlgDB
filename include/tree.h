#ifndef TREE_H
#define TREE_H

#include "types.h"
#include <stdio.h>
#include <stdbool.h>



TTree* toTree(TStack *stk);
TTree* fillTree(FILE *f);
TTree* getInfoNameTree(TTree *tr, char *name);
TStack* addNameBST(TTree *tr, char *name, char *DoB, char *DoD);
TTree* deleteNameBST(TTree *tr, char *name);
TTree* updateNameBST(TTree *tr, char *name, char *s, char *DoB, char *DoD);
TTree* traversalBSTinOrder(TTree *tr);
TTree* traversalBSTpreOrder(TTree *tr);
TTree* traversalBSTpostOrder(TTree *tr);
void heightSizeBST(TTree *tr);
TTree* lowestCommonAncestor(TTree *tr, char *word1, char *word2);
int countNodesRange(TTree *tr, int l, int h);
TTree* inOrderSuccessor(TTree *tr, char *word);
TTree* BSTMirror(TTree *tr);
bool isBalancedBST(TTree *tr);
TTree* BTSMerge(TTree *tr1, TTree *tr2);
#endif

#ifndef RECURSION_H
#define RECURSION_H

#include <stdio.h>
#include <stdbool.h>

/* Core recursive functions */
int  countOccurence(FILE *f, char *name);
void recursiveFilter(FILE *src, FILE *temp, char *word);
void removeOccurence(char *filename, char *word);
void recursiveReplace(FILE *src, FILE *temp, char *targetName, char *newName);
void replaceOccurence(char *filename, char *targetName, char *newName);
void swap(char *x, char *y);
void permute(char *a, int l, int r);
void namePermutation(char *name);
void recursiveSubseq(char *word, char *output, int index, int outIndex);
void subseqName(char *word);
int  recursionExtractYear(char *date);
void scanOverlap(FILE *f, char *date1, char *date2);
void longestSubyear(char *date1, char *date2);
int  distinctSubseqWord(char *event);
bool isPalindromeRecursive(char *s, int start, int end);
bool isPalindromeWord(char *event);

/* CLI wrapper functions */
void _countOccurence();
void _removeOccurence();
void _replaceOccurence();
void _namePermutation();
void _subseqName();
void _longestSubyear();
void _distinctSubseqWord();
void _isPalindromeWord();

#endif

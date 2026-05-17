#include "../include/recursion.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

int countOccurence(FILE *f, char *name) {
    char buffer[256];
    char lowerBuffer[256];
    char lowerName[256];
    int i;

    buffer[0] = '\0';

    /*
     * " %255[^=:\n{}]" reads up to 255 characters until it encounters
     * '=', ':', '\n', '{', or '}'.
     * fscanf returns EOF when it reaches the end of the file or if an error occurs.
     */
    if (fscanf(f, " %255[^=:\n{}]", buffer) == EOF) {
        return 0;
    }

    /*
     * fgetc is used to read the next character from the file stream,
     * jumping over the delimiter (=, :, or {) to land on the next piece of data.
     */
    fgetc(f);

    strcpy(lowerBuffer, buffer);
    for (i = 0; lowerBuffer[i]; i++) {
        lowerBuffer[i] = tolower((unsigned char)lowerBuffer[i]);
    }

    strcpy(lowerName, name);
    for (i = 0; lowerName[i]; i++) {
        lowerName[i] = tolower((unsigned char)lowerName[i]);
    }

    /*
     * strstr is used instead of strcmp because buffer contains a large chunk
     * of text and we want to check if 'name' appears anywhere inside it.
     */
    if (strstr(lowerBuffer, lowerName) != NULL) {
        return 1 + countOccurence(f, name);
    } else {
        return countOccurence(f, name);
    }
}

void recursiveFilter(FILE *src, FILE *temp, char *word) {
    char buffer[256];
    char delimiter;

    if (fscanf(src, " %255[^=:\n{}]", buffer) == EOF) {
        return;
    }

    delimiter = fgetc(src);

    if (strcmp(buffer, word) != 0) {
        fprintf(temp, "%s", buffer);
        if (delimiter != EOF) {
            fputc(delimiter, temp);
        }
    } else {
        if (delimiter != EOF) {
            fputc(delimiter, temp);
        }
    }

    recursiveFilter(src, temp, word);
}

void removeOccurence(char *filename, char *word) {
    FILE *f;
    FILE *temp;

    f = fopen(filename, "r");
    temp = fopen("temp.txt", "w");

    if (f == NULL || temp == NULL) {
        perror("File error");
        return;
    }

    recursiveFilter(f, temp, word);

    fclose(f);
    fclose(temp);

    remove(filename);
    rename("temp.txt", filename);
}

void recursiveReplace(FILE *src, FILE *temp, char *targetName, char *newName) {
    char buffer[256];
    char delimiter;

    if (fscanf(src, " %255[^=:\n{}]", buffer) == EOF) {
        return;
    }

    delimiter = fgetc(src);

    if (strcmp(buffer, targetName) == 0) {
        fprintf(temp, "%s", newName);
    } else {
        fprintf(temp, "%s", buffer);
    }

    if (delimiter != EOF) {
        fputc(delimiter, temp);
    }
    recursiveReplace(src, temp, targetName, newName);
}

void replaceOccurence(char *filename, char *targetName, char *newName) {
    FILE *f;
    FILE *temp;

    f = fopen(filename, "r");
    temp = fopen("temp_replace.txt", "w");

    if (f == NULL || temp == NULL) {
        perror("File error");
        return;
    }

    recursiveReplace(f, temp, targetName, newName);

    fclose(f);
    fclose(temp);

    remove(filename);
    rename("temp_replace.txt", filename);
}

void swap(char *x, char *y) {
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void permute(char *a, int l, int r) {
    int i;

    if (l == r) {
        printf("%s\n", a);
    } else {
        for (i = l; i <= r; i++) {
            swap((a + l), (a + i));
            permute(a, l + 1, r);
            swap((a + l), (a + i));
        }
    }
}

void namePermutation(char *name) {
    char *tempName;
    int n;

    tempName = strdup(name);
    n = strlen(tempName);
    permute(tempName, 0, n - 1);
    free(tempName);
}

void recursiveSubseq(char *word, char *output, int index, int outIndex) {
    if (word[index] == '\0') {
        output[outIndex] = '\0';
        if (outIndex > 0) printf("%s ", output);
        return;
    }
    output[outIndex] = word[index];
    recursiveSubseq(word, output, index + 1, outIndex + 1);
    recursiveSubseq(word, output, index + 1, outIndex);
}

void subseqName(char *word) {
    char output[256];
    printf("\nSubsequences of '%s':\n", word);
    recursiveSubseq(word, output, 0, 0);
    printf("\n");
}

int recursionExtractYear(char *date) {
    int len;
    len = strlen(date);
    if (len >= 4) {
        return atoi(date + len - 4);
    }
    return atoi(date);
}


void scanOverlap(FILE *f, char *date1, char *date2) {
    char line[1024];
    char *open;
    char *close;
    char dateBuf[40];
    int len;  
    int startYear;
    int endYear;
    int y1;
    int y2;

    if (fgets(line, sizeof(line), f) == NULL) {
        return;
    }

    y1 = recursionExtractYear(date1);
    y2 = recursionExtractYear(date2);
    open = strchr(line, '{');
    close = strchr(line, '}');

    if (open != NULL && close != NULL && close > open) {
        len = (int)(close - open - 1);
        if (len > 0 && len < 40) {
            strncpy(dateBuf, open + 1, len);
            dateBuf[len] = '\0';
            /* startYear = first number (e.g. 1889 from "1889-1940")
             * endYear   = last 4 chars (e.g. 1940 from "1889-1940")
             * Overlap: startYear <= y2 AND endYear >= y1 */
            startYear = atoi(dateBuf);
            endYear   = recursionExtractYear(dateBuf);
            if (startYear <= y2 && endYear >= y1) {
                printf("%s", line);
            }
        }
    }

    scanOverlap(f, date1, date2);
}


void longestSubyear(char *date1, char *date2) {
    char filePath[100];
    FILE *f;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    f = fopen(filePath, "r");
    if (f == NULL) {
        printf("Error opening file.\n");
        return;
    }

    printf("\nEntries overlapping [%s - %s]:\n", date1, date2);
    scanOverlap(f, date1, date2);
    fclose(f);
}

int distinctSubseqWord(char *event) {
    int n;
    int j;
    int found;

    if (*event == '\0') return 1;

    n = strlen(event);
    j = 1;
    found = 0;

    while (j < n) {
        if (event[j] == event[0]) {
            found = 1;
            break;
        }
        j++;
    }

    if (found) {
        return 2 * distinctSubseqWord(event + 1) - distinctSubseqWord(event + j + 1);
    } else {
        return 2 * distinctSubseqWord(event + 1);
    }
}

bool isPalindromeRecursive(char *s, int start, int end) {
    if (start >= end) return true;
    if (s[start] != s[end]) return false;
    return isPalindromeRecursive(s, start + 1, end - 1);
}

bool isPalindromeWord(char *event) {
    return isPalindromeRecursive(event, 0, strlen(event) - 1);
}

void _countOccurence() {
    char filePath[100];
    char targetName[100];
    FILE *f;
    int count;

    printf("Enter file path: ");
    scanf("%99s", filePath);
    printf("Enter name to count: ");
    scanf(" %99[^\n]", targetName);
    f = fopen(filePath, "r");
    if (f == NULL) {
        printf("Error: Could not open file %s\n", filePath);
        return;
    }
    count = countOccurence(f, targetName);
    printf("The name '%s' appeared %d times in %s\n", targetName, count, filePath);
    fclose(f);
}

void _removeOccurence() {
    char filePath[100];
    char word[100];

    printf("Enter file path: ");
    scanf("%99s", filePath);
    printf("Enter word to remove: ");
    scanf(" %99[^\n]", word);
    removeOccurence(filePath, word);
    printf("Occurrences of '%s' removed from %s.\n", word, filePath);
}

void _replaceOccurence() {
    char filePath[100];
    char target[100];
    char replacement[100];

    printf("Enter file path: ");
    scanf("%99s", filePath);
    printf("Enter target word: ");
    scanf(" %99[^\n]", target);
    printf("Enter replacement word: ");
    scanf(" %99[^\n]", replacement);
    replaceOccurence(filePath, target, replacement);
    printf("Replaced '%s' with '%s' in %s.\n", target, replacement, filePath);
}

void _namePermutation() {
    char name[100];

    printf("Enter name for permutations: ");
    scanf(" %99[^\n]", name);
    printf("Permutations of '%s':\n", name);
    namePermutation(name);
}

void _subseqName() {
    char word[100];

    printf("Enter word for subsequences: ");
    scanf(" %99[^\n]", word);
    subseqName(word);
}

void _longestSubyear() {
    char start[10];
    char end[10];

    printf("Enter start year: ");
    scanf("%9s", start);
    printf("Enter end year: ");
    scanf("%9s", end);
    longestSubyear(start, end);
}

void _distinctSubseqWord() {
    char word[100];

    printf("Enter word for distinct subsequences: ");
    scanf(" %99[^\n]", word);
    printf("Distinct subsequences of '%s': %d\n", word, distinctSubseqWord(word));
}

void _isPalindromeWord() {
    char word[100];

    printf("Enter word: ");
    scanf(" %99s", word);
    if (isPalindromeWord(word)) {
        printf("'%s' is a palindrome.\n", word);
    } else {
        printf("'%s' is NOT a palindrome.\n", word);
    }
}
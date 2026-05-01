#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/linkedlist.h"

TList* getPersonality(FILE *f) {
    TList *head = NULL;
    TList *tail = NULL;
    char line[1024]; 
    char *equal_sign;
    TList *newNode;
    char *brace;
    int name_length;
    
    if (f == NULL) {
        return NULL;
    }
    
    rewind(f);

    while (fgets(line, sizeof(line), f) != NULL) {
        line[strcspn(line, "\n")] = 0; /* strcspn returns index of first '\n', setting it to 0 strips it */

        equal_sign = strchr(line, '='); /* strchr finds first occurrence of '=' */
        
        if (equal_sign != NULL) {
            newNode = (TList *)malloc(sizeof(TList));
            newNode->next = NULL;
            newNode->prev = NULL;
            
            newNode->DoB[0] = '\0';
            newNode->DoD[0] = '\0';

            strcpy(newNode->definition, equal_sign + sizeof(char));
            
            if (newNode->definition[0] == ' ') {
                memmove(newNode->definition, newNode->definition + 1, strlen(newNode->definition));
            }

            brace = strstr(line, " {"); /* strstr finds the " {" substring to locate name boundary */
            if (brace != NULL) {
                name_length = brace - line; 
                strncpy(newNode->name, line, name_length);
                newNode->name[name_length] = '\0'; 
            }

            if (head == NULL) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }
    }

    return head;
}


void _getPersonality(){
    char filePath[100];
    FILE* file;
    TList *head;
    
    printf("Please Enter The File Path : ");
    scanf("%99s", filePath);
    
    file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Error: Could not open %s\n", filePath);
        return;
    }
    
    head = getPersonality(file);
    
    while(head != NULL){
        printf("Name : %s\n", head->name);
        printf("Definition : %s\n", head->definition);
        head = head->next;
    }

    fclose(file);
}

TList* getDatePersonality(FILE *f) {
    TList *head = NULL;
    TList *tail = NULL;
    char line[1024]; 
    char *equal_sign;
    TList *newNode;
    char *brace_start;
    char *brace_end;
    int name_length;
    int dates_len;
    char dates_str[40];
    char *dash;
    int dob_len;
    
    if (f == NULL) {
        return NULL;
    }
    
    rewind(f);

    while (fgets(line, sizeof(line), f) != NULL) {
        line[strcspn(line, "\n")] = 0; /* strcspn returns index of first '\n', setting it to 0 strips it */

        equal_sign = strchr(line, '='); /* strchr finds first '=' which marks a personality line */
        
        if (equal_sign != NULL) {
            newNode = (TList *)malloc(sizeof(TList));
            newNode->next = NULL;
            newNode->prev = NULL;
            
            newNode->name[0] = '\0';
            newNode->definition[0] = '\0';
            newNode->DoB[0] = '\0';
            newNode->DoD[0] = '\0';

            brace_start = strchr(line, '{');
            brace_end = strchr(line, '}');

            if (brace_start != NULL && brace_end != NULL && brace_end > brace_start) {
                name_length = brace_start - line;
                
                if (name_length > 0 && line[name_length - 1] == ' ') {
                    name_length--; 
                }
                
                strncpy(newNode->name, line, name_length);
                newNode->name[name_length] = '\0';

                dates_len = brace_end - brace_start - 1;
                strncpy(dates_str, brace_start + 1, dates_len);
                dates_str[dates_len] = '\0';

                dash = strchr(dates_str, '-');
                if (dash != NULL) {
                    dob_len = dash - dates_str;
                    
                    strncpy(newNode->DoB, dates_str, dob_len);
                    newNode->DoB[dob_len] = '\0';
                    
                    strcpy(newNode->DoD, dash + 1);
                } else {
                    strcpy(newNode->DoB, dates_str);
                }
            }

            if (head == NULL) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }
    }
    return head;
}

void _getDatePersonality() {
    char filePath[100];
    FILE* file;
    TList *head;
    
    printf("Please Enter The File Path : ");
    scanf("%99s", filePath);
    
    file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Error: Could not open %s\n", filePath);
        return;
    }
    
    head = getDatePersonality(file);
    
    while(head != NULL){
        printf("Name : %s\n", head->name);
        printf("Definition : %s\n", head->definition);
        printf("DoB : %s\n", head->DoB);
        printf("DoD : %s\n", head->DoD);
        head = head->next;
    }

    fclose(file);
}




void getInfoByDates(TList *s, TList *DoB) {
    TList *current;
    
    if (s == NULL || DoB == NULL) return;
    
    current = s;
    while (current != NULL) {
        if (strcmp(current->DoB, DoB->DoB) == 0) {
            printf("Found Match for DoB %s:\n", DoB->DoB);
            printf("Name: %s\n", current->name);
            printf("Definition: %s\n\n", current->definition);
        }
        current = current->next;
    }
}

void _getInfoByDates() {
    char filePath[100];
    char targetDoB[MAX_DATE];
    FILE *file;
    TList *s;
    TList *datesList;
    TList *mergedList;
    TList dummyDoB;
    
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    
    printf("Please Enter the Date of Birth to search (e.g. 1923): ");
    scanf("%19s", targetDoB);
    
    file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Error: Could not open %s\n", filePath);
        return;
    }
    
    s = getPersonality(file);
    fclose(file);
    
    file = fopen(filePath, "r");
    datesList = getDatePersonality(file);
    fclose(file);
    
    strcpy(dummyDoB.DoB, targetDoB);
    mergedList = mergeNodes(s, datesList);
    getInfoByDates(mergedList, &dummyDoB); 
}

void getInfoByDates2(TList *s, TList *DoD) {
    TList *current;
    
    if (s == NULL || DoD == NULL) return;
    
    current = s;
    while (current != NULL) {
        if (strcmp(current->DoD, DoD->DoD) == 0) {
            printf("Found Match for DoD %s:\n", DoD->DoD);
            printf("Name: %s\n", current->name);
            printf("Definition: %s\n\n", current->definition);
        }
        current = current->next;
    }
}

void _getInfoByDates2() {
    char filePath[100];
    char targetDoD[MAX_DATE];
    FILE *file;
    TList *s;
    TList *datesList;
    TList *mergedList;
    TList dummyDoD;
    
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    
    printf("Please Enter the Date of Death to search: ");
    scanf("%19s", targetDoD);
    
    file = fopen(filePath, "r");
    if (file == NULL) return;
    
    s = getPersonality(file);
    fclose(file);
    
    file = fopen(filePath, "r");
    if (file != NULL) {
        datesList = getDatePersonality(file);
        fclose(file);
    } else {
        datesList = NULL;
    }
    
    strcpy(dummyDoD.DoD, targetDoD);
    mergedList = mergeNodes(s, datesList);
    getInfoByDates2(mergedList, &dummyDoD);
}

void swapData(TList* l1,TList* l2){
    char name[MAX_NAME];
    char definition[MAX_DEF];
    char DoB[MAX_DATE];
    char DoD[MAX_DATE];

    strcpy(name,l1->name);
    strcpy(l1->name,l2->name);
    strcpy(l2->name,name);
    strcpy(definition,l1->definition);
    strcpy(l1->definition,l2->definition);
    strcpy(l2->definition,definition);
    strcpy(DoB,l1->DoB);
    strcpy(l1->DoB,l2->DoB);
    strcpy(l2->DoB,DoB);
    strcpy(DoD,l1->DoD);
    strcpy(l1->DoD,l2->DoD);
    strcpy(l2->DoD,DoD);
}





TList* sortWord(TList *syn){
    TList* cur,*cur2;


    cur=syn;
    while(cur!=NULL){
        cur2=syn;
        while(cur2!=NULL && cur2->next!=NULL){
            if(strcmp(cur2->name,cur2->next->name)>0){
                swapData(cur2,cur2->next);

            }
            cur2=cur2->next;
        }        

        cur=cur->next;
    }
        


    return syn;
}

void _sortWord(){
    char filePath[100];
    FILE *file;
    TList *syn;
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    syn = getPersonality(file);
    fclose(file);
    syn = sortWord(syn);
    while(syn!=NULL){
        printf("%s\n",syn->name);
        syn=syn->next;
    }
}



TList* sortWord2(TList *syn){
    TList* cur,*cur2;


    cur=syn;
    while(cur!=NULL){
        cur2=syn;
        while(cur2!=NULL && cur2->next!=NULL){
            if(strlen(cur2->name)>strlen(cur2->next->name)){
                swapData(cur2,cur2->next);

            }
            cur2=cur2->next;
        }        

        cur=cur->next;
    }
        


    return syn;
}

void _sortWord2(){
    char filePath[100];
    FILE *file;
    TList *syn;
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    syn = getPersonality(file);
    fclose(file);
    syn = sortWord2(syn);
    while(syn!=NULL){
        printf("%s\n",syn->name);
        syn=syn->next;
    }
}




int extractYear(char *dateStr) {
    char *lastSlash;
    if (dateStr == NULL || dateStr[0] == '\0') return 0;
    
    lastSlash = strrchr(dateStr, '/'); /* strrchr finds the LAST '/' to isolate the year in dd/mm/yyyy */
    if (lastSlash != NULL) {
        return atoi(lastSlash + 1);
    }
    return atoi(dateStr);
}

TList* sortPersonality(TList *syn){
    TList *cur, *cur2;
    int dob1, dod1, age1;
    int dob2, dod2, age2;

    cur = syn;
    while(cur != NULL){
        cur2 = syn;
        while(cur2 != NULL && cur2->next != NULL){
            
            dob1 = extractYear(cur2->DoB);
            if (strlen(cur2->DoD) > 0) {
                dod1 = extractYear(cur2->DoD);
            } else {
                dod1 = 2026;
            }
            age1 = dod1 - dob1;

            dob2 = extractYear(cur2->next->DoB);
            if (strlen(cur2->next->DoD) > 0) {
                dod2 = extractYear(cur2->next->DoD);
            } else {
                dod2 = 2026;
            }
            age2 = dod2 - dob2;

            if(age1 > age2){
                swapData(cur2, cur2->next);
            }
            
            cur2 = cur2->next;
        }        

        cur = cur->next;
    }

    return syn;
}

void _sortPersonality(){
    char filePath[100];
    FILE *file;
    TList *a;
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);
    a = sortPersonality(a);
    while(a!=NULL){
        printf("%s | DoB: %s | DoD: %s\n",a->name, a->DoB, a->DoD);
        a=a->next;
    }
}


TList* deletepersonality(FILE *f, TList *s, TList *a, char *name) {
    TList *curr;
    FILE *temp;
    char line[1024];

    curr = s;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            if (curr->prev != NULL) curr->prev->next = curr->next;
            else s = curr->next;
            if (curr->next != NULL) curr->next->prev = curr->prev;
            free(curr);
            break;
        }
        curr = curr->next;
    }

    curr = a;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            if (curr->prev != NULL) curr->prev->next = curr->next;
            else a = curr->next;
            if (curr->next != NULL) curr->next->prev = curr->prev;
            free(curr);
            break;
        }
        curr = curr->next;
    }

    rewind(f);
    temp = fopen("temp_algeria.txt", "w");
    if (temp != NULL) {
        while (fgets(line, sizeof(line), f) != NULL) {
            if (strncmp(line, name, strlen(name)) == 0 && line[strlen(name)] == ' ') { /* strncmp checks if line starts with the name */
                continue;
            }
            fputs(line, temp);
        }
        fclose(temp);
    }

    return s;
}

void _deletepersonality(){
    char filePath[100];
    char targetName[100];
    FILE *file;
    TList *s;
    TList *a;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    printf("Please Enter the Name to delete: ");
    scanf("%99s", targetName);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = deletepersonality(file, s, a, targetName);
    fclose(file);

    remove(filePath);
    rename("temp_algeria.txt", filePath);

    printf("Deleted %s successfully.\n", targetName);
}
TList* updatePersonality(FILE *f, TList *s, TList *a, char *name, char *definition, char *DoB, char *DoD) {
    TList *curr;
    FILE *temp;
    char line[1024];

    curr = s;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            strcpy(curr->definition, definition);
            strcpy(curr->DoB, DoB);
            strcpy(curr->DoD, DoD);
            break;
        }
        curr = curr->next;
    }

    curr = a;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            strcpy(curr->DoB, DoB);
            strcpy(curr->DoD, DoD);
            break;
        }
        curr = curr->next;
    }

    rewind(f);
    temp = fopen("temp_algeria.txt", "w");
    if (temp != NULL) {
        while (fgets(line, sizeof(line), f) != NULL) {
            if (strncmp(line, name, strlen(name)) == 0 && line[strlen(name)] == ' ') { /* strncmp checks if line starts with the name */
                if (strlen(DoD) > 0) {
                    fprintf(temp, "%s {%s-%s}= %s\n", name, DoB, DoD, definition);
                } else {
                    fprintf(temp, "%s {%s}= %s\n", name, DoB, definition);
                }
            } else {
                fputs(line, temp);
            }
        }
        fclose(temp);
    }

    return s;
}

void _updatePersonality() {
    char filePath[100];
    char targetName[MAX_NAME];
    char newDef[MAX_DEF];
    char newDoB[MAX_DATE];
    char newDoD[MAX_DATE];
    FILE *file;
    TList *s;
    TList *a;
    int c;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    c = getchar();
    while (c != '\n' && c != EOF) c = getchar();

    printf("Name of personality to update: ");
    fgets(targetName, sizeof(targetName), stdin);
    targetName[strcspn(targetName, "\n")] = 0; /* strcspn strips the trailing newline left by fgets */

    printf("New Definition: ");
    fgets(newDef, sizeof(newDef), stdin);
    newDef[strcspn(newDef, "\n")] = 0;

    printf("New DoB: ");
    fgets(newDoB, sizeof(newDoB), stdin);
    newDoB[strcspn(newDoB, "\n")] = 0;

    printf("New DoD (leave empty if none): ");
    fgets(newDoD, sizeof(newDoD), stdin);
    newDoD[strcspn(newDoD, "\n")] = 0;

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = updatePersonality(file, s, a, targetName, newDef, newDoB, newDoD);
    fclose(file);

    remove(filePath);
    rename("temp_algeria.txt", filePath);

    printf("Updated %s successfully.\n", targetName);
}
TList* similarPersonality(TList *s, char *word) {
    TList *result;
    TList *resultTail;
    TList *curr;
    TList *newNode;

    result = NULL;
    resultTail = NULL;
    curr = s;

    while (curr != NULL) {
        if (strstr(curr->DoB, word) != NULL || strstr(curr->DoD, word) != NULL) { /* strstr checks if year appears anywhere in the date string */
            newNode = (TList *)malloc(sizeof(TList));
            strcpy(newNode->name, curr->name);
            strcpy(newNode->definition, curr->definition);
            strcpy(newNode->DoB, curr->DoB);
            strcpy(newNode->DoD, curr->DoD);
            newNode->next = NULL;
            newNode->prev = NULL;

            if (result == NULL) {
                result = newNode;
                resultTail = newNode;
            } else {
                resultTail->next = newNode;
                newNode->prev = resultTail;
                resultTail = newNode;
            }
        }
        curr = curr->next;
    }
    return result;
}

void _similarPersonality() {
    char filePath[100];
    char yearWord[20];
    FILE *file;
    TList *a;
    TList *res;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    printf("Enter year to search (e.g. 1923): ");
    scanf("%19s", yearWord);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);

    res = similarPersonality(a, yearWord);
    while (res != NULL) {
        printf("- %s (DoB: %s, DoD: %s)\n", res->name, res->DoB, res->DoD);
        res = res->next;
    }
}

TList* countPersonality(TList *s, date *prt) {
    char yearStr[10];
    sprintf(yearStr, "%d", prt->year); /* sprintf converts int year to string for strstr comparison */
    return similarPersonality(s, yearStr);
}

void _countPersonality() {
    char filePath[100];
    FILE *file;
    TList *a;
    TList *res;
    date targetDate;
    int count;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    printf("Enter target year: ");
    scanf("%d", &targetDate.year);
    targetDate.day = 0;
    targetDate.month = 0;

    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);

    res = countPersonality(a, &targetDate);
    count = 0;
    printf("Personalities involving year %d:\n", targetDate.year);
    while (res != NULL) {
        count++;
        printf("- %s (DoB: %s, DoD: %s)\n", res->name, res->DoB, res->DoD);
        res = res->next;
    }
    printf("Total: %d\n", count);
}
int isPalindrome(char *word) {
    int len;
    int i;
    len = strlen(word);
    if (len < 2) return 0;
    for (i = 0; i < len / 2; i++) {
        if (tolower((unsigned char)word[i]) != tolower((unsigned char)word[len - 1 - i])) {
            return 0;
        }
    }
    return 1;
}

TList* palindromeName(TList *s) {
    TList *result;
    TList *curr;
    TList *check;
    TList *temp;
    TList *newNode;
    char defCopy[MAX_DEF];
    char *token;
    int exists;

    result = NULL;
    curr = s;

    while (curr != NULL) {
        strcpy(defCopy, curr->definition);
        token = strtok(defCopy, " ,.-()"); /* strtok splits definition into words on these delimiters */

        while (token != NULL) {
            if (isPalindrome(token)) {
                exists = 0;
                check = result;
                while (check != NULL) {
                if (strcasecmp(check->name, token) == 0) { /* strcasecmp compares strings ignoring uppercase/lowercase */
                        exists = 1;
                        break;
                    }
                    check = check->next;
                }

                if (!exists) {
                    newNode = (TList *)malloc(sizeof(TList));
                    strcpy(newNode->name, token);
                    newNode->definition[0] = '\0';
                    newNode->DoB[0] = '\0';
                    newNode->DoD[0] = '\0';
                    newNode->next = NULL;
                    newNode->prev = NULL;

                    if (result == NULL || strcasecmp(result->name, token) >= 0) {
                        newNode->next = result;
                        if (result != NULL) result->prev = newNode;
                        result = newNode;
                    } else {
                        temp = result;
                        while (temp->next != NULL && strcasecmp(temp->next->name, token) < 0) {
                            temp = temp->next;
                        }
                        newNode->next = temp->next;
                        if (temp->next != NULL) temp->next->prev = newNode;
                        temp->next = newNode;
                        newNode->prev = temp;
                    }
                }
            }
            token = strtok(NULL, " ,.-()"); /* strtok(NULL) continues tokenizing from where it left off */
        }
        curr = curr->next;
    }
    return result;
}

void _palindromeName() {
    char filePath[100];
    FILE *file;
    TList *s;
    TList *res;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);

    res = palindromeName(s);
    printf("Palindromes found in definitions:\n");
    while (res != NULL) {
        printf("- %s\n", res->name);
        res = res->next;
    }
}
TList* mergeNodes(TList *s, TList *a) {
    TList *merged;
    TList *tail;
    TList *currS;
    TList *currA;
    TList *newNode;

    merged = NULL;
    tail = NULL;
    currS = s;

    while (currS != NULL) {
        newNode = (TList *)malloc(sizeof(TList));
        strcpy(newNode->name, currS->name);
        strcpy(newNode->definition, currS->definition);
        newNode->DoB[0] = '\0';
        newNode->DoD[0] = '\0';
        newNode->next = NULL;
        newNode->prev = NULL;

        currA = a;
        while (currA != NULL) {
            if (strcmp(currA->name, currS->name) == 0) {
                strcpy(newNode->DoB, currA->DoB);
                strcpy(newNode->DoD, currA->DoD);
                break;
            }
            currA = currA->next;
        }

        if (merged == NULL) {
            merged = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }

        currS = currS->next;
    }
    return merged;
}

void _mergeNodes() {
    char filePath[100];
    FILE *file;
    TList *s;
    TList *a;
    TList *m;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);

    m = mergeNodes(s, a);
    while (m != NULL) {
        printf("%s | DoB: %s | DoD: %s | %s\n", m->name, m->DoB, m->DoD, m->definition);
        m = m->next;
    }
}
TList* merge2Nodes(TList *s, TList *a) {
    TList *merged;
    TList *tail;

    merged = mergeNodes(s, a);
    if (merged == NULL) return NULL;

    tail = merged;
    while (tail->next != NULL) {
        tail = tail->next;
    }

    tail->next = merged;
    merged->prev = tail;

    return merged;
}

void _merge2Nodes() {
    char filePath[100];
    FILE *file;
    TList *s;
    TList *a;
    TList *m;

    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);

    m = merge2Nodes(s, a);
    if (m != NULL) {
        printf("Circular list created.\n");
        printf("First node: %s\n", m->name);
        printf("Last node (via prev): %s\n", m->prev->name);
        printf("Last->next points back to: %s\n", m->prev->next->name);
    }
}
TList* addPersonality(TList *s, TList *a, char *name, char *DoB, char *DoD) {
    TList *newNodeS;
    TList *newNodeA;
    TList *tail;
    FILE *f;

    newNodeS = (TList *)malloc(sizeof(TList));
    strcpy(newNodeS->name, name);
    newNodeS->definition[0] = '\0';
    strcpy(newNodeS->DoB, DoB);
    strcpy(newNodeS->DoD, DoD);
    newNodeS->next = NULL;
    newNodeS->prev = NULL;

    if (s == NULL) {
        s = newNodeS;
    } else {
        tail = s;
        while (tail->next != NULL) tail = tail->next;
        tail->next = newNodeS;
        newNodeS->prev = tail;
    }

    newNodeA = (TList *)malloc(sizeof(TList));
    strcpy(newNodeA->name, name);
    newNodeA->definition[0] = '\0';
    strcpy(newNodeA->DoB, DoB);
    strcpy(newNodeA->DoD, DoD);
    newNodeA->next = NULL;
    newNodeA->prev = NULL;

    if (a != NULL) {
        tail = a;
        while (tail->next != NULL) tail = tail->next;
        tail->next = newNodeA;
        newNodeA->prev = tail;
    }

    f = fopen("data/algeria_history.txt", "a");
    if (f != NULL) {
        if (strlen(DoD) > 0) {
            fprintf(f, "%s {%s-%s}= \n", name, DoB, DoD);
        } else {
            fprintf(f, "%s {%s}= \n", name, DoB);
        }
        fclose(f);
    }

    return s;
}

void _addPersonality() {
    char name[MAX_NAME];
    char dob[MAX_DATE];
    char dod[MAX_DATE];
    int c;

    c = getchar();
    while (c != '\n' && c != EOF) c = getchar();

    printf("Enter new Personality Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter DoB: ");
    fgets(dob, sizeof(dob), stdin);
    dob[strcspn(dob, "\n")] = 0;

    printf("Enter DoD (leave empty if none): ");
    fgets(dod, sizeof(dod), stdin);
    dod[strcspn(dod, "\n")] = 0;

    addPersonality(NULL, NULL, name, dob, dod);
    printf("Added %s to file.\n", name);
}

TList* addEvents(TEvent *b, char *namEvente, char *date) {
    FILE *f;

    f = fopen("data/algeria_history.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s {%s}: \n", namEvente, date);
        fclose(f);
    }

    return (TList *)b;
}

void _addEvents() {
    char name[MAX_NAME];
    char eventDate[MAX_DATE];
    int c;

    c = getchar();
    while (c != '\n' && c != EOF) c = getchar();

    printf("Enter Event Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter Event Date (e.g. 20/10/1827): ");
    fgets(eventDate, sizeof(eventDate), stdin);
    eventDate[strcspn(eventDate, "\n")] = 0;

    addEvents(NULL, name, eventDate);
    printf("Added event %s to file.\n", name);
}

int wordCount(char *str) {
    int count;
    int inWord;
    count = 0;
    inWord = 0;
    while (*str) {
        if (*str == ' ') inWord = 0;
        else if (!inWord) { inWord = 1; count++; }
        str++;
    }
    return count;
}

TQueue* makeQueue(TList *list) {
    TQueue *q;
    TList *tail;
    q = (TQueue *)malloc(sizeof(TQueue));
    q->front = list;
    tail = list;
    if (tail != NULL)
        while (tail->next != NULL) tail = tail->next;
    q->rear = tail;
    return q;
}

TQueue* sName(TList *s) {
    TList *cur;
    TList *cur2;
    cur = s;
    while (cur != NULL) {
        cur2 = s;
        while (cur2 != NULL && cur2->next != NULL) {
            if (wordCount(cur2->name) > wordCount(cur2->next->name))
                swapData(cur2, cur2->next);
            cur2 = cur2->next;
        }
        cur = cur->next;
    }
    return makeQueue(s);
}

void _sName() {
    char filePath[100];
    FILE *file;
    TList *s;
    TQueue *q;
    TList *curr;
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);
    q = sName(s);
    curr = q->front;
    while (curr != NULL) {
        printf("- %s (words: %d)\n", curr->name, wordCount(curr->name));
        curr = curr->next;
    }
}

TQueue* ageP(TList *a) {
    return makeQueue(sortPersonality(a));
}

void _ageP() {
    char filePath[100];
    FILE *file;
    TList *a;
    TQueue *q;
    TList *curr;
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);
    q = ageP(a);
    curr = q->front;
    while (curr != NULL) {
        printf("- %s | DoB: %s | DoD: %s\n", curr->name, curr->DoB, curr->DoD);
        curr = curr->next;
    }
}

TQueue* toQueue(TList *merged) {
    return makeQueue(merged);
}

void _toQueue() {
    char filePath[100];
    FILE *file;
    TList *s;
    TList *a;
    TQueue *q;
    printf("Please Enter The File Path: ");
    scanf("%99s", filePath);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    s = getPersonality(file);
    fclose(file);
    file = fopen(filePath, "r");
    if (file == NULL) return;
    a = getDatePersonality(file);
    fclose(file);
    q = toQueue(mergeNodes(s, a));
    printf("Queue front: %s\n", q->front->name);
    printf("Queue rear: %s\n", q->rear->name);
}

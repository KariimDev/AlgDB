#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tree.h"
#include "../include/stack.h"
#include "../include/linkedlist.h"

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
    TTree *root;
    TTree *newNode;
    TStack *popped;

    root = NULL;

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
    char filePath[100];

    root = NULL;
    
    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

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
    TTree *root;
    char name[100];
    char dob[40];
    char dod[40];
    char filePath[100];

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
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
    TTree* current;
    current = node;
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
    TTree *root;
    char nameToSearch[100];
    char filePath[100];

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter name to delete: ");
    scanf(" %99[^\n]", nameToSearch);

    root = deleteNameBST(root, nameToSearch);

    printf("Deleted %s from the BST if it existed.\n", nameToSearch);
}


TTree* updateNameBST(TTree *tr, char *name, char *s, char *DoB, char *DoD) {
    TTree *newNode;

    tr = deleteNameBST(tr, name);

    newNode = (TTree *)malloc(sizeof(TTree));
    strcpy(newNode->name, name);
    strcpy(newNode->definition, s);
    strcpy(newNode->DoB, DoB);
    strcpy(newNode->DoD, DoD);
    newNode->left = NULL;
    newNode->right = NULL;

    return insertTree(tr, newNode);
}

void _updateNameBST() {
    FILE *file;
    TTree *root;
    char name[100];
    char definition[500];
    char dob[40];
    char dod[40];
    char filePath[100];

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter name of personality to update: ");
    scanf(" %99[^\n]", name);
    printf("Enter new definition: ");
    scanf(" %499[^\n]", definition);
    printf("Enter new Date of Birth: ");
    scanf(" %39[^\n]", dob);
    printf("Enter new Date of Death: ");
    scanf(" %39[^\n]", dod);

    root = updateNameBST(root, name, definition, dob, dod);

    if (root != NULL) {
        printf("Updated %s using delete-and-insert method.\n", name);
    }
}

TTree* traversalBSTinOrder(TTree *tr) {
    if (tr != NULL) {
        traversalBSTinOrder(tr->left);
        printf("-> %s\n", tr->name);
        traversalBSTinOrder(tr->right);
    }
    return tr;
}

void _traversalBSTinOrder() {
    char filePath[100];
    FILE *file;
    TTree *root;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("\n--- In-Order Traversal ---\n");
    traversalBSTinOrder(root);
}

TTree* traversalBSTpreOrder(TTree *tr) {
    if (tr != NULL) {
        printf("-> %s\n", tr->name);
        traversalBSTpreOrder(tr->left);
        traversalBSTpreOrder(tr->right);
    }
    return tr;
}

void _traversalBSTpreOrder() {
    char filePath[100];
    FILE *file;
    TTree *root;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("\n--- Pre-Order Traversal ---\n");
    traversalBSTpreOrder(root);
}

TTree* traversalBSTpostOrder(TTree *tr) {
    if (tr != NULL) {
        traversalBSTpostOrder(tr->left);
        traversalBSTpostOrder(tr->right);
        printf("-> %s\n", tr->name);
    }
    return tr;
}

void _traversalBSTpostOrder() {
    char filePath[100];
    FILE *file;
    TTree *root;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("\n--- Post-Order Traversal ---\n");
    traversalBSTpostOrder(root);
}
int getTreeHeight(TTree *tr) {
    int leftHeight;
    int rightHeight;
    
    if (tr == NULL) return 0;
    
    leftHeight = getTreeHeight(tr->left);
    rightHeight = getTreeHeight(tr->right);
    
    if (leftHeight > rightHeight) {
        return leftHeight + 1;
    } else {
        return rightHeight + 1;
    }
}

int getTreeSize(TTree *tr) {
    if (tr == NULL) return 0;
    
    return 1 + getTreeSize(tr->left) + getTreeSize(tr->right);
}

void heightSizeBST(TTree *tr) {
    int h;
    int s;

    h = getTreeHeight(tr);
    s = getTreeSize(tr);
    
    printf("Tree Height: %d\n", h);
    printf("Tree Size (Number of Nodes): %d\n", s);
}

void _heightSizeBST() {
    char filePath[100];
    FILE *file;
    TTree *root;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("\n--- Tree Statistics ---\n");
    heightSizeBST(root);
}
TTree* lowestCommonAncestor(TTree *tr, char *word1, char *word2) {
    int cmp1;
    int cmp2;

    if (tr == NULL) return NULL;

    cmp1 = strcmp(word1, tr->name);
    cmp2 = strcmp(word2, tr->name);

    if (cmp1 < 0 && cmp2 < 0) {
        return lowestCommonAncestor(tr->left, word1, word2);
    }
    
    if (cmp1 > 0 && cmp2 > 0) {
        return lowestCommonAncestor(tr->right, word1, word2);
    }

    return tr;
}

void _lowestCommonAncestor() {
    char filePath[100];
    FILE *file;
    TTree *root;
    TTree *lca;
    char word1[100];
    char word2[100];

    root = NULL;
    lca = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter first name: ");
    scanf(" %99[^\n]", word1);
    
    printf("Enter second name: ");
    scanf(" %99[^\n]", word2);

    lca = lowestCommonAncestor(root, word1, word2);

    if (lca != NULL) {
        printf("The Lowest Common Ancestor is: %s\n", lca->name);
    } else {
        printf("Lowest Common Ancestor not found in the tree.\n");
    }
}

int countNodesRange(TTree *tr, int l, int h) {
    int count;
    int len;

    count = 0;

    if (tr == NULL) return 0;

    len = strlen(tr->name);
    
    if (len >= l && len <= h) {
        count = 1;
    }

    count += countNodesRange(tr->left, l, h);
    count += countNodesRange(tr->right, l, h);

    return count;
}

void _countNodesRange() {
    char filePath[100];
    FILE *file;
    TTree *root;
    int l;
    int h;
    int count;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter minimum name length (int): ");
    scanf("%d", &l);
    
    printf("Enter maximum name length (int): ");
    scanf("%d", &h);

    count = countNodesRange(root, l, h);

    printf("There are %d personalities with a name length between %d and %d.\n", count, l, h);
}
TTree* inOrderSuccessor(TTree *tr, char *word) {
    TTree *successor;
    int cmp;

    successor = NULL;

    while (tr != NULL) {
        cmp = strcmp(word, tr->name);

        if (cmp < 0) {
            successor = tr;
            tr = tr->left;
        } else if (cmp > 0) {
            tr = tr->right;
        } else {
            if (tr->right != NULL) {
                return minValueNode(tr->right);
            }
            break;
        }
    }

    return successor;
}

void _inOrderSuccessor() {
    char filePath[100];
    FILE *file;
    TTree *root;
    TTree *successor;
    char word[100];

    root = NULL;
    successor = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter name to find its successor: ");
    scanf(" %99[^\n]", word);

    successor = inOrderSuccessor(root, word);

    if (successor != NULL) {
        printf("The In-Order Successor of '%s' is: %s\n", word, successor->name);
    } else {
        printf("There is no In-Order Successor for '%s' (it might be the last node, or not in the tree).\n", word);
    }
}
TTree* BSTMirror(TTree *tr) {
    TTree *temp;

    if (tr == NULL) {
        return NULL;
    }

    BSTMirror(tr->left);
    BSTMirror(tr->right);

    temp = tr->left;
    tr->left = tr->right;
    tr->right = temp;

    return tr;
}

void _BSTMirror() {
    char filePath[100];
    FILE *file;
    TTree *root;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    root = BSTMirror(root);

    if (root != NULL) {
        printf("The tree has been successfully mirrored!\n");
    }
}
int absInt(int x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

bool isBalancedBST(TTree *tr) {
    int lh;
    int rh;

    if (tr == NULL) return true;

    lh = getTreeHeight(tr->left);
    rh = getTreeHeight(tr->right);

    if (absInt(lh - rh) <= 1 && 
        isBalancedBST(tr->left) && 
        isBalancedBST(tr->right)) {
        return true;
    }

    return false;
}

void _isBalancedBST() {
    char filePath[100];
    FILE *file;
    TTree *root;

    root = NULL;

    printf("Enter file path: ");
    scanf("%99s", filePath);

    file = fopen(filePath, "r");
    if (file != NULL) {
        root = fillTree(file);
        fclose(file);
    } else {
        printf("Error opening file.\n");
        return;
    }

    if (isBalancedBST(root)) {
        printf("The BST is balanced!\n");
    } else {
        printf("The BST is NOT balanced.\n");
    }
}

void storeInOrder(TTree *node, TTree **arr, int *index) {
    if (node == NULL) return;
    storeInOrder(node->left, arr, index);
    arr[*index] = node;
    (*index)++;
    storeInOrder(node->right, arr, index);
}

TTree* sortedArrayToBST(TTree **arr, int start, int end) {
    int mid;
    TTree *root;
    
    if (start > end) return NULL;
    
    mid = (start + end) / 2;
    root = arr[mid];
    
    root->left = sortedArrayToBST(arr, start, mid - 1);
    root->right = sortedArrayToBST(arr, mid + 1, end);
    
    return root;
}

TTree* BTSMerge(TTree *tr1, TTree *tr2) {
    int size1;
    int size2;
    int total;
    TTree **arr1;
    TTree **arr2;
    TTree **merged;
    int idx1;
    int idx2;
    int i;
    int j;
    int k;
    TTree *root;

    size1 = getTreeSize(tr1);
    size2 = getTreeSize(tr2);
    total = size1 + size2;
    
    arr1 = (TTree **)malloc(size1 * sizeof(TTree *));
    arr2 = (TTree **)malloc(size2 * sizeof(TTree *));
    merged = (TTree **)malloc(total * sizeof(TTree *));
    
    idx1 = 0;
    idx2 = 0;
    i = 0;
    j = 0;
    k = 0;

    storeInOrder(tr1, arr1, &idx1);
    storeInOrder(tr2, arr2, &idx2);

    while (i < size1 && j < size2) {
        if (strcmp(arr1[i]->name, arr2[j]->name) < 0) {
            merged[k++] = arr1[i++];
        } else if (strcmp(arr1[i]->name, arr2[j]->name) > 0) {
            merged[k++] = arr2[j++];
        } else {
            merged[k++] = arr1[i++];
            j++;
            total--; 
        }
    }

    while (i < size1) merged[k++] = arr1[i++];
    while (j < size2) merged[k++] = arr2[j++];

    root = sortedArrayToBST(merged, 0, total - 1);

    free(arr1);
    free(arr2);
    free(merged);

    return root;
}

void _BTSMerge() {
    FILE *file1;
    FILE *file2;
    TTree *root1;
    TTree *root2;
    TTree *mergedRoot;
    char path1[100];
    char path2[100];

    root1 = NULL;
    root2 = NULL;
    mergedRoot = NULL;

    printf("Enter first file path: ");
    scanf("%99s", path1);
    file1 = fopen(path1, "r");
    if (file1 != NULL) {
        root1 = fillTree(file1);
        fclose(file1);
    } else {
        printf("Error opening first file.\n");
    }

    printf("Enter second file path: ");
    scanf("%99s", path2);
    file2 = fopen(path2, "r");
    if (file2 != NULL) {
        root2 = fillTree(file2);
        fclose(file2);
    } else {
        printf("Error opening second file.\n");
    }

    mergedRoot = BTSMerge(root1, root2);
    
    printf("\n--- Merged Balanced Tree (In-Order) ---\n");
    traversalBSTinOrder(mergedRoot);
}

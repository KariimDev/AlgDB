#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/linkedlist.h"
#include "include/tree.h"

#define START_R 60
#define START_G 160
#define START_B 255

#define END_R 255
#define END_G 150
#define END_B 100

#define SHADOW_R 20
#define SHADOW_G 20
#define SHADOW_B 30

#define C_NUM "\033[38;2;255;150;100m"
#define C_RST "\033[0m"

void printBitmapLogo() {
    const char *logo[] = {
        "#         ##    #       ##    ###   ###  ",
        " #       #  #   #      #  #   #  #  #  # ",
        "  #      #  #   #      #      #  #  #  # ",
        "   #     ####   #      # ##   #  #  ###  ",
        "  #      #  #   #      #  #   #  #  #  # ",
        " #       #  #   #      #  #   #  #  #  # ",
        "#        #  #   ####    ##    ###   ###  "
    };

    int rows = sizeof(logo) / sizeof(logo[0]);
    int width = strlen(logo[0]);
    int y, x;

    printf("\n");
    for (y = 0; y < rows + 1; y++) {
        printf("  ");
        for (x = 0; x < width + 1; x++) {
            int is_pixel = (y < rows && x < width && logo[y][x] == '#');
            int is_shadow = (y - 1 >= 0 && x - 1 >= 0 &&
                             y - 1 < rows && x - 1 < width &&
                             logo[y - 1][x - 1] == '#');

            if (is_pixel) {
                float ratio = (float)x / (float)width;
                int r = (int)(START_R + (END_R - START_R) * ratio);
                int g = (int)(START_G + (END_G - START_G) * ratio);
                int b = (int)(START_B + (END_B - START_B) * ratio);
                printf("\033[48;2;%d;%d;%dm  \033[0m", r, g, b);
            } else if (is_shadow) {
                printf("\033[48;2;%d;%d;%dm  \033[0m", SHADOW_R, SHADOW_G, SHADOW_B);
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseConsole() {
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void menuLinkedList();
void showAbout();

int main() {
    int choice;
    do {
        clearScreen();
        printBitmapLogo();
        printf("------------- AlgDB Main Menu -------------\n");
        printf(C_NUM "1." C_RST " Linked Lists & Queues\n");
        printf(C_NUM "2." C_RST " About\n");
        printf(C_NUM "0." C_RST " Exit\n");
        printf("-------------------------------------------\n");
        printf("Enter your choice: ");
        while (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number: ");
            while (getchar() != '\n');
        }

        switch (choice) {
            case 1: menuLinkedList(); break;
            case 2: showAbout(); break;
            case 0:
                printf("Exiting AlgDB. Goodbye.\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
                pauseConsole();
        }
    } while (choice != 0);

    return 0;
}

void menuLinkedList() {
    int choice;
    do {
        clearScreen();
        printf("\033[38;2;%d;%d;%dm >> Linked Lists & Queues\033[0m\n", START_R, START_G, START_B);
        printf("-------------------------------------\n");
        printf(" " C_NUM " 1." C_RST " Get Personalities       " C_NUM " 2." C_RST " Get Date Personalities\n");
        printf(" " C_NUM " 3." C_RST " Search by Date of Birth " C_NUM " 4." C_RST " Search by Date of Death\n");
        printf(" " C_NUM " 5." C_RST " Sort Alphabetically     " C_NUM " 6." C_RST " Sort by Name Length\n");
        printf(" " C_NUM " 7." C_RST " Sort by Age             " C_NUM " 8." C_RST " Delete Personality\n");
        printf(" " C_NUM " 9." C_RST " Update Personality      " C_NUM "10." C_RST " Similar Personalities\n");
        printf(" " C_NUM "11." C_RST " Count by Year           " C_NUM "12." C_RST " Palindrome Names\n");
        printf(" " C_NUM "13." C_RST " Merge Lists             " C_NUM "14." C_RST " Merge Circular\n");
        printf(" " C_NUM "15." C_RST " Add Personality         " C_NUM "16." C_RST " Add Event\n");
        printf(" " C_NUM "17." C_RST " Sort Names (Queue)      " C_NUM "18." C_RST " Sort by Age (Queue)\n");
        printf(" " C_NUM "19." C_RST " Convert to Queue        " C_NUM " 0." C_RST " Back\n");
        printf("-------------------------------------\n");
        printf("Choice: ");
        while (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number: ");
            while (getchar() != '\n');
        }

        switch (choice) {
            case  1: _getPersonality();    break;
            case  2: _getDatePersonality(); break;
            case  3: _getInfoByDates();    break;
            case  4: _getInfoByDates2();   break;
            case  5: _sortWord();          break;
            case  6: _sortWord2();         break;
            case  7: _sortPersonality();   break;
            case  8: _deletepersonality(); break;
            case  9: _updatePersonality(); break;
            case 10: _similarPersonality(); break;
            case 11: _countPersonality();  break;
            case 12: _palindromeName();    break;
            case 13: _mergeNodes();        break;
            case 14: _merge2Nodes();       break;
            case 15: _addPersonality();    break;
            case 16: _addEvents();         break;
            case 17: _sName();             break;
            case 18: _ageP();              break;
            case 19: _toQueue();           break;
            case  0: return;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) pauseConsole();
    } while (choice != 0);
}

void showAbout() {
    clearScreen();
    printBitmapLogo();
    printf("\033[38;2;%d;%d;%dm >> About AlgDB\033[0m\n", START_R, START_G, START_B);
    printf("AlgDB is a database management system for the History of Algeria.\n");
    printf("It manages personalities, events and important dates using dynamic\n");
    printf("data structures: linked lists, queues, stacks, and BST trees.\n");
    printf("================================================\n");
    printf("Project: History of Algeria Database\n");
    printf("Academic Year: 2025/2026\n");
    printf("School: NSCS, Pole scientifique, Sidi Abdallah\n");
    printf("================================================\n");
    pauseConsole();
}

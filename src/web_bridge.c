#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tree.h"

/* ── JSON string escaper ──────────────────────────────── */
void jstr(const char *s) {
    printf("\"");
    if (!s) { printf("\""); return; }
    while (*s) {
        if      (*s == '"')  printf("\\\"");
        else if (*s == '\\') printf("\\\\");
        else if (*s == '\n') printf("\\n");
        else if (*s == '\r') {} /* skip CR */
        else if (*s == '\t') printf("\\t");
        else putchar(*s);
        s++;
    }
    printf("\"");
}

/* ── Trim trailing whitespace in place ────────────────── */
void rtrim(char *s) {
    int n = strlen(s);
    while (n > 0 && (s[n-1] == '\r' || s[n-1] == '\n' || s[n-1] == ' ')) {
        s[--n] = '\0';
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("{\"error\":\"Missing arguments\"}\n");
        return 1;
    }

    char *cmd      = argv[1];
    char *filepath = argv[2];

    /* ── get_personalities ────────────────────────────── */
    if (strcmp(cmd, "get_personalities") == 0) {
        FILE *f = fopen(filepath, "r");
        if (!f) { printf("{\"error\":\"Cannot open file\"}\n"); return 1; }

        char line[4096];
        int first = 1;
        printf("[\n");

        while (fgets(line, sizeof(line), f)) {
            rtrim(line);
            if (strlen(line) == 0) continue;

            /* Personality lines: Name {dob-dod}= definition */
            char *brace_open  = strchr(line, '{');
            char *brace_close = strchr(line, '}');
            char *eq          = strchr(line, '=');
            char *colon       = strchr(line, ':');

            /* Must have {}, =, and = must come after } */
            if (!brace_open || !brace_close || !eq) continue;
            /* Skip event lines (they have ':' before '=') */
            if (colon && colon < eq) continue;

            /* Extract name */
            char name[512] = {0};
            int name_len = brace_open - line;
            if (name_len <= 0 || name_len >= 512) continue;
            strncpy(name, line, name_len);
            name[name_len] = '\0';
            rtrim(name);

            /* Extract dates string "dob-dod" or "dob" */
            char dates[128] = {0};
            int dates_len = brace_close - brace_open - 1;
            if (dates_len > 0 && dates_len < 128) {
                strncpy(dates, brace_open + 1, dates_len);
                dates[dates_len] = '\0';
            }

            /* Split dates on '-', being careful about dd/mm/yyyy dashes */
            char dob[64] = {0};
            char dod[64] = {0};
            /* Find the '-' that separates dob from dod — it's the one NOT inside slashes */
            /* Strategy: first token before '-' that isn't part of a dd/mm/yyyy is DoB */
            char *dash = strchr(dates, '-');
            if (dash) {
                int dob_len = dash - dates;
                strncpy(dob, dates, dob_len); dob[dob_len] = '\0';
                strncpy(dod, dash + 1, sizeof(dod)-1);
            } else {
                strncpy(dob, dates, sizeof(dob)-1);
            }

            /* Extract definition — everything after '= ' */
            char *def_start = eq + 1;
            while (*def_start == ' ') def_start++;

            if (!first) printf(",\n");
            first = 0;
            printf("  {\"name\":");
            jstr(name);
            printf(",\"dob\":");
            jstr(dob);
            printf(",\"dod\":");
            jstr(dod);
            printf(",\"definition\":");
            jstr(def_start);
            printf("}");
        }

        printf("\n]\n");
        fclose(f);
        return 0;
    }

    /* ── get_events ───────────────────────────────────── */
    if (strcmp(cmd, "get_events") == 0) {
        FILE *f = fopen(filepath, "r");
        if (!f) { printf("{\"error\":\"Cannot open file\"}\n"); return 1; }

        char line[4096];
        int first = 1;
        printf("[\n");

        while (fgets(line, sizeof(line), f)) {
            rtrim(line);
            if (strlen(line) == 0) continue;

            /* Event lines: Name {date}: description */
            char *brace_open  = strchr(line, '{');
            char *brace_close = strchr(line, '}');
            char *colon       = strstr(line, "}:");

            if (!brace_open || !brace_close || !colon) continue;

            /* Must NOT have '=' (that would be a personality) */
            if (strchr(line, '=') != NULL) continue;

            /* Extract event name */
            char name[512] = {0};
            int name_len = brace_open - line;
            if (name_len <= 0 || name_len >= 512) continue;
            strncpy(name, line, name_len);
            name[name_len] = '\0';
            rtrim(name);

            /* Extract date */
            char date[128] = {0};
            int date_len = brace_close - brace_open - 1;
            if (date_len > 0 && date_len < 128) {
                strncpy(date, brace_open + 1, date_len);
                date[date_len] = '\0';
            }

            /* Extract description — after '}: ' */
            char *desc = colon + 2;
            while (*desc == ' ') desc++;

            if (!first) printf(",\n");
            first = 0;
            printf("  {\"name\":");
            jstr(name);
            printf(",\"date\":");
            jstr(date);
            printf(",\"description\":");
            jstr(desc);
            printf("}");
        }

        printf("\n]\n");
        fclose(f);
        return 0;
    }

    /* ── get_bst ──────────────────────────────────────── */
    if (strcmp(cmd, "get_bst") == 0) {
        FILE *f = fopen(filepath, "r");
        if (!f) { printf("{\"error\":\"Cannot open file\"}\n"); return 1; }

        TTree *root = fillTree(f);
        fclose(f);

        void print_tree(TTree *node) {
            if (!node) { printf("null"); return; }
            printf("{\"name\":");
            jstr(node->name);
            printf(",\"left\":");
            print_tree(node->left);
            printf(",\"right\":");
            print_tree(node->right);
            printf("}");
        }

        print_tree(root);
        printf("\n");
        return 0;
    }

    /* ── add_personality ──────────────────────────────────────── */
    if (strcmp(cmd, "add_personality") == 0) {
        if (argc < 7) { printf("{\"error\":\"Missing arguments for add_personality\"}\n"); return 1; }
        char *name = argv[3];
        char *dob = argv[4];
        char *dod = argv[5];
        char *def = argv[6];

        FILE *f = fopen(filepath, "r");
        if (!f) { printf("{\"error\":\"Cannot open file for reading\"}\n"); return 1; }

        char temp_path[1024];
        sprintf(temp_path, "%s.tmp", filepath);
        FILE *temp = fopen(temp_path, "w");
        if (!temp) { fclose(f); printf("{\"error\":\"Cannot create temp file\"}\n"); return 1; }

        char line[4096];
        int inserted = 0;

        while (fgets(line, sizeof(line), f)) {
            /* If we haven't inserted yet, check if this line is an event (has }: and no =) */
            if (!inserted && strstr(line, "}:") && !strchr(line, '=')) {
                if (strlen(dod) > 0) fprintf(temp, "%s {%s-%s}= %s\n", name, dob, dod, def);
                else fprintf(temp, "%s {%s}= %s\n", name, dob, def);
                inserted = 1;
            }
            fputs(line, temp);
        }

        /* If no events were found, append to the very end */
        if (!inserted) {
            if (strlen(dod) > 0) fprintf(temp, "%s {%s-%s}= %s\n", name, dob, dod, def);
            else fprintf(temp, "%s {%s}= %s\n", name, dob, def);
        }

        fclose(f);
        fclose(temp);

        remove(filepath);
        rename(temp_path, filepath);

        printf("{\"status\":\"success\"}\n");
        return 0;
    }

    /* ── delete_personality ───────────────────────────────────── */
    if (strcmp(cmd, "delete_personality") == 0) {
        if (argc < 4) { printf("{\"error\":\"Missing name for delete_personality\"}\n"); return 1; }
        char *target_name = argv[3];

        FILE *f = fopen(filepath, "r");
        if (!f) { printf("{\"error\":\"Cannot open file for reading\"}\n"); return 1; }

        char temp_path[1024];
        sprintf(temp_path, "%s.tmp", filepath);
        FILE *temp = fopen(temp_path, "w");
        if (!temp) { fclose(f); printf("{\"error\":\"Cannot create temp file\"}\n"); return 1; }

        char line[4096];
        int deleted = 0;
        while (fgets(line, sizeof(line), f)) {
            /* For personalities, line starts with target_name followed by " {" */
            char search_str[1024];
            sprintf(search_str, "%s {", target_name);
            if (strncmp(line, search_str, strlen(search_str)) == 0 && strchr(line, '=')) {
                deleted = 1;
                continue;
            }
            fputs(line, temp);
        }

        fclose(f);
        fclose(temp);

        remove(filepath);
        rename(temp_path, filepath);

        if (deleted) printf("{\"status\":\"success\"}\n");
        else printf("{\"error\":\"Personality not found\"}\n");
        return 0;
    }

    /* ── delete_event ───────────────────────────────────── */
    if (strcmp(cmd, "delete_event") == 0) {
        if (argc < 4) { printf("{\"error\":\"Missing name for delete_event\"}\n"); return 1; }
        char *target_name = argv[3];

        FILE *f = fopen(filepath, "r");
        if (!f) { printf("{\"error\":\"Cannot open file for reading\"}\n"); return 1; }

        char temp_path[1024];
        sprintf(temp_path, "%s.tmp", filepath);
        FILE *temp = fopen(temp_path, "w");
        if (!temp) { fclose(f); printf("{\"error\":\"Cannot create temp file\"}\n"); return 1; }

        char line[4096];
        int deleted = 0;
        while (fgets(line, sizeof(line), f)) {
            /* For events, line starts with target_name followed by " {" and NO '=' */
            char search_str[1024];
            sprintf(search_str, "%s {", target_name);
            if (strncmp(line, search_str, strlen(search_str)) == 0 && !strchr(line, '=')) {
                deleted = 1;
                continue;
            }
            fputs(line, temp);
        }

        fclose(f);
        fclose(temp);

        remove(filepath);
        rename(temp_path, filepath);

        if (deleted) printf("{\"status\":\"success\"}\n");
        else printf("{\"error\":\"Event not found\"}\n");
        return 0;
    }

    printf("{\"error\":\"Unknown command\"}\n");
    return 1;
}

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/types.h"
#include "../include/linkedlist.h"
#include "../include/tree.h"
#include "../include/stack.h"

/* =========================================================
 * GLOBAL STATE
 * ========================================================= */

static TList  *g_personalities = NULL; /* name + definition only        */
static TList  *g_dates_list    = NULL; /* name + DoB + DoD only         */
static TList  *g_merged        = NULL; /* name + definition + DoB + DoD */
static TEvent *g_events        = NULL; /* events list                   */
static TTree  *g_bst           = NULL; /* BST built from file           */
static char    g_filepath[512] = "";   /* currently loaded file path    */

/* =========================================================
 * UI PACK
 * Passed to callbacks that need multiple widgets at once.
 * ========================================================= */

typedef struct {
    GtkWidget *input1;
    GtkWidget *input2;
    GtkWidget *input3;
    GtkWidget *output_label;
    GtkWidget *list_box;
} UiPack;

/* =========================================================
 * LOAD PACK
 * Passed to the Open File callback so it can refresh
 * both the personalities list and the events list.
 * ========================================================= */

typedef struct {
    GtkWidget *personalities_list;
    GtkWidget *events_list;
} LoadPack;

/* =========================================================
 * STDOUT CAPTURE
 * Some functions (traversals, heightSizeBST) only print
 * to stdout via printf. We redirect stdout to a temp file,
 * call the function, then read it back into a string.
 * This way results appear in the GUI instead of the terminal.
 * ========================================================= */

#include <unistd.h>
#include <fcntl.h>

static void call_inorder(TTree *t) { traversalBSTinOrder(t); }
static void call_preorder(TTree *t) { traversalBSTpreOrder(t); }
static void call_postorder(TTree *t) { traversalBSTpostOrder(t); }

static char *capture_stdout(void (*func)(TTree *), TTree *arg) {
    char  tmp_path[600];
    int old_stdout_fd;
    int tmp_fd;
    char *buffer;
    long  file_size;
    FILE *f;

    snprintf(tmp_path, sizeof(tmp_path), "cap_tmp.txt");

    fflush(stdout);
    old_stdout_fd = dup(fileno(stdout));
    tmp_fd = open(tmp_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (tmp_fd < 0) return strdup("(capture failed)");
    
    dup2(tmp_fd, fileno(stdout));
    
    func(arg);
    fflush(stdout);
    
    dup2(old_stdout_fd, fileno(stdout));
    close(old_stdout_fd);
    close(tmp_fd);

    f = fopen(tmp_path, "r");
    if (!f) return strdup("(capture failed)");
    fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    rewind(f);

    buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(f);
        remove(tmp_path);
        return strdup("(out of memory)");
    }

    fread(buffer, 1, file_size, f);
    buffer[file_size] = '\0';

    fclose(f);
    remove(tmp_path);
    return buffer;
}

/* =========================================================
 * HELPERS
 * ========================================================= */

static void show_popup(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "%s", message
    );
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* Clears a list box and repopulates it from a TList */
static void refresh_list(GtkWidget *list_box, TList *source) {
    GList     *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    GList     *it;
    TList     *cur;
    char       row_text[300];
    GtkWidget *lbl;

    for (it = children; it != NULL; it = it->next)
        gtk_widget_destroy(GTK_WIDGET(it->data));
    g_list_free(children);

    for (cur = source; cur != NULL; cur = cur->next) {
        snprintf(row_text, sizeof(row_text), "%s   [%s - %s]",
                 cur->name,
                 cur->DoB[0] ? cur->DoB : "?",
                 cur->DoD[0] ? cur->DoD : "?");
        lbl = gtk_label_new(row_text);
        gtk_widget_set_halign(lbl, GTK_ALIGN_START);
        gtk_widget_set_margin_start(lbl, 6);
        gtk_widget_set_margin_top(lbl, 3);
        gtk_widget_set_margin_bottom(lbl, 3);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), lbl, -1);
    }
    gtk_widget_show_all(list_box);
}
/* Parses events from file directly since the backend doesn't have getEvents */
static TEvent *parse_events_from_file(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    char line[1024];
    TEvent *head = NULL, *tail = NULL;
    char *brace_start, *brace_end, *colon;

    if (!f) return NULL;

    while (fgets(line, sizeof(line), f)) {
        brace_start = strchr(line, '{');
        brace_end = strchr(line, '}');
        colon = strchr(line, ':');

        if (brace_start && brace_end && colon && colon > brace_end) {
            TEvent *ev = malloc(sizeof(TEvent));
            int name_len;
            int date_len;
            memset(ev, 0, sizeof(TEvent));
            
            name_len = brace_start - line;
            if (name_len > 0 && line[name_len-1] == ' ') name_len--;
            strncpy(ev->name, line, name_len);
            
            date_len = brace_end - brace_start - 1;
            strncpy(ev->date, brace_start + 1, date_len);
            
            strcpy(ev->description, colon + 1);
            while (ev->description[0] == ' ') {
                memmove(ev->description, ev->description + 1, strlen(ev->description));
            }
            ev->description[strcspn(ev->description, "\r\n")] = '\0';
            
            if (!head) {
                head = ev;
                tail = ev;
            } else {
                tail->next = ev;
                ev->prev = tail;
                tail = ev;
            }
        }
    }
    fclose(f);
    return head;
}

static void refresh_events_list(GtkWidget *list_box) {
    GList     *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    GList     *it;
    TEvent    *cur;
    char       row_text[300];
    GtkWidget *lbl;

    for (it = children; it != NULL; it = it->next)
        gtk_widget_destroy(GTK_WIDGET(it->data));
    g_list_free(children);

    for (cur = g_events; cur != NULL; cur = cur->next) {
        snprintf(row_text, sizeof(row_text), "[%s]  %s", cur->date, cur->name);
        lbl = gtk_label_new(row_text);
        gtk_widget_set_halign(lbl, GTK_ALIGN_START);
        gtk_widget_set_margin_start(lbl, 6);
        gtk_widget_set_margin_top(lbl, 3);
        gtk_widget_set_margin_bottom(lbl, 3);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), lbl, -1);
    }
    gtk_widget_show_all(list_box);
}


/* Wraps a widget in a GtkFrame with a visible title */
static GtkWidget *make_frame(const char *title, GtkWidget *child) {
    GtkWidget *frame = gtk_frame_new(title);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 4);
    gtk_container_add(GTK_CONTAINER(frame), child);
    return frame;
}

/* =========================================================
 * FILE PICKER CALLBACK
 * Open File button lives in the header bar.
 * Loads the file and refreshes both list boxes.
 * ========================================================= */

static void on_open_file_clicked(GtkWidget *button, gpointer data) {
    LoadPack  *lpack = (LoadPack *)data;
    GtkWidget *dialog;
    gint       result;
    FILE      *f;

    dialog = gtk_file_chooser_dialog_new(
        "Choose your Algeria history file", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open",   GTK_RESPONSE_ACCEPT,
        NULL
    );

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_ACCEPT) {
        char *fp = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        strncpy(g_filepath, fp, sizeof(g_filepath) - 1);
        g_free(fp);

        f = fopen(g_filepath, "r");
        if (f != NULL) {
            g_personalities = getPersonality(f);      rewind(f);
            g_dates_list    = getDatePersonality(f);  rewind(f);
            g_bst           = fillTree(f);
            fclose(f);

            /* mergeNodes combines name+definition with name+dates into one full list */
            g_merged = mergeNodes(g_personalities, g_dates_list);

            g_events = parse_events_from_file(g_filepath);

            refresh_list(lpack->personalities_list, g_merged);
            refresh_events_list(lpack->events_list);
        }
    }

    gtk_widget_destroy(dialog);
}

/* =========================================================
 * PERSONALITY ROW CLICK
 * Clicking a row opens a popup with full details.
 * ========================================================= */

static void on_row_activated(GtkListBox *lb, GtkListBoxRow *row, gpointer data) {
    GtkWidget  *lbl      = gtk_bin_get_child(GTK_BIN(row));
    const char *row_text = gtk_label_get_text(GTK_LABEL(lbl));
    char        name_buf[MAX_NAME];
    TList      *cur;
    char        detail[1024];
    int         len;

    /* extract the name (everything before the first '[') */
    memset(name_buf, 0, sizeof(name_buf));
    sscanf(row_text, "%[^[]", name_buf);
    len = strlen(name_buf);
    while (len > 0 && name_buf[len - 1] == ' ') name_buf[--len] = '\0';

    /* find the node in g_merged */
    for (cur = g_merged; cur != NULL; cur = cur->next)
        if (strcmp(cur->name, name_buf) == 0) break;

    if (cur == NULL) return;

    snprintf(detail, sizeof(detail),
        "Name    : %s\n\n"
        "Born    : %s\n"
        "Died    : %s\n\n"
        "Killed? : %s\n\n"
        "--- Definition ---\n%s",
        cur->name,
        cur->DoB[0] ? cur->DoB : "Unknown",
        cur->DoD[0] ? cur->DoD : "Unknown",
        isPersonalityKilled(cur->definition) ? "Yes" : "No / Unknown",
        cur->definition
    );

    show_popup(detail);
}

/* =========================================================
 * SORT CALLBACKS
 * ========================================================= */

static void on_sort_alpha(GtkWidget *btn, gpointer data) {
    if (g_merged == NULL) return;
    g_merged = sortWord(g_merged);
    refresh_list((GtkWidget *)data, g_merged);
}

static void on_sort_length(GtkWidget *btn, gpointer data) {
    if (g_merged == NULL) return;
    g_merged = sortWord2(g_merged);
    refresh_list((GtkWidget *)data, g_merged);
}

static void on_sort_age(GtkWidget *btn, gpointer data) {
    if (g_merged == NULL) return;
    g_merged = sortPersonality(g_merged);
    refresh_list((GtkWidget *)data, g_merged);
}

/* =========================================================
 * ADD / DELETE / UPDATE / SIMILAR CALLBACKS
 * ========================================================= */

static void on_add_personality(GtkWidget *btn, gpointer data) {
    UiPack    *p    = (UiPack *)data;
    const char *name = gtk_entry_get_text(GTK_ENTRY(p->input1));
    const char *dob  = gtk_entry_get_text(GTK_ENTRY(p->input2));
    const char *dod  = gtk_entry_get_text(GTK_ENTRY(p->input3));

    if (strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Name cannot be empty.");
        return;
    }
    g_personalities = addPersonality(g_personalities, g_dates_list,
                                     (char *)name, (char *)dob, (char *)dod);
    g_merged = mergeNodes(g_personalities, g_dates_list);
    refresh_list(p->list_box, g_merged);
    gtk_label_set_text(GTK_LABEL(p->output_label), "Added successfully.");
}

static void on_delete_personality(GtkWidget *btn, gpointer data) {
    UiPack    *p    = (UiPack *)data;
    const char *name = gtk_entry_get_text(GTK_ENTRY(p->input1));
    FILE      *f;

    if (strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Enter a name.");
        return;
    }
    f = fopen(g_filepath, "r+");
    if (f == NULL) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "No file loaded.");
        return;
    }
    g_personalities = deletepersonality(f, g_personalities, g_dates_list, (char *)name);
    fclose(f);
    g_merged = mergeNodes(g_personalities, g_dates_list);
    refresh_list(p->list_box, g_merged);
    gtk_label_set_text(GTK_LABEL(p->output_label), "Deleted (if found).");
}

static void on_update_personality(GtkWidget *btn, gpointer data) {
    UiPack    *p    = (UiPack *)data;
    const char *name = gtk_entry_get_text(GTK_ENTRY(p->input1));
    const char *def  = gtk_entry_get_text(GTK_ENTRY(p->input2));
    FILE      *f;
    TList     *cur;
    char       dob[MAX_DATE] = "";
    char       dod[MAX_DATE] = "";

    if (strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Enter a name.");
        return;
    }

    /* Fetch existing DoB and DoD to preserve them */
    for (cur = g_merged; cur != NULL; cur = cur->next) {
        if (strcmp(cur->name, name) == 0) {
            strcpy(dob, cur->DoB);
            strcpy(dod, cur->DoD);
            break;
        }
    }

    f = fopen(g_filepath, "r+");
    if (f == NULL) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "No file loaded.");
        return;
    }
    g_personalities = updatePersonality(f, g_personalities, g_dates_list,
                                        (char *)name, (char *)def, dob, dod);
    fclose(f);
    g_merged = mergeNodes(g_personalities, g_dates_list);
    refresh_list(p->list_box, g_merged);
    gtk_label_set_text(GTK_LABEL(p->output_label), "Updated (if found).");
}

static void on_find_similar(GtkWidget *btn, gpointer data) {
    UiPack    *p    = (UiPack *)data;
    const char *name = gtk_entry_get_text(GTK_ENTRY(p->input1));
    TList     *result;
    char       output[1024];
    char       line[128];

    if (strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Enter a year or date.");
        return;
    }
    result = similarPersonality(g_merged, (char *)name);
    if (result == NULL) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "No similar personalities found.");
        return;
    }
    memset(output, 0, sizeof(output));
    while (result != NULL) {
        snprintf(line, sizeof(line), "- %s\n", result->name);
        strncat(output, line, sizeof(output) - strlen(output) - 1);
        result = result->next;
    }
    gtk_label_set_text(GTK_LABEL(p->output_label), output);
}



/* =========================================================
 * ADD EVENT CALLBACK
 * ========================================================= */

static void on_add_event(GtkWidget *btn, gpointer data) {
    UiPack     *p     = (UiPack *)data;
    const char *name  = gtk_entry_get_text(GTK_ENTRY(p->input1));
    const char *edate = gtk_entry_get_text(GTK_ENTRY(p->input2));

    if (strlen(name) == 0 || strlen(edate) == 0) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Fill both fields.");
        return;
    }
    addEvents(g_events, (char *)name, (char *)edate);
    g_events = parse_events_from_file(g_filepath);
    refresh_events_list(p->list_box);
    gtk_label_set_text(GTK_LABEL(p->output_label), "Event added.");
}

static void on_event_row_activated(GtkListBox *lb, GtkListBoxRow *row, gpointer data) {
    GtkWidget  *lbl      = gtk_bin_get_child(GTK_BIN(row));
    const char *row_text = gtk_label_get_text(GTK_LABEL(lbl));
    char        name_buf[MAX_NAME];
    TEvent     *cur;
    char        detail[1024];
    char       *bracket_end;

    bracket_end = strchr(row_text, ']');
    if (!bracket_end) return;
    strcpy(name_buf, bracket_end + 2);
    while (name_buf[0] == ' ') memmove(name_buf, name_buf + 1, strlen(name_buf));

    for (cur = g_events; cur != NULL; cur = cur->next)
        if (strcmp(cur->name, name_buf) == 0) break;

    if (cur == NULL) return;

    snprintf(detail, sizeof(detail), "Event: %s\nDate: %s\n\n%s", cur->name, cur->date, cur->description);
    show_popup(detail);
}

/* =========================================================
 * BST CALLBACKS
 * ========================================================= */

static void on_bst_search(GtkWidget *btn, gpointer data) {
    UiPack    *p    = (UiPack *)data;
    const char *name = gtk_entry_get_text(GTK_ENTRY(p->input1));
    TTree     *result;
    char       output[1024];

    if (strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Enter a name.");
        return;
    }
    result = getInfoNameTree(g_bst, (char *)name);
    if (result == NULL) {
        gtk_label_set_text(GTK_LABEL(p->output_label), "Not found in BST.");
        return;
    }
    snprintf(output, sizeof(output),
        "Name : %s\nBorn : %s\nDied : %s\n\n%s",
        result->name, result->DoB, result->DoD, result->definition);
    gtk_label_set_text(GTK_LABEL(p->output_label), output);
}

static void on_bst_inorder(GtkWidget *btn, gpointer data) {
    UiPack *p = (UiPack *)data;
    char   *out;
    if (g_bst == NULL) { gtk_label_set_text(GTK_LABEL(p->output_label), "No BST loaded."); return; }
    out = capture_stdout(call_inorder, g_bst);
    gtk_label_set_text(GTK_LABEL(p->output_label), out);
    free(out);
}

static void on_bst_preorder(GtkWidget *btn, gpointer data) {
    UiPack *p = (UiPack *)data;
    char   *out;
    if (g_bst == NULL) { gtk_label_set_text(GTK_LABEL(p->output_label), "No BST loaded."); return; }
    out = capture_stdout(call_preorder, g_bst);
    gtk_label_set_text(GTK_LABEL(p->output_label), out);
    free(out);
}

static void on_bst_postorder(GtkWidget *btn, gpointer data) {
    UiPack *p = (UiPack *)data;
    char   *out;
    if (g_bst == NULL) { gtk_label_set_text(GTK_LABEL(p->output_label), "No BST loaded."); return; }
    out = capture_stdout(call_postorder, g_bst);
    gtk_label_set_text(GTK_LABEL(p->output_label), out);
    free(out);
}

static void on_bst_height(GtkWidget *btn, gpointer data) {
    UiPack *p = (UiPack *)data;
    char   *out;
    if (g_bst == NULL) { gtk_label_set_text(GTK_LABEL(p->output_label), "No BST loaded."); return; }
    out = capture_stdout((void (*)(TTree *))heightSizeBST, g_bst);
    gtk_label_set_text(GTK_LABEL(p->output_label), out);
    free(out);
}

static void on_bst_mirror(GtkWidget *btn, gpointer data) {
    UiPack *p = (UiPack *)data;
    if (g_bst == NULL) { gtk_label_set_text(GTK_LABEL(p->output_label), "No BST loaded."); return; }
    g_bst = BSTMirror(g_bst);
    gtk_label_set_text(GTK_LABEL(p->output_label), "BST mirrored successfully.");
}

static void on_bst_balance(GtkWidget *btn, gpointer data) {
    UiPack *p = (UiPack *)data;
    if (g_bst == NULL) { gtk_label_set_text(GTK_LABEL(p->output_label), "No BST loaded."); return; }
    gtk_label_set_text(GTK_LABEL(p->output_label),
        isBalancedBST(g_bst) ? "BST is balanced." : "BST is NOT balanced.");
}



/* =========================================================
 * LIVE SEARCH FILTER
 * ========================================================= */

static gboolean filter_personalities(GtkListBoxRow *row, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *search_text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (search_text == NULL || strlen(search_text) == 0) return TRUE;

    GtkWidget *lbl = gtk_bin_get_child(GTK_BIN(row));
    const char *row_text = gtk_label_get_text(GTK_LABEL(lbl));

    char *lower_row = g_ascii_strdown(row_text, -1);
    char *lower_search = g_ascii_strdown(search_text, -1);

    gboolean match = (strstr(lower_row, lower_search) != NULL);

    g_free(lower_row);
    g_free(lower_search);

    return match;
}

static void on_search_changed(GtkEditable *editable, gpointer user_data) {
    GtkListBox *list_box = GTK_LIST_BOX(user_data);
    gtk_list_box_invalidate_filter(list_box);
}

/* =========================================================
 * BUILD TAB: PERSONALITIES
 * Left panel  : sort toolbar + scrollable personality list
 * Right panel : Add / Delete / Update / Find Similar frames
 * ========================================================= */

static GtkWidget *build_personalities_tab(GtkWidget **out_list_box) {
    GtkWidget *paned;
    GtkWidget *left_box;
    GtkWidget *right_scroll;
    GtkWidget *right_box;
    GtkWidget *toolbar;
    GtkWidget *list_box;
    GtkWidget *scroll;
    GtkWidget *result_label;
    GtkWidget *frame_box;
    GtkWidget *sort_a;
    GtkWidget *sort_l;
    GtkWidget *sort_age;
    GtkWidget *add_name;
    GtkWidget *add_dob;
    GtkWidget *add_dod;
    GtkWidget *add_btn;
    GtkWidget *del_name;
    GtkWidget *del_btn;
    GtkWidget *upd_name;
    GtkWidget *upd_def;
    GtkWidget *upd_btn;
    GtkWidget *sim_name;
    GtkWidget *sim_btn;
    GtkWidget *search_entry;
    UiPack    *pack;

    /* ---- LIVE SEARCH FILTERS ---- */


    /* ---- LEFT: toolbar + list ---- */
    left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(left_box), 8);

    toolbar  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    sort_a   = gtk_button_new_with_label("Sort A-Z");
    sort_l   = gtk_button_new_with_label("Sort by Length");
    sort_age = gtk_button_new_with_label("Sort by Age");
    gtk_box_pack_start(GTK_BOX(toolbar), sort_a,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), sort_l,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), sort_age, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left_box), toolbar, FALSE, FALSE, 0);

    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Search personalities...");
    gtk_box_pack_start(GTK_BOX(left_box), search_entry, FALSE, FALSE, 0);

    list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_SINGLE);
    g_signal_connect(list_box, "row-activated", G_CALLBACK(on_row_activated), NULL);

    /* LIVE SEARCH: connect signals and filter */
    g_signal_connect(search_entry, "changed", G_CALLBACK(on_search_changed), list_box);
    gtk_list_box_set_filter_func(GTK_LIST_BOX(list_box), filter_personalities, search_entry, NULL);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), list_box);
    gtk_box_pack_start(GTK_BOX(left_box), scroll, TRUE, TRUE, 0);

    g_signal_connect(sort_a,   "clicked", G_CALLBACK(on_sort_alpha),  list_box);
    g_signal_connect(sort_l,   "clicked", G_CALLBACK(on_sort_length), list_box);
    g_signal_connect(sort_age, "clicked", G_CALLBACK(on_sort_age),    list_box);

    /* ---- RIGHT: action frames ---- */
    result_label = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(result_label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(result_label), TRUE);
    gtk_widget_set_halign(result_label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(result_label, 4);

    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 8);

    /* ADD frame */
    add_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(add_name), "Full name");
    add_dob  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(add_dob), "Date of Birth (e.g. 1920)");
    add_dod  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(add_dod), "Date of Death (e.g. 1957)");
    add_btn  = gtk_button_new_with_label("Add");
    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(frame_box), 6);
    gtk_box_pack_start(GTK_BOX(frame_box), add_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), add_dob,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), add_dod,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), add_btn,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("Add Personality", frame_box), FALSE, FALSE, 0);
    pack = g_malloc(sizeof(UiPack));
    pack->input1 = add_name; pack->input2 = add_dob; pack->input3 = add_dod;
    pack->output_label = result_label; pack->list_box = list_box;
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_personality), pack);

    /* DELETE frame */
    del_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(del_name), "Name to delete");
    del_btn  = gtk_button_new_with_label("Delete");
    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(frame_box), 6);
    gtk_box_pack_start(GTK_BOX(frame_box), del_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), del_btn,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("Delete Personality", frame_box), FALSE, FALSE, 0);
    pack = g_malloc(sizeof(UiPack));
    pack->input1 = del_name; pack->input2 = NULL; pack->input3 = NULL;
    pack->output_label = result_label; pack->list_box = list_box;
    g_signal_connect(del_btn, "clicked", G_CALLBACK(on_delete_personality), pack);

    /* UPDATE frame */
    upd_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(upd_name), "Name to update");
    upd_def  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(upd_def), "New definition");
    upd_btn  = gtk_button_new_with_label("Update");
    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(frame_box), 6);
    gtk_box_pack_start(GTK_BOX(frame_box), upd_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), upd_def,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), upd_btn,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("Update Personality", frame_box), FALSE, FALSE, 0);
    pack = g_malloc(sizeof(UiPack));
    pack->input1 = upd_name; pack->input2 = upd_def; pack->input3 = NULL;
    pack->output_label = result_label; pack->list_box = list_box;
    g_signal_connect(upd_btn, "clicked", G_CALLBACK(on_update_personality), pack);

    /* SIMILAR frame */
    sim_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(sim_name), "Year to search (e.g. 1954)");
    sim_btn  = gtk_button_new_with_label("Find by Date");
    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(frame_box), 6);
    gtk_box_pack_start(GTK_BOX(frame_box), sim_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), sim_btn,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("Find Personalities by Year/Date", frame_box), FALSE, FALSE, 0);
    pack = g_malloc(sizeof(UiPack));
    pack->input1 = sim_name; pack->input2 = NULL; pack->input3 = NULL;
    pack->output_label = result_label; pack->list_box = list_box;
    g_signal_connect(sim_btn, "clicked", G_CALLBACK(on_find_similar), pack);

    /* Result frame at bottom of right panel */
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("Result", result_label), FALSE, FALSE, 0);

    right_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(right_scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(right_scroll), right_box);

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_pack1(GTK_PANED(paned), left_box,     TRUE,  TRUE);
    gtk_paned_pack2(GTK_PANED(paned), right_scroll, FALSE, FALSE);
    gtk_paned_set_position(GTK_PANED(paned), 520);

    *out_list_box = list_box;
    return paned;
}



/* =========================================================
 * BUILD TAB: BST
 * Left  : search entry + large scrollable output text area
 * Right : Traversals frame + BST Operations frame
 * ========================================================= */

static GtkWidget *build_bst_tab(void) {
    GtkWidget *paned;
    GtkWidget *left_box;
    GtkWidget *right_box;
    GtkWidget *search_entry;
    GtkWidget *search_btn;
    GtkWidget *scroll;
    GtkWidget *result_label;
    GtkWidget *frame_box;
    UiPack    *search_pack;
    UiPack    *btn_pack;

    result_label = gtk_label_new("Load a file first, then use the buttons on the right.");
    gtk_label_set_line_wrap(GTK_LABEL(result_label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(result_label), TRUE);
    gtk_widget_set_halign(result_label, GTK_ALIGN_START);
    gtk_widget_set_valign(result_label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(result_label, 6);

    /* ---- LEFT ---- */
    left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(left_box), 8);

    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter name to search in BST");
    search_btn = gtk_button_new_with_label("Search");

    search_pack = g_malloc(sizeof(UiPack));
    search_pack->input1 = search_entry; search_pack->input2 = NULL;
    search_pack->output_label = result_label; search_pack->list_box = NULL;
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_bst_search), search_pack);

    gtk_box_pack_start(GTK_BOX(left_box), search_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(left_box), search_btn,   FALSE, FALSE, 0);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), result_label);
    gtk_box_pack_start(GTK_BOX(left_box), scroll, TRUE, TRUE, 0);

    /* ---- RIGHT ---- */
    btn_pack = g_malloc(sizeof(UiPack));
    btn_pack->input1 = NULL; btn_pack->input2 = NULL;
    btn_pack->output_label = result_label; btn_pack->list_box = NULL;

    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 8);

    /* Traversals frame */
    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(frame_box), 6);
    {
        GtkWidget *b1 = gtk_button_new_with_label("In-Order");
        GtkWidget *b2 = gtk_button_new_with_label("Pre-Order");
        GtkWidget *b3 = gtk_button_new_with_label("Post-Order");
        g_signal_connect(b1, "clicked", G_CALLBACK(on_bst_inorder),   btn_pack);
        g_signal_connect(b2, "clicked", G_CALLBACK(on_bst_preorder),  btn_pack);
        g_signal_connect(b3, "clicked", G_CALLBACK(on_bst_postorder), btn_pack);
        gtk_box_pack_start(GTK_BOX(frame_box), b1, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(frame_box), b2, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(frame_box), b3, FALSE, FALSE, 0);
    }
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("Traversals", frame_box), FALSE, FALSE, 0);

    /* BST Operations frame */
    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(frame_box), 6);
    {
        GtkWidget *b1 = gtk_button_new_with_label("Height & Size");
        GtkWidget *b2 = gtk_button_new_with_label("Is Balanced?");
        GtkWidget *b3 = gtk_button_new_with_label("Mirror BST");
        g_signal_connect(b1, "clicked", G_CALLBACK(on_bst_height),  btn_pack);
        g_signal_connect(b2, "clicked", G_CALLBACK(on_bst_balance), btn_pack);
        g_signal_connect(b3, "clicked", G_CALLBACK(on_bst_mirror),  btn_pack);
        gtk_box_pack_start(GTK_BOX(frame_box), b1, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(frame_box), b2, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(frame_box), b3, FALSE, FALSE, 0);
    }
    gtk_box_pack_start(GTK_BOX(right_box),
                       make_frame("BST Operations", frame_box), FALSE, FALSE, 0);

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_pack1(GTK_PANED(paned), left_box,  TRUE,  TRUE);
    gtk_paned_pack2(GTK_PANED(paned), right_box, FALSE, FALSE);
    gtk_paned_set_position(GTK_PANED(paned), 600);

    return paned;
}

/* =========================================================
 * BUILD TAB: EVENTS
 * ========================================================= */

static GtkWidget *build_events_tab(GtkWidget **out_events_list) {
    GtkWidget *paned;
    GtkWidget *left_box;
    GtkWidget *right_box;
    GtkWidget *list_box;
    GtkWidget *scroll;
    GtkWidget *frame_box;
    GtkWidget *ev_name;
    GtkWidget *ev_date;
    GtkWidget *ev_btn;
    GtkWidget *result_label;
    UiPack    *pack;

    left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(left_box), 8);
    gtk_box_pack_start(GTK_BOX(left_box),
        gtk_label_new("Historical events — click a row to see its description."),
        FALSE, FALSE, 4);

    list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_SINGLE);
    g_signal_connect(list_box, "row-activated", G_CALLBACK(on_event_row_activated), NULL);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), list_box);
    gtk_box_pack_start(GTK_BOX(left_box), scroll, TRUE, TRUE, 0);

    result_label = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(result_label), TRUE);
    gtk_widget_set_halign(result_label, GTK_ALIGN_START);

    ev_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ev_name), "Event name");
    ev_date = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ev_date), "Date (e.g. 01/11/1954)");
    ev_btn  = gtk_button_new_with_label("Add Event");

    pack = g_malloc(sizeof(UiPack));
    pack->input1 = ev_name; pack->input2 = ev_date;
    pack->output_label = result_label; pack->list_box = list_box;
    g_signal_connect(ev_btn, "clicked", G_CALLBACK(on_add_event), pack);

    frame_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start(GTK_BOX(frame_box), ev_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), ev_date, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(frame_box), ev_btn,  FALSE, FALSE, 0);

    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 8);
    gtk_box_pack_start(GTK_BOX(right_box), make_frame("Add Event", frame_box), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(right_box), make_frame("Result", result_label), FALSE, FALSE, 0);

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_pack1(GTK_PANED(paned), left_box,  TRUE,  TRUE);
    gtk_paned_pack2(GTK_PANED(paned), right_box, FALSE, FALSE);
    gtk_paned_set_position(GTK_PANED(paned), 520);

    *out_events_list = list_box;
    return paned;
}

/* =========================================================
 * ACTIVATE — assembles the main window
 * ========================================================= */

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *header_bar;
    GtkWidget *stack;
    GtkWidget *stack_switcher;
    GtkWidget *open_btn;
    GtkWidget *personalities_list;
    GtkWidget *events_list;

    LoadPack  *lpack;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "AlgDB - History of Algeria");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 680);

    header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    /* Open File button sits in the header bar, always visible */
    open_btn = gtk_button_new_with_label("Open File");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), open_btn);

    /* build tabs and get pointers to both list boxes */
    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack),
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_add_titled(GTK_STACK(stack),
        build_personalities_tab(&personalities_list), "p", "Personalities");

    gtk_stack_add_titled(GTK_STACK(stack),
        build_events_tab(&events_list),               "e", "Events");

    gtk_stack_add_titled(GTK_STACK(stack),
        build_bst_tab(),                              "b", "BST");

    /* wire Open File to refresh both list boxes after loading */
    lpack = g_malloc(sizeof(LoadPack));
    lpack->personalities_list = personalities_list;
    lpack->events_list = events_list;
    g_signal_connect(open_btn, "clicked", G_CALLBACK(on_open_file_clicked), lpack);

    stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher),
                                 GTK_STACK(stack));
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header_bar), stack_switcher);

    gtk_container_add(GTK_CONTAINER(window), stack);
    gtk_widget_show_all(window);
}

/* =========================================================
 * MAIN
 * ========================================================= */

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    app = gtk_application_new("dz.nscs.algdb", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

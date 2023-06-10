#ifndef LIST_H
#define LIST_H
#include <gtk/gtk.h>

void add_single_transaction_from_entryboxes(GtkWidget *widget, gpointer model);
unsigned char add_all_rows(GtkTreeModel *model);
void amount_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data);
void toggle_paid(GtkCellRendererToggle *cell_render, gchar *path, gpointer args_struct);
#endif

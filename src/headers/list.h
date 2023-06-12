#ifndef LIST_H
#define LIST_H
#include <gtk/gtk.h>
#include "config.h"

void add_single_transaction_from_entryboxes(GtkWidget *widget, gpointer model);
unsigned char add_all_rows(GtkTreeModel *model);
void amount_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data);
void toggle_paid(GtkCellRendererToggle *cell_render, gchar *path, struct treeview_models *models);
void date_edited_callback (GtkCellRendererText *cell, char *path_string, char *new_text, struct treeview_models *models);
void customer_edited_callback (GtkCellRendererText *cell, char *path_string, char *new_text, struct treeview_models *models);
void method_edited_callback (GtkCellRendererText *cell, char *path_string, char *new_text, struct treeview_models *models);
void amount_edited_callback (GtkCellRendererText *cell, char *path_string, char *new_text, struct treeview_models *models);
#endif

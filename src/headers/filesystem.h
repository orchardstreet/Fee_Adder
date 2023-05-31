#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <gtk/gtk.h>

unsigned char load_items(GtkListStore *model);
void save_items(GtkWidget *widget, gpointer model_void);

#endif

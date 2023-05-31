#ifndef UTILS_H
#define UTILS_H
#include <gtk/gtk.h>

enum endianness {IS_BIG_ENDIAN,IS_LITTLE_ENDIAN};

unsigned char check_endianness(void);
void skip_whitespace(char **text_skip);
char * strsep_custom(char **str,char delim);
unsigned char truncate_double(double *temp_double);
gboolean keypress_function(GtkWidget *widget, GdkEventKey *event, gpointer data);
void scroll_to_end (GtkWidget *widget, GdkRectangle *allocate, gpointer user_data);

#endif

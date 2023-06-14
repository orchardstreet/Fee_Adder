#ifndef UTILS_H
#define UTILS_H
#include <gtk/gtk.h>
#include <stdarg.h>

enum endianness {IS_BIG_ENDIAN,IS_LITTLE_ENDIAN};

unsigned char check_endianness(void);
void skip_whitespace(char **text_skip);
void skip_zeros(char **text_skip);
unsigned long long pow_custom(unsigned char base,unsigned char exponent);
char * strsep_custom(char **str,char delim);
gboolean keypress_function(GtkWidget *widget, GdkEventKey *event, gpointer data);
void scroll_to_end (void);
unsigned char count_digits (unsigned long long n);
unsigned char check_system_compatibility(void);
void dialog_popup(char *header, const char* body_fmt, ...);

#endif

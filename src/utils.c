#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/config.h"
#include "headers/utils.h"

void skip_whitespace(char **text_skip) {
	for(;**text_skip == ' ';(*text_skip)++) {};
}

void skip_zeros(char **text_skip) {
	for(;**text_skip == '0';(*text_skip)++) {};
}

unsigned char count_digits (unsigned long long n)
{
    if (n == 0)
        return 1;
    unsigned char i = 0;
    while (n != 0) {
        n = n / 10;
        ++i;
    }
    return i;
}

unsigned long long pow_custom(unsigned char base,unsigned char exponent)
{
	long long result = 1;
	int i = 0;
	for (;i <  exponent;i++) {
		result = result * base;

	}
	return result;

}

unsigned char
check_endianness(void)
{

        unsigned short a=0x1234;
        if (*((unsigned char *)&a)==0x12)
                return IS_BIG_ENDIAN;
        else
                return IS_LITTLE_ENDIAN;

}

char * strsep_custom(char **str,char delim) {

        char *save = *str;
        if(!(**str)) {
                return NULL;
        }
        *str = strchr(*str,delim);
        if(!(*str)) {
                return save;
                *str = NULL;
        }
        **str = 0;
        (*str)++;
        return save;
}

gboolean keypress_function(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	/*
	if (event->keyval == GDK_KEY_space){
		printf("test!\n");
		return TRUE;
	}
	*/
	return FALSE;
}

void scroll_to_end (GtkWidget *widget, GdkRectangle *allocate, gpointer user_data)
{
	GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment (
	GTK_SCROLLED_WINDOW (scrolled_window));
	double upper = gtk_adjustment_get_upper (adjustment);
	double page_size = gtk_adjustment_get_page_size (adjustment);
	gtk_adjustment_set_value (adjustment, upper - page_size - 0);
}

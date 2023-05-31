#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/config.h"
#include "headers/utils.h"

void skip_whitespace(char **text_skip) {
	for(;**text_skip == ' ';(*text_skip)++) {};
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

unsigned char truncate_double(double *temp_double)
{

	char string_from_double[50] = {0};
	char *endptr;

	snprintf(string_from_double,sizeof(string_from_double),"%.2lf",*temp_double);
	printf("what snprintf produced trying to convert double to string: %s\n",string_from_double);
	*temp_double = strtod(string_from_double,&endptr);
	if(endptr == string_from_double) {
		gtk_label_set_text( GTK_LABEL(total_results_label), "error");
		gtk_label_set_text( GTK_LABEL(total_filtered_results_label), "error");
		gtk_text_buffer_set_text(error_buffer,"snprintf() outputting garbage",-1);
		fprintf(stderr,"can't convert to double from snprintf created string\n"
				"snprintf() outputting garbage\n");
		return FAILURE;
	}
	return SUCCESS;

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

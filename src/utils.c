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

void scroll_to_end (void)
{
	if(is_scrolling){
		while(gtk_events_pending()) {
			gtk_main_iteration();
		}
		GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment (
		GTK_SCROLLED_WINDOW (scrolled_window));
		double upper = gtk_adjustment_get_upper (adjustment);
		double page_size = gtk_adjustment_get_page_size (adjustment);
		gtk_adjustment_set_value (adjustment, upper - page_size - 0);
	}
}

unsigned char check_system_compatibility(void)
{

	printf("Size of unsigned char: %zu\nSize of unsigned int: %zu\n"
			"Size of unsigned long: %zu\nSize of unsigned long long: %zu\n"
			,sizeof(unsigned char),sizeof(unsigned int)
			,sizeof(unsigned long),sizeof(unsigned long long));

	if(check_endianness() == IS_BIG_ENDIAN) {
		gtk_text_buffer_set_text(error_buffer,"Computer should be litte endian, please exit to avoid data issues\n",-1);
		fprintf(stderr,"Computer should be little endian, please exit to avoid data issues\n");
        return FAILURE;
	}
	if (sizeof(void *) != 8) {
		gtk_text_buffer_set_text(error_buffer,"Error: please use a 64-bit computer, please exit to avoid data issues\n",-1);
		fprintf(stderr,"Error: please use a 64-bit computer, please exit to avoid data issues\n");
        return FAILURE;
    }
	/* warn if compiler didn't create correct length for types */
	if(sizeof(unsigned char) != 1) {
		gtk_text_buffer_set_text(error_buffer,"size of unsigned char must equal 1, please exit to avoid data issues\n",-1);
		fprintf(stderr,"size of unsigned char must equal 1, please exit to avoid data issues\n");
        return FAILURE;
	}
	if(sizeof(unsigned int) != 4 || sizeof(signed int) != 4 || sizeof(int) !=  4) {
		gtk_text_buffer_set_text(error_buffer,"size of unsigned int must equal 4, please exit to avoid data issues\n",-1);
		fprintf(stderr,"size of unsigned int must equal 4, please exit to avoid data issues\n");
        return FAILURE;
	}
	if(sizeof(unsigned long) != 8 && sizeof(unsigned long) != 4) {
		gtk_text_buffer_set_text(error_buffer,"size of long must equal 8 or 4, please exit to avoid data issues\n",-1);
		fprintf(stderr,"size of long must equal 8 or 4, please exit to avoid data issues\n");
        return FAILURE;
	}
	if(sizeof(unsigned long long) != 8) {
		gtk_text_buffer_set_text(error_buffer,"size of unsigned long long must equal 8, please exit to avoid data issues\n",-1);
		fprintf(stderr,"size of unsigned long long must equal 8, please exit to avoid data issues\n");
        return FAILURE;
	}

	return SUCCESS;

}

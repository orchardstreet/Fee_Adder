#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/config.h"
#include "headers/utils.h"
#include <stdarg.h>

enum arg_types {NONE_C, U_LONG_LONG_C, STRING_C};

void dialog_popup(char *header, const char *body_fmt, ...)
{
	const char *browse = body_fmt;
	const char *arg_type_location;
	unsigned char arg_type = NONE_C;
	GtkWidget *message;

	va_list arglist;
	va_start(arglist,body_fmt);

    message = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "%s",header);

	/* find format args if any */
	for(;*browse;browse++) {
		if(*browse == '\\' && *(browse + 1) == '%') {
			browse++;
			continue;
		}
		if(*browse == '%') {
			arg_type_location = browse + 1;
			/* TODO check this for bugs */
			if(!strncmp(arg_type_location,"llu",3)) {
				arg_type = U_LONG_LONG_C;
			} else if (*arg_type_location == 's') {
				arg_type = STRING_C;
			}
			break;
		}
	}

	/* create message in window according to args, only supports no arguments or %llu right now */
	switch(arg_type) {
		case NONE_C:
			gtk_message_dialog_format_secondary_text( GTK_MESSAGE_DIALOG(message), "%s","body_fmt");
			break;
		case U_LONG_LONG_C:
			gtk_message_dialog_format_secondary_text( GTK_MESSAGE_DIALOG(message), body_fmt,va_arg(arglist,unsigned long long));
			break;
		case STRING_C:
			gtk_message_dialog_format_secondary_text( GTK_MESSAGE_DIALOG(message), body_fmt,va_arg(arglist,char *));
			break;
		default:
			gtk_message_dialog_format_secondary_text( GTK_MESSAGE_DIALOG(message), "%s","body_fmt");
			break;
	}

	va_end(arglist);

	/* launch dialog and wait for user to press 'ok' */
    int response = gtk_dialog_run(GTK_DIALOG(message));
	(void)response;

    //printf("response was %d (OK=%d, DELETE_EVENT=%d)\n", response, GTK_RESPONSE_OK, GTK_RESPONSE_DELETE_EVENT);

	/* destroy dialog */
    gtk_widget_destroy(message);
}

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

	printf("Checking system compatibility...\nSize of unsigned char: %zu\nSize of unsigned int: %zu\n"
			"Size of unsigned long: %zu\nSize of unsigned long long: %zu\n"
			,sizeof(unsigned char),sizeof(unsigned int)
			,sizeof(unsigned long),sizeof(unsigned long long));

	if(check_endianness() == IS_BIG_ENDIAN) {
		dialog_popup("Incompatible system","Computer should be litte endian");
		fprintf(stderr,"Computer should be little endian, please exit to avoid data issues\n");
        return FAILURE;
	}
	if (sizeof(void *) != 8) {
		dialog_popup("Incompatible system","please use a 64-bit computer");
		fprintf(stderr,"Error: please use a 64-bit computer, please exit to avoid data issues\n");
        return FAILURE;
    }
	/* warn if compiler didn't create correct length for types */
	if(sizeof(unsigned char) != 1) {
		dialog_popup("Incompatible system","size of unsigned char must equal 1");
		fprintf(stderr,"size of unsigned char must equal 1, please exit to avoid data issues\n");
        return FAILURE;
	}
	if(sizeof(unsigned int) != 4 || sizeof(signed int) != 4 || sizeof(int) !=  4) {
		dialog_popup("Incompatible system","size of unsigned int must equal 4");
		fprintf(stderr,"size of unsigned int must equal 4, please exit to avoid data issues\n");
        return FAILURE;
	}
	if(sizeof(unsigned long) != 8 && sizeof(unsigned long) != 4) {
		dialog_popup("Incompatible system","size of long must equal 8 or 4");
		fprintf(stderr,"size of long must equal 8 or 4, please exit to avoid data issues\n");
        return FAILURE;
	}
	if(sizeof(unsigned long long) != 8) {
		dialog_popup("Incompatible system","size of unsigned long long must equal 8");
		fprintf(stderr,"size of unsigned long long must equal 8, please exit to avoid data issues\n");
        return FAILURE;
	}

	return SUCCESS;

}

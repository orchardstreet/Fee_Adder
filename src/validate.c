#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "headers/validate.h"
#include "headers/config.h"
#include "headers/utils.h"

unsigned char validate_day(unsigned long *number)
{
	if(*number < 0 || *number > MAX_DAY_NUMBER)
		return FAILURE;
	return SUCCESS;
}

unsigned char validate_month(unsigned long *number)
{
	if(*number < 0 || *number > MAX_MONTH_NUMBER)
		return FAILURE;
	return SUCCESS;
}

unsigned char validate_year(unsigned long *number)
{
	if(*number < 0 || *number > MAX_YEAR_NUMBER)
		return FAILURE;
	return SUCCESS;
}

unsigned char validate_amount(char *str, double *number)
{

	char *endptr;

	if (!strlen(str)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a fee amount",-1);
		return FAILURE;
	}
	if(strlen(str) > MAX_AMOUNT_CHARS - 1)  {
		gtk_text_buffer_set_text(error_buffer,"Too many characters entered for amount",-1);
		return FAILURE;
	}

	*number = strtod(str,&endptr);

	if(endptr == str) {
		gtk_text_buffer_set_text(error_buffer,"Fee amount entered has an invalid format."
				" Please enter ascii digits.  You can include a decimal.",-1);
		return FAILURE;
//	} else if (*number >= HUGE_VAL || *number <= -HUGE_VAL) {
//		gtk_text_buffer_set_text(error_buffer,"Fee amount entered has too many digits",-1);
//		return FAILURE;
	} else if (*number <= -7777777777.55 || *number >= 7777777777.55) {
		gtk_text_buffer_set_text(error_buffer,"Fee amount out of range, should be between"
				" -7777777777.55 and 7777777777.55",-1);
		return FAILURE;

	} else if (*endptr != '\0') {
		skip_whitespace(&endptr);
		if(*endptr != '\0') {
			gtk_text_buffer_set_text(error_buffer,"Only digits in fee amount please",-1);
			return FAILURE;
		}
	}

	return SUCCESS;

}

unsigned char validate_person(char *text)
{
	if(!strlen(text)) {
		fprintf(stderr,"Please enter a person\n");
		gtk_text_buffer_set_text(error_buffer,"Please enter a person",-1);
		return FAILURE;
	}
	if(strlen(text) > MAX_PERSON_CHARS - 1)  {
		fprintf(stderr,"Name of person is too long\n");
		gtk_text_buffer_set_text(error_buffer,"Name of person is too long, "
				"recompile to allow longer person names",-1);
		return FAILURE;
	}
	if(strchr(text,',')) {
		fprintf(stderr,"Customer name cannot contain commas\n");
		gtk_text_buffer_set_text(error_buffer,"Customer name cannot contain commas",-1);
		return FAILURE;
	}
	if(strchr(text,'\n')) {
		fprintf(stderr,"Customer name cannot contain newlines\n");
		gtk_text_buffer_set_text(error_buffer,"Customer name cannot contain newlines",-1);
		return FAILURE;
	}
	return SUCCESS;
}

unsigned char validate_method(char *text)
{
	if(strlen(text) > MAX_METHOD_CHARS - 1)  {
		gtk_text_buffer_set_text(error_buffer,"Name of payment method is too long, "
				"recompile to allow longer method names",-1);
		return FAILURE;
	}
	if(strchr(text,',')) {
		gtk_text_buffer_set_text(error_buffer,"Method name cannot contain commas",-1);
		return FAILURE;
	}
	if(strchr(text,'\n')) {
		gtk_text_buffer_set_text(error_buffer,"Method name cannot contain newlines",-1);
		return FAILURE;
	}
	return SUCCESS;
}
unsigned char validate_date (char *text, unsigned int *year_s, unsigned char *month_s, unsigned char *day_s)
{
	unsigned long number;
	char *endptr;

	/* return failed if nothing in string */
	if(!strlen(text)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a date",-1);
		return FAILURE;
	}
	if(strlen(text) > MAX_DATE_CHARS - 1) {
		gtk_text_buffer_set_text(error_buffer,"Entered too many characters, must be in dd/mm/yy format",-1);
		return FAILURE;
	}

	/* skip whitespace */
	skip_whitespace(&text);
	if(*text == '\0') {
		gtk_text_buffer_set_text(error_buffer,"0: Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}
	/* find day */
	number = strtoul(text,&endptr,10);
	if(endptr == text || *endptr != '/' || (validate_day(&number) == FAILURE)) {
		gtk_text_buffer_set_text(error_buffer,"Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}
	*day_s = number;
	text = endptr + 1;
	/* find month */
	number = strtoul(text,&endptr,10);
	if(endptr == text || *endptr != '/' || (validate_month(&number) == FAILURE)) {
		gtk_text_buffer_set_text(error_buffer,"Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}
	*month_s = number;
	text = endptr + 1;
	/* find year */
	number = strtoul(text,&endptr,10);
	if(endptr == text || (validate_year(&number) == FAILURE) || !(*endptr == '\0' || *endptr == ' ')) {
		gtk_text_buffer_set_text(error_buffer,"Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}
	*year_s = number;
	text = endptr;
	/* skip whitespace */
	skip_whitespace(&text);
	if(*text != '\0') {
		gtk_text_buffer_set_text(error_buffer,"Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}

	return SUCCESS;
}

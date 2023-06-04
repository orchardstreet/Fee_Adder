#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "headers/validate.h"
#include "headers/config.h"
#include "headers/utils.h"

unsigned char cent_string_to_cents(char *str,unsigned long long *number) {
	char *endptr;
	*number = 0;
	unsigned long long result;
	if(!(*str)) {
		*number = 0;
		return SUCCESS;
	}
	result = strtoull(str,&endptr,10);
	if(str == endptr) {
		gtk_text_buffer_set_text(error_buffer,"item in cent string contains invalid characters",-1);
		fprintf(stderr,"item in cent string contains invalid characters\n");
		return FAILURE;
	} else if (*endptr != '\0') {
		gtk_text_buffer_set_text(error_buffer,"invalid character in cent string, terminating operation",-1);
		fprintf(stderr,"invalid character in cent string, terminating operation\n");
		return FAILURE;
	} else if (result >= UINT64_MAX) {
		gtk_text_buffer_set_text(error_buffer,"item in cent string is too large, setting to 0",-1);
		fprintf(stderr,"item in cent string is too large, setting to 0\n");
		*number = 0;
		return SUCCESS;
	}
	*number = result;
	return SUCCESS;

}
unsigned char cents_to_string(unsigned long long number, char *str) {

	int retval;
	unsigned char num_digits_right_side;
	unsigned char right_side_offset;

	memset(str,'0',MAX_AMOUNT_CHARS - 1);
	str[MAX_AMOUNT_CHARS - 1] = 0;
	unsigned long long integer_digits = number / pow_custom(10,DECIMAL_PLACES);
	unsigned long long fractional_digits = number % pow_custom(10,DECIMAL_PLACES);
	num_digits_right_side = count_digits(fractional_digits);
	retval = snprintf(str,MAX_AMOUNT_CHARS,"%llu.",integer_digits);
	if(retval < 1) {
		fprintf(stderr,"snprint failed in llu_to_string\n");
		gtk_text_buffer_set_text(error_buffer,"snprintf failed in llu_to_string\n",-1);
		return FAILURE;
	}
	right_side_offset = DECIMAL_PLACES - num_digits_right_side;
	//printf("offset: %u\n",right_side_offset);
	//printf("left: %llu",MAX_AMOUNT_CHARS - retval - 1 - right_side_offset);
	if(MAX_AMOUNT_CHARS - retval - 1 - right_side_offset < 0) {
		fprintf(stderr,"no space to hold fractional digits in string\n");
		gtk_text_buffer_set_text(error_buffer,"no space to hold fractional digits in string\n",-1);
		return FAILURE;
	}
	str[retval] = '0';
	snprintf(str + retval + right_side_offset,MAX_AMOUNT_CHARS - retval - 1 - right_side_offset,"%llu",fractional_digits);
	return SUCCESS;

}
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

unsigned char validate_fractional_digits(char *str, unsigned long long *fractional_digits) {
	unsigned char i;
	unsigned char digit;
	*fractional_digits = 0;
	for(i = 0;i < DECIMAL_PLACES; i++) {
		if (!str[i]) {
			return FINISHED;
		}
		if (str[i] == ' ') {
			skip_whitespace(&str);
			if(!(*str)) {
				return FINISHED;
			} else {
				return FAILURE;
			}
		}
		if (str[i] >= '0' && str[i] <= '9') {
			digit = str[i] - 48;
			*fractional_digits += digit * pow_custom(10,DECIMAL_PLACES - 1 - i);
		} else {
			return FAILURE;
		}
	}

	return SUCCESS;

}


unsigned char validate_integer_digits(char **str_ptr,unsigned long long *number)
{
	char *endptr;

	/* if no integer digits present, return function to parse fractional digits */
	if(**str_ptr == '.') {
		(*str_ptr)++;
		return SUCCESS;
	}
	*number = strtoull(*str_ptr,&endptr,10);
	//printf("strtuoll result: %llu\n",*number);
	if(endptr == *str_ptr) {
		fprintf(stderr,"please enter a valid amount\n");
		*number = 0;
		return FAILURE;
	} else if (*number == UINT64_MAX || (((*number) * 100) > (UINT64_MAX - (pow_custom(10,DECIMAL_PLACES))))) {
		fprintf(stderr,"integer digits too large, please enter a valid amount\n");
		*number = 0;
		return FAILURE;
	}
	*number = *number * pow_custom(10,DECIMAL_PLACES);
	*str_ptr = endptr;

	//printf("ending character: %c\n",**str_ptr);

	/*hanld space after integer digits */
	if(**str_ptr == ' ') {
		skip_whitespace(str_ptr);
		/* successful conversion of only integer digits */
		if(**str_ptr == '\0') {
			return FINISHED;

		/* if invalid character after integer digits, return FAILURE */
		} else {
			fprintf(stderr,"invalid characters in amount field, please enter a valid amount\n");
			*number = 0;
			return FAILURE;
		}
	/* successful conversion of only integer digits */
	} else if (**str_ptr == '\0') {
		return FINISHED;
	/* if invalid character after integer digits, return FAILURE */
	} else if (**str_ptr != '.') {
			fprintf(stderr,"invalid characters in amount field, please enter a valid amount\n");
			*number = 0;
			return FAILURE;
	}
	/* skipping one character past period '.' */
	(*str_ptr)++;
	return SUCCESS;

}
// converts money amount into cents (or foreign equivalent, such as agorot)
// the cent amount is stored in an unsigned long long
// and can't exceed UINT64_MAX ever
//   integer digits (222) -> 222.345 <-  fractional digits (345)
unsigned char validate_amount(char *str_ptr, unsigned long long *number)
{

	unsigned long long fractional_digits = 0;
	unsigned char retval;
	size_t string_length = strlen(str_ptr);
	*number = 0;
	/* get integer digits */
	if(string_length > MAX_AMOUNT_CHARS - 1) {
		*number = 0;
		fprintf(stderr,"amount entry contains too many characters\n");
		return FAILURE;
	}
	skip_whitespace(&str_ptr);
	if(!(*str_ptr)) {
		*number = 0;
		fprintf(stderr,"amount field empty\n");
		gtk_text_buffer_set_text(error_buffer,"Please enter an amount",-1);
		return FAILURE;
	}
	/* validate integer digits */
	retval = validate_integer_digits(&str_ptr,number);
	if(retval == FAILURE) {
		*number = 0;
		return FAILURE;
	} else if (retval == FINISHED) {
		return SUCCESS;
	}
	/* validate fractional digits */
	retval = validate_fractional_digits(str_ptr,&fractional_digits);
	if(retval == FAILURE) {
		fprintf(stderr,"invalid characters in amount field, please enter a valid amount\n");
		gtk_text_buffer_set_text(error_buffer,"Invalid characters in amount field, please enter a valid amount\n",-1);
		*number = 0;
		return FAILURE;
	} else if (retval == FINISHED) {
		*number += fractional_digits;
		return SUCCESS;
	}
	str_ptr += 2;
	skip_zeros(&str_ptr);
	if(*str_ptr > '0' && *str_ptr <= '9') {
		fprintf(stderr,"too many digits to right of decimal, please enter a valid amount\n");
		gtk_text_buffer_set_text(error_buffer,"Invalid characters in amount field, please enter a valid amount\n",-1);
		*number = 0;
		return FAILURE;
	}
	skip_whitespace(&str_ptr);
	if(*str_ptr) {
		fprintf(stderr,"invalid characters in amount field, please enter a valid amount\n");
		gtk_text_buffer_set_text(error_buffer,"Invalid characters in amount field, please enter a valid amount\n",-1);
		*number = 0;
		return FAILURE;
	}
	*number += fractional_digits;
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


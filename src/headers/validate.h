#ifndef VALIDATE_H
#define VALIDATE_H

unsigned char validate_day(unsigned long *number) ;
unsigned char validate_month(unsigned long *number);
unsigned char validate_year(unsigned long *number);
unsigned char validate_amount(char *str_ptr, unsigned long long *number);
unsigned char validate_person(char *text);
unsigned char validate_method(char *text);
unsigned char validate_date (char *text, unsigned int *year_s, unsigned char *month_s, unsigned char *day_s);
unsigned char cents_to_string(unsigned long long number, char *str);

#endif

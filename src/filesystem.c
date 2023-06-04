#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <gtk/gtk.h>
#include "headers/config.h"
#include "headers/filesystem.h"
#include "headers/list.h"
#include "headers/validate.h"
#include "headers/utils.h"

unsigned char load_items(GtkListStore *model)
{

	unsigned long long current_line = 0;
	FILE *the_file;
	char * retval;
	char line[MAX_DAY_CHARS + MAX_MONTH_CHARS + MAX_YEAR_CHARS +
		 MAX_METHOD_CHARS + MAX_PERSON_CHARS + MAX_AMOUNT_CHARS + 20];
	char finished_message[FILENAME_SIZE + 35] = {0};
	char error_message[FILENAME_SIZE + 35] = {0};
	char *newline_location;
	char *endptr;
	char *token;
	char *end;
	unsigned long number;
	unsigned long long amount_s;
	char date_s[MAX_DATE_CHARS] = {0};
	unsigned char day;
	unsigned char month;
	unsigned int year;
	char *person_s;
	char *method_s;

	snprintf(error_message,sizeof(error_message),"Cannot open file at: %s",filename);

	errno = 0;
	the_file = fopen(filename,"r");
	if(!the_file) {
		perror("");
		fprintf(stderr,"%s\n",error_message);
		gtk_text_buffer_set_text(error_buffer,error_message,-1);
	  	return FAILURE;
	}

	snprintf(finished_message,sizeof(finished_message),"Finished loading file at: %s",filename);
	printf("Loading file from disk\n");

	for(;current_line < ULONG_MAX;current_line++) {

		memset(line,0,sizeof(line));
		errno = 0;
		retval = fgets(line,sizeof(line),the_file);
		if(!retval) {
			if(ferror(the_file)) {
				perror("Couldn't fully load file: ");
				gtk_text_buffer_set_text(error_buffer,"Couldn't fully load file, "
						"fatal error, should exit program",-1);
				fclose(the_file);
				return UNFINISHED;
			} else if(feof(the_file)) {
				if(current_line == 0) {
					fprintf(stderr,"Empty file, missing csv header, corrupt file %llu\n",current_line);
					gtk_text_buffer_set_text(error_buffer,"Empty file, missing csv header, corrupt file",-1);
					fclose(the_file);
					return FAILURE;
				} else if (current_line == 1) {
					fprintf(stderr,"File is empty, nothing to do\n");
					gtk_text_buffer_set_text(error_buffer,"Couldn't fully load file, "
							"unknown error, should exit program",-1);
					fclose(the_file);
					return SUCCESS;
				} else {
					//SUCESSS !
					gtk_text_buffer_set_text(error_buffer,finished_message,-1);
					break;
				}
			} else {
				fprintf(stderr,"Couldn't fully load file, unknown error, should exit program\n");
				gtk_text_buffer_set_text(error_buffer,"Couldn't fully load file, "
						"unknown error, should exit program",-1);
				fclose(the_file);
				return UNFINISHED;
			}
		}
		newline_location = strchr(line,'\n');
		if(!newline_location) {
			fprintf(stderr,"No newline at end of line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,"No newline at end of .csv line."
					"  File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		if(current_line == 0) {
			if(strcmp(line,"Day,Month,Year,Customer,Method,Amount\n")) {
				fprintf(stderr,"Invalid csv header, corrupt file %llu\n",current_line);
				gtk_text_buffer_set_text(error_buffer,"Invalid csv header, corrupt file",-1);
				fclose(the_file);
				return FAILURE;
			}
			continue;
		}
		/* init strsep variable to beginning of scanned line from file */
		end = line;
		/* get day from file ----------------------------------------------------------------- */
		/* run strsetp */
		token = strsep_custom(&end,',');
		/* if no ending comma for day field */
		if(!end) {
			fprintf(stderr,"Missing comma on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if day csv field is empty, return */
		if(!(*token)) {
			fprintf(stderr,"Missing day on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing day."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		number = strtoul(token,&endptr,10);
		if(endptr == token || (validate_day(&number) == FAILURE) || *endptr != '\0') {
			fprintf(stderr,"Missing day on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing day."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		day = (unsigned char) number;
		//printf("day: %u\n",day);
		/* get month from file -------------------------------------------------------------------- */
		token = strsep_custom(&end,',');
		/* if null character after last valid csv field, return */
		if(!token) {
			fprintf(stderr,".csv ended prematurely on line %llu, file is corrupted\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please exit program and check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if no ending comma for month field */
		if(!end) {
			fprintf(stderr,"Missing comma on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if month csv field is empty, return */
		if(!(*token)) {
			fprintf(stderr,"Missing month on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing month."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		number = strtoul(token,&endptr,10);
		if(endptr == token || (validate_month(&number) == FAILURE) || *endptr != '\0') {
			fprintf(stderr,"Missing month on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing month."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		month = (unsigned char) number;
		//printf("month: %u\n",month);
		/* get year from file --------------------------------------------------------------------------- */
		token = strsep_custom(&end,',');
		/* if null character after last valid csv field, return */
		if(!token) {
			fprintf(stderr,".csv ended prematurely on line %llu, file is corrupted\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please exit program and check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if no ending comma for year field */
		if(!end) {
			fprintf(stderr,"Missing comma on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if year csv field is empty, return */
		if(!(*token)) {
			fprintf(stderr,"Missing year on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing year."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		number = strtoul(token,&endptr,10);
		if(endptr == token || (validate_year(&number) == FAILURE) || *endptr != '\0') {
			fprintf(stderr,"Missing year on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing year."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		year = (unsigned int) number;
		//printf("year: %u\n",year);
		/* get person from file --------------------------------------------------------------- */
		token = strsep_custom(&end,',');
		/* if null character after last valid csv field, return */
		if(!token) {
			fprintf(stderr,".csv ended prematurely on line %llu, file is corrupted\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please exit program and check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if no ending comma for person field */
		if(!end) {
			fprintf(stderr,"Missing comma on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if empty person csv field, or person string doesn't validate, return */
		if(validate_person(token) == FAILURE) {
			fprintf(stderr,"Cannot parse person on %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing properly formatted person."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		person_s = token;
		//printf("person: %s\n",person_s);
		/* get method from file ------------------------------------------ */
		token = strsep_custom(&end,',');
		/* if null character after last valid csv field, return */
		if(!token) {
			fprintf(stderr,".csv ended prematurely on line %llu, file is corrupted\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please exit program and check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if no ending comma for method field */
		if(!end) {
			fprintf(stderr,"Missing comma on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* method field is optional */
		/* if method is non empty, ie there isn't ,, in csv, then validate it for sanity */
		if(*token) {
			if(validate_method(token) == FAILURE) {
				fprintf(stderr,"Cannot parse person on %llu\n",current_line);
				gtk_text_buffer_set_text(error_buffer,".csv file is missing properly formatted person."
						"File is corrupted, please check file",-1);
				fclose(the_file);
				return UNFINISHED;
			}
			method_s = token;
		} else {/* else just keep method an empty string and put an empty string in liststore */
			method_s = "";
		}
		//printf("method: %s\n",method_s);
		/* get amount from file -------------------------------------------------------------------- */
		token = strsep_custom(&end,'\n');
		/* if null character after last valid csv field, return */
		if(!token) {
			fprintf(stderr,".csv ended prematurely on line %llu, file is corrupted\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please exit program and check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if no ending newline for amount field */
		if(!end) {
			fprintf(stderr,"Missing comma on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing comma."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if amount csv field is empty, return */
		if(!(*token)) {
			fprintf(stderr,"Missing month on line %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing month."
				      	" File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		/* if empty person csv field, or amount string doesn't validate, return */
		if(validate_amount(token,&amount_s) == FAILURE) {
			fprintf(stderr,"Cannot parse person on %llu\n",current_line);
			gtk_text_buffer_set_text(error_buffer,".csv file is missing properly formatted person."
				      	"File is corrupted, please check file",-1);
			fclose(the_file);
			return UNFINISHED;
		}
		//printf("amount: %llu\n",amount_s);

		snprintf(date_s,sizeof(date_s),"%u/%u/%u",day,month,year);

		//gtk_widget_freeze_child_notify(GTK_WIDGET(tree_view));
		gtk_list_store_insert_with_values(model, NULL, -1,
						DATE_C, date_s,
						PERSON_C, person_s,
						PAYMENT_METHOD_C, method_s,
						AMOUNT_C, amount_s,
						YEAR_C, year,
						MONTH_C, month,
						DAY_C, day,
						SHOW_C, 1,
						-1);
		//gtk_widget_thaw_child_notify(GTK_WIDGET(tree_view));

		if(current_line % 1000 == 0) {
			printf("Loaded %llu items from disk\n",current_line);
		}

	} /* end of for loop reading file */

	fclose(the_file);
	printf("success in parsing entire file! :)\n");
	add_all_rows(GTK_TREE_MODEL(model));
	return SUCCESS;

}


void save_items(GtkWidget *widget, gpointer model_void) {
	GtkTreeModel *model = (GtkTreeModel *)model_void;
	GtkTreeIter iter;
	char save_message[FILENAME_SIZE + 20] = {0};
	char error_message[FILENAME_SIZE + 40] = {0};
	/*year*/
	guint gyear_s;
	unsigned int year_s;
	/*month*/
	guchar gmonth_s;
	unsigned char month_s;
	/*day*/
	guchar gday_s;
	unsigned char day_s;
	/*name*/
	gchar *gname_s;
	char name_s[MAX_PERSON_CHARS] = {0};
	/*method*/
	gchar *gmethod_s;
	char method_s[MAX_METHOD_CHARS] = {0};
	/*amount*/
	guint64 gitem_amount;
	unsigned long long item_amount;
	char amount_str[MAX_AMOUNT_CHARS] = {0};
	FILE *the_file;

	snprintf(error_message,sizeof(error_message),"Cannot create or open file at %s",filename);

	errno = 0;
	the_file = fopen(filename,"w+");
	if(!the_file) {
		perror("");
		fprintf(stderr,"%s\n",error_message);
		gtk_text_buffer_set_text(error_buffer,error_message,-1);
	  	return;
	}
	snprintf(save_message,sizeof(save_message),"File saved at %s",filename);
	fprintf(the_file,"Day,Month,Year,Customer,Method,Amount\n");

	if(gtk_tree_model_get_iter_first(model,&iter) == FALSE) {
		gtk_text_buffer_set_text(error_buffer,save_message,-1);
		fclose(the_file);
		return;
	}

	do {
		/*year*/
		gtk_tree_model_get(model, &iter, YEAR_C, &gyear_s, -1);
		year_s = (unsigned int) gyear_s;
		/*month*/
		gtk_tree_model_get(model, &iter, MONTH_C, &gmonth_s, -1);
		month_s = (unsigned char) gmonth_s;
		/*day*/
		gtk_tree_model_get(model, &iter, DAY_C, &gday_s, -1);
		day_s = (unsigned char) gday_s;
		/*name*/
		gtk_tree_model_get(model, &iter, PERSON_C, &gname_s, -1);
		snprintf(name_s,sizeof(name_s),"%s",(char *)gname_s);
		g_free(gname_s);
		/*method*/
		gtk_tree_model_get(model, &iter, PAYMENT_METHOD_C, &gmethod_s, -1);
		snprintf(method_s,sizeof(method_s),"%s",(char *)gmethod_s);
		g_free(gmethod_s);
		/*amount*/
		gtk_tree_model_get(model, &iter, AMOUNT_C, &gitem_amount, -1);
		item_amount = (unsigned long long) gitem_amount;
		if(cents_to_string(item_amount,amount_str) == FAILURE) {
			strcpy(amount_str,"0");
		}

		fprintf(the_file,"%u,%u,%u,%s,%s,%s\n",day_s,month_s,year_s,name_s,method_s,amount_str);
	} while(gtk_tree_model_iter_next(model,&iter));


	fclose(the_file);
	gtk_text_buffer_set_text(error_buffer,save_message,-1);

}


#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/config.h"
#include "headers/list.h"
#include "headers/validate.h"
#include "headers/utils.h"

unsigned char add_all_rows(GtkTreeModel *model)
{
	/* first parameter of this function is accepting our base GtkListStore cast to a GtkTreeModel */
	GtkTreeIter iter;
	gboolean is_visible;
	gdouble gitem_amount;
	char string_from_double[50] = {0};
	double item_amount = 0;
	/* set globally defined totals to 0, because we are re-counting everything in the liststore */
	amount_total = 0;
	filtered_amount_total = 0;

	if(gtk_tree_model_get_iter_first(model,&iter) == FALSE) {
		gtk_label_set_text( GTK_LABEL(total_results_label), "0");
		gtk_label_set_text( GTK_LABEL(total_filtered_results_label), "0");
		return SUCCESS;
	}

	do {
		gtk_tree_model_get(model, &iter, SHOW_C,&is_visible,-1);
		gtk_tree_model_get(model, &iter, AMOUNT_C, &gitem_amount, -1);
		item_amount = (double) gitem_amount;
		printf("item amount: %lf\n",item_amount);
		/* truncate double the same way we do the rest of the program, so totals make sense */
		/* ie with the %.2lf snprintf/sprintf format specifier */
		if(truncate_double(&item_amount) == FAILURE)
			return FAILURE;
		if(is_visible)
			filtered_amount_total += item_amount;
		amount_total += item_amount;

	} while(gtk_tree_model_iter_next(model,&iter));

	printf("filtered total paid: %lf\n",filtered_amount_total);
	printf("total paid: %lf\n",amount_total);
	snprintf(string_from_double,50,"%.2lf",amount_total);
	gtk_label_set_text(GTK_LABEL(total_results_label), string_from_double);
	snprintf(string_from_double,50,"%.2lf",filtered_amount_total);
	gtk_label_set_text(GTK_LABEL(total_filtered_results_label), string_from_double);

	return SUCCESS;

}

void do_add(GtkWidget *widget, gpointer model)
{

	double number;
	/* The data at these pointers can't be modified */
	/* ideally we should copy this into a local array
	 * Check to see if it is not undefined behaviour to
	 * pass a local char array to gtk_list_stre_insert_with_value
	 * and if not, then do that, because it's safer and can cut off
	 * trailing whitespace etc */
	char *date_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(date_entry));
	char *person_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(person_entry));
	char *amount_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(amount_entry));
	char *method_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(method_entry));
	unsigned int year_s;
	unsigned char month_s;
	unsigned char day_s;

	if(validate_date(date_ptr,&year_s,&month_s,&day_s) == FAILURE) {
		return;
	}
	if(validate_person(person_ptr) == FAILURE) {
		return;
	}
	if(validate_method(method_ptr) == FAILURE) {
		return;
	}
	if(validate_amount(amount_ptr,&number) == FAILURE)
		return;
	printf("added number: %lf\n",amount_total+number);
	if(amount_total + number >= 7777777777.55 || amount_total + number <= -7777777777.55) {
		gtk_text_buffer_set_text(error_buffer,"Cannot add amount. Amount total out of range, should be between"
				" -7777777777.55 and 7777777777.55",-1);
		return;
	}

	gtk_list_store_insert_with_values(model, NULL, -1,
					DATE_C, date_ptr,
					PERSON_C, person_ptr,
					PAYMENT_METHOD_C, method_ptr,
					AMOUNT_C, number,
					YEAR_C, year_s,
					MONTH_C, month_s,
					DAY_C, day_s,
					SHOW_C, 1, /* 1 for, yes show in tree */
					-1);

	gtk_text_buffer_set_text(error_buffer," ",-1);
	gtk_entry_set_text(GTK_ENTRY(date_entry), "");
	gtk_entry_set_text(GTK_ENTRY(person_entry), "");
	gtk_entry_set_text(GTK_ENTRY(amount_entry), "");
	gtk_entry_set_text(GTK_ENTRY(method_entry), "");

	add_all_rows(GTK_TREE_MODEL(model));

}

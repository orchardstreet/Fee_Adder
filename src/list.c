#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/config.h"
#include "headers/list.h"
#include "headers/validate.h"
#include "headers/utils.h"
#include "headers/config.h"

void toggle_paid(GtkCellRendererToggle *cell_render, gchar *path_c, gpointer args_struct)
{
	/* init variables */
	struct toggle_paid_args *args_struct_ptr = (struct toggle_paid_args *) args_struct;
	GtkTreeModel *sorted_model = args_struct_ptr->sorted_model_arg;
	GtkTreeModel *filter = args_struct_ptr->filter_arg;
	GtkListStore *model = args_struct_ptr->model_arg;
	gboolean is_active;
    GtkTreeIter sorted_iter;
    GtkTreeIter filter_iter;
    GtkTreeIter liststore_iter;
	(void)cell_render;

	if(!gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(sorted_model), &sorted_iter, (gchar *)path_c)) {
		fprintf(stderr,"cannot get iter from treeview after toggle\n");
		return;
	}
	gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(sorted_model),&filter_iter, &sorted_iter );
	gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER(filter), &liststore_iter, &filter_iter);

	gtk_tree_model_get(GTK_TREE_MODEL(model),&liststore_iter,PAID_C,&is_active,-1);

	if(is_active) {
		gtk_list_store_set(model,&liststore_iter,PAID_C,0,-1);
	} else {
		gtk_list_store_set(model,&liststore_iter,PAID_C,1,-1);
	}
}

/* Render each row in amount column as full decimal rather than cents */
void amount_cell_data_func(GtkTreeViewColumn *col,
						   GtkCellRenderer *renderer,
						   GtkTreeModel *model,
						   GtkTreeIter *iter,
						   gpointer user_data) {
	char amount_string[MAX_AMOUNT_CHARS] = {0};
	unsigned long long amount;
	gtk_tree_model_get(model,iter,AMOUNT_C,&amount,-1);
	if(cents_to_string(amount,amount_string) == FAILURE) {
		g_object_set(renderer,"text","error",NULL);
		return;
	}
	g_object_set(renderer,"text",amount_string,NULL);
}

unsigned char add_all_rows(GtkTreeModel *model)
{
	/* first parameter of this function is accepting our base GtkListStore cast to a GtkTreeModel */
	GtkTreeIter iter;
	gboolean is_visible;
	char string_from_llu[MAX_AMOUNT_CHARS] = {0};
	unsigned long long item_amount = 0;
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
		gtk_tree_model_get(model, &iter, AMOUNT_C, &item_amount, -1);
		//printf("item amount: %llu\n",item_amount);
		if(amount_total + item_amount >= UINT64_MAX || amount_total + item_amount < amount_total) {
			gtk_text_buffer_set_text(error_buffer,"Cannot add amount. Amount total out of range, should be between"
					" 0 and UINT64_MAX",-1);
			fprintf(stderr,"Cannot add amount. Amount total out of range, should be between 0 and UINT64_MAX\n");
			fprintf(stderr,"amount total: %llu, item_amount: %llu\n",amount_total,item_amount);
			return FAILURE;
		}
		if(is_visible)
			filtered_amount_total += item_amount;
		amount_total += item_amount;

	} while(gtk_tree_model_iter_next(model,&iter));

	//printf("filtered total paid: %llu\n",filtered_amount_total);
	//printf("total paid: %llu\n",amount_total);
	if(cents_to_string(amount_total,string_from_llu) == FAILURE)
		return FAILURE;
	gtk_label_set_text(GTK_LABEL(total_results_label), string_from_llu);
	if(cents_to_string(filtered_amount_total,string_from_llu) == FAILURE)
		return FAILURE;
	gtk_label_set_text(GTK_LABEL(total_filtered_results_label), string_from_llu);

	return SUCCESS;

}

void add_single_transaction_from_entryboxes(GtkWidget *widget, gpointer model)
{

	unsigned long long number;
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
	unsigned int sortable_date = 0;
	gboolean is_active = 0;
	char date_s[MAX_DATE_CHARS] = {0};

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

	printf("added number: %llu\n",amount_total+number);

	/* try to detect if adding a number will overflow UINT64_MAX */
	if(amount_total + number >= UINT64_MAX || amount_total + number < amount_total) {
		gtk_text_buffer_set_text(error_buffer,"Cannot add amount. Amount total out of range, should be between"
				" 0 and UINT64_MAX",-1);
		fprintf(stderr,"Cannot add amount. Amount total out of range, should be between 0 and UINT64_MAX\n");
		fprintf(stderr,"amount total: %llu, item_amount: %llu\n",amount_total,number);
		return;
	}

	/* get paid status */
	is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(paid_check_button));

	/* convert validated date numbers to readable text,
	 * necessary to display years as full years, also removes whitespace etc */
	snprintf(date_s,sizeof(date_s),"%02u/%02u/%u",day_s,month_s,year_s);
	year_month_day_to_sortable_date(year_s, month_s,day_s,&sortable_date);

	gtk_list_store_insert_with_values(model, NULL, -1,
					DATE_C, date_s,
					PERSON_C, person_ptr,
					PAYMENT_METHOD_C, method_ptr,
					AMOUNT_C, number,
					YEAR_C, year_s,
					MONTH_C, month_s,
					DAY_C, day_s,
					PAID_C,is_active,
					DATE_SORT_C,sortable_date,
					SHOW_C, 1, /* 1 for, yes show in tree */
					-1);

	is_scrolling = 1;
	scroll_to_end();
	is_scrolling = 0;

	gtk_text_buffer_set_text(error_buffer," ",-1);
	gtk_entry_set_text(GTK_ENTRY(date_entry), "");
	gtk_entry_set_text(GTK_ENTRY(person_entry), "");
	gtk_entry_set_text(GTK_ENTRY(amount_entry), "");
	gtk_entry_set_text(GTK_ENTRY(method_entry), "");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(paid_check_button),FALSE);

	add_all_rows(GTK_TREE_MODEL(model));
}

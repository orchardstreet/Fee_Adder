/* TODO add paid this month column */
/* TODO implement llu_to_str in save and possiby elsewhere */
/* TODO change %ld to %llu everywhere!!! */
/* TODO launch external window after filters button */
/* TODO put border around entire value entry section, to remove awkward look */
/* TODO filter out special characters from input */
/* TODO add switch below 'set filters' */
/* TODO see where two numbers added messes up total at bottom, and adjust the range
 * of acceptable values in do_add accordingly */
/* TODO make totals BOLD and BIG */
/* TODO less 0s https://docs.gtk.org/gtk3/treeview-tutorial.html#cell-data-functions */
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#define _ISOC99_SOURCE 
#include <math.h>
#include "headers/validate.h"
#include "headers/config.h"
#include "headers/utils.h"
#include "headers/filesystem.h"
#include "headers/list.h"

/* Init global variables */
GtkWidget *amount_entry, *date_entry, *person_entry, *method_entry;
GtkTextBuffer *error_buffer;
GtkWidget *error_widget;
GtkWidget *scrolled_window;
GtkWidget *total_filtered_results_label;
GtkWidget *total_results_label;
GtkWidget *window;
unsigned long long filtered_amount_total;
unsigned long long amount_total;
char filename[FILENAME_SIZE] = "purchase_log.csv";

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



/* Main */
int main(int argc, char **argv)
{

	/* Init local variables */
	GtkWidget *grid, *grid2, *grid3, *box, *box2, *box3, *date_label, *person_label,
		*amount_label, *add_label, *tree_view, *filter_button, *search_button, *edit_button, *add_button,
		*delete_button, *save_button, *before_totals_seperator, *method_label,
		*total_filtered_label, *total_label;
	GtkTreeViewColumn *column, *column1, *column2, *column3;
	GtkListStore *model;
	GtkTreeModel *filter;

	printf("size of double: %zu\nsize of gdouble %zu\nsize of int: %zu\n"
			"size of unsigned int: %zu\nsize of long: %zu\n"
			"size of unsigned long %zu\n"
			,sizeof(double),sizeof(gdouble),sizeof(int),sizeof(unsigned int)
			,sizeof(long),sizeof(unsigned long));

	/* Init GTK */
	gtk_init(&argc,&argv);
	
	/* Temp debug info */
	printf("Double has a precision of %d digits\n",LDBL_DIG);
	printf("Columns: %d\n",TOTAL_COLUMNS);

	/* Create vertically oriented box to pack program widgets into */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);

	/* Create window, set title, border width, and size */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window) , "Fee Adder" );
	gtk_container_set_border_width (GTK_CONTAINER(window),10); 
	gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
	gtk_window_set_default_size ( GTK_WINDOW(window), 800, 580);

	/* Destroy window on close */
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	/* Create grid for value entry */
	grid = gtk_grid_new();
	gtk_grid_set_row_spacing (GTK_GRID(grid),2);
	gtk_grid_set_column_spacing (GTK_GRID(grid),10);
	/* Add grid to vertical box */
	gtk_box_pack_start (GTK_BOX (box), grid, FALSE, TRUE, 0);
	/* Add margin to value entry grid */
	gtk_widget_set_margin_top(grid,15);

	/* Add date entry to value entry grid */
	date_label = gtk_label_new("Date (dd/mm/yy)");
        gtk_widget_set_halign (date_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(date_label,3);
	gtk_grid_attach(GTK_GRID(grid),date_label,0,0,1,1);
	date_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),date_entry,0,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(date_entry), 8);

	/* Add person entry to value entry grid */
	person_label = gtk_label_new("Customerrr");
        gtk_widget_set_halign (person_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(person_label,3);
	gtk_grid_attach(GTK_GRID(grid),person_label,1,0,1,1);
	person_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),person_entry,1,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(person_entry), 20);

	/* Add payment method to value entry grid */
	method_label = gtk_label_new("Method");
        gtk_widget_set_halign (method_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(method_label,3);
	gtk_grid_attach(GTK_GRID(grid),method_label,2,0,1,1);
	method_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),method_entry,2,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(method_entry), 10);

	/* Add amount entry to value entry grid */
	amount_label = gtk_label_new("Shekels");
        gtk_widget_set_halign (amount_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(amount_label,3);
	gtk_grid_attach(GTK_GRID(grid),amount_label,3,0,1,1);
	amount_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),amount_entry,3,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(amount_entry), 10);
	/* Align value entry grid */
        gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_end(grid,70);

	/* Add 'add' button to value entry grid */
	add_label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid),add_label,4,0,1,1);
	add_button = gtk_button_new_with_label("Add");
	gtk_grid_attach(GTK_GRID(grid), add_button, 4, 1, 1, 1);

	/* Create horizontally oriented box to horizontally pack progam widgets into */
	box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,20);
	/* Pack horizontal box into vertical box */
	gtk_box_pack_start (GTK_BOX (box), box2, FALSE, TRUE, 0);
	/* Align horizontal box */
        gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);

	/* Create another vertically oriented box to pack into last hbox
	 * this will contain the scrolled window and totals */
	box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);
	gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, TRUE, 0);

	/* Create a scrollable window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	/* Set preferred size of scrollable window */
 	gtk_widget_set_size_request (scrolled_window, 540, 300);
	/* Always include a vertical scroll cursor on scroll window */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	/* Add scrollable window to horizontal box */
	gtk_box_pack_start (GTK_BOX (box3), scrolled_window, TRUE, TRUE, 0);
	/* Align scrollable window */
        gtk_widget_set_halign (scrolled_window, GTK_ALIGN_START);
	g_object_set (scrolled_window, "vexpand", TRUE, NULL);
	g_object_set (scrolled_window, "hexpand", FALSE, NULL);
        //gtk_widget_set_valign (scrolled_window, GTK_ALIGN_START);
        //g_object_set (scrolled_window, "margin", 10, NULL);
    	gtk_widget_show (scrolled_window);

	/* Create liststore for table */
	model = gtk_list_store_new(
		TOTAL_COLUMNS,  /* required parameter, total columns */
		G_TYPE_STRING,  /* 0th column, date, DATE_C */
		G_TYPE_STRING,  /* first column, person, PERSON_C */
		G_TYPE_STRING,  /* second column, payment method, PAYMENT_METHOD_C */
		G_TYPE_UINT64,  /* third column, amount, AMOUNT_C */
		G_TYPE_UINT,   /* fourth column, year, YEAR_C */
		G_TYPE_UCHAR,   /* fifth column, month, MONTH_C */
		G_TYPE_UCHAR,   /* sixth column, day, DAY_C */
		G_TYPE_BOOLEAN  /* 1 for show, 0 for hide */
		);

	/* Example rows, if any */
	/*
	gtk_list_store_insert_with_values(model, NULL, -1,
		DATE_C, "03/22/23",
		PERSON_C, "Natalie Smith",
		AMOUNT_C, 100.20,
		-1);
	*/

	/* create treestore as filter of liststore */
	filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(model),NULL);
	gtk_tree_model_filter_set_visible_column(GTK_TREE_MODEL_FILTER(filter),TOTAL_COLUMNS - 1);

	/* Create treeview from treestore */
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(filter));

	/* Scroll the scrollable window to bottom when the size of treeview changes
	 * Default behaviour is to hide new rows at bottom for some reason */
	g_signal_connect (tree_view, "size-allocate", G_CALLBACK (scroll_to_end), NULL);

	/* Unref treestore */
	g_object_unref(filter);

	/* Add columns to treeview */
	/* Treeview column 0 */
	column = gtk_tree_view_column_new_with_attributes("Date paid",
		gtk_cell_renderer_spin_new(),
		"text", DATE_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	gtk_tree_view_column_set_expand ( column, TRUE);
	gtk_tree_view_column_set_fixed_width (column,125);

	/* Treeview column 1 */
	column1 = gtk_tree_view_column_new_with_attributes("Customer",
		gtk_cell_renderer_text_new(),
		"text", PERSON_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column1);
	gtk_tree_view_column_set_expand ( column1, TRUE);
	gtk_tree_view_column_set_fixed_width (column1,190);

	/* Treeview column 2 */
	column2 = gtk_tree_view_column_new_with_attributes("Method",
		gtk_cell_renderer_text_new(),
		"text", PAYMENT_METHOD_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column2);
	gtk_tree_view_column_set_expand ( column2, TRUE);
	gtk_tree_view_column_set_fixed_width (column2,105);


	/* Treeview column 3 */
	column3 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column3,"Shekels");
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),column3);
	GtkCellRenderer *renderer3;
	renderer3 = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column3,renderer3,TRUE);
	gtk_tree_view_column_add_attribute(column3,renderer3,"text",AMOUNT_C);
	g_object_set(renderer3,
				 "weight",PANGO_WEIGHT_BOLD,
				 "weight-set", TRUE,
				 NULL);
	gtk_tree_view_column_set_cell_data_func(column3,renderer3,amount_cell_data_func,NULL, NULL);
	gtk_tree_view_column_set_expand ( column3, TRUE);
	gtk_tree_view_column_set_fixed_width (column2,100);
	g_object_set(renderer3, "editable", TRUE, NULL);


	/*
	column3 = gtk_tree_view_column_new_with_attributes("Shekels",
		gtk_cell_renderer_text_new(),
		"text", AMOUNT_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column3);
	gtk_tree_view_column_set_expand ( column3, TRUE);
	gtk_tree_view_column_set_fixed_width (column3,100);
	*/



	/* Create grid for right-side buttons */
	grid2 = gtk_grid_new();
	gtk_grid_set_row_spacing (GTK_GRID(grid2),20);
	gtk_widget_set_margin_top(grid2,55);

	/* Create filter button */
	filter_button = gtk_button_new_with_label("Set filters");
	gtk_grid_attach(GTK_GRID(grid2), filter_button, 0, 0, 1, 1);

	/* Create search button */
	search_button = gtk_button_new_with_label("Show all");
	gtk_grid_attach(GTK_GRID(grid2), search_button, 0, 1, 1, 1);

	/* Create edit button */
	edit_button = gtk_button_new_with_label("Edit row");
	gtk_grid_attach(GTK_GRID(grid2), edit_button, 0, 2, 1, 1);

	/* Create delete button */
	delete_button = gtk_button_new_with_label("Delete row");
	gtk_grid_attach(GTK_GRID(grid2), delete_button, 0, 3, 1, 1);

	/* Create Save button */
	save_button = gtk_button_new_with_label("Save");
	gtk_grid_attach(GTK_GRID(grid2), save_button, 0, 4, 1, 1);

	/* Pack grid2 to box2 */
	gtk_box_pack_start (GTK_BOX (box2), grid2, TRUE, TRUE, 0);

	/* Align grid2 */
        gtk_widget_set_halign (grid2, GTK_ALIGN_START);
        gtk_widget_set_valign (grid2, GTK_ALIGN_START);

	/* create seperator for before totals */
	before_totals_seperator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
 	gtk_widget_set_size_request (before_totals_seperator, 40, 2);

	/* Create grid for totals */
	grid3 = gtk_grid_new();
	/* Set grid spacing */
	gtk_grid_set_row_spacing (GTK_GRID(grid3),9);
	gtk_grid_set_column_spacing (GTK_GRID(grid3),0);
	/* Set grid alignment */
        gtk_widget_set_halign (grid3, GTK_ALIGN_END);
	gtk_widget_set_margin_end(grid3,30);
	/* Create grid members */
	//filters_applied_label = gtk_label_new("Filters applied: ");
        //gtk_widget_set_halign (filters_applied_label, GTK_ALIGN_START);
	//filters_applied_results_label = gtk_label_new("date, payment method");
        //gtk_widget_set_halign (filters_applied_results_label, GTK_ALIGN_START);
	total_filtered_label = gtk_label_new("Total paid including filters:  ₪");
        gtk_widget_set_halign (total_filtered_label, GTK_ALIGN_END);
	total_filtered_results_label = gtk_label_new("0.00");
        gtk_widget_set_halign (total_filtered_results_label, GTK_ALIGN_END);
	total_label = gtk_label_new("Total paid:  ₪");
        gtk_widget_set_halign (total_label, GTK_ALIGN_END);
	total_results_label = gtk_label_new("0.00");
        gtk_widget_set_halign (total_results_label, GTK_ALIGN_END);
	/* Attach grid members */
	//gtk_grid_attach(GTK_GRID(grid3),filters_applied_label,0,0,1,1);
	//gtk_grid_attach(GTK_GRID(grid3),filters_applied_results_label,1,0,3,1);
	gtk_grid_attach(GTK_GRID(grid3),before_totals_seperator,0,0,2,1);
	gtk_grid_attach(GTK_GRID(grid3),total_filtered_label,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid3),total_filtered_results_label,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid3),total_label,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid3),total_results_label,1,2,1,1);
	gtk_box_pack_start (GTK_BOX (box3), grid3, TRUE, TRUE, 0);

	/* error widget */
	error_buffer = gtk_text_buffer_new(NULL);
	error_widget = gtk_text_view_new_with_buffer(error_buffer);
	gtk_text_buffer_set_text(error_buffer," ",-1);
	gtk_box_pack_start (GTK_BOX (box), error_widget, 0, 0, 0);

	printf("Fee adder 0.1\n");
	
	if(check_endianness() == IS_BIG_ENDIAN) {
		gtk_text_buffer_set_text(error_buffer,"Computer should be litte endian, please exit to avoid data issues\n",-1);	
		fprintf(stderr,"Computer should be little endian, please exit to avoid data issues\n");	

	}
	if (sizeof(void *) != 8) {
		gtk_text_buffer_set_text(error_buffer,"Error: please use a 64-bit computer, please exit to avoid data issues\n",-1);
		fprintf(stderr,"Error: please use a 64-bit computer, please exit to avoid data issues\n");
        }
	/* warn if compiler didn't create correct length for types */
	if(sizeof(double) != 8) {
		gtk_text_buffer_set_text(error_buffer,"size of double must equal 8, please exit to avoid data issues\n",-1);	
		fprintf(stderr,"size of double must equal 8, please exit to avoid data issues\n");	
	}
	if(sizeof(gdouble) != 8) {
		gtk_text_buffer_set_text(error_buffer,"size of gdouble must equal 8, please exit to avoid data issues\n",-1);	
		fprintf(stderr,"size of gdouble must equal 8, please exit to avoid data issues\n");	
	}
	if(sizeof(unsigned int) != 4) {
		gtk_text_buffer_set_text(error_buffer,"size of unsigned int must equal 4, please exit to avoid data issues\n",-1);	
		fprintf(stderr,"size of unsigned int must equal 4, please exit to avoid data issues\n");	
	}
	if(sizeof(int) != 4) {
		gtk_text_buffer_set_text(error_buffer,"size of int must equal 4, please exit to avoid data issues\n",-1);	
		fprintf(stderr,"size of int must equal 4, please exit to avoid data issues\n");	
	}
	if(sizeof(long) != 8) {
		gtk_text_buffer_set_text(error_buffer,"size of long must equal 8, please exit to avoid data issues\n",-1);
		fprintf(stderr,"size of long must equal 8, please exit to avoid data issues\n");
	}
	if(sizeof(unsigned long) != 8) {
		gtk_text_buffer_set_text(error_buffer,"size of unsigned long must equal 8, please exit to avoid data issues\n",-1);	
		fprintf(stderr,"size of unsigned long must equal 8, please exit to avoid data issues\n");	
	}
	/* load items from csv file into liststore, treestore, and treeview */
	load_items(model);

	/* After clicking add, call 'do_add' function */
	g_signal_connect(add_button,"clicked",G_CALLBACK(do_add),model);
	/* After clicking save, call 'save_items */
	g_signal_connect(save_button,"clicked",G_CALLBACK(save_items),GTK_TREE_MODEL(model));
	/* After pressing a keyboard button, called 'key_press_event */
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (keypress_function), NULL);

	/* Add tree view to scrolled window */
	gtk_container_add(GTK_CONTAINER(scrolled_window),tree_view);	
	
	/* Add box to window */
	gtk_container_add(GTK_CONTAINER(window),box);

	/* Show everything */
	gtk_widget_show_all(box);
	gtk_widget_show_all(scrolled_window);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}

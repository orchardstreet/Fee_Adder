#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <string.h>
#define _ISOC99_SOURCE 
#include <math.h>
#define MAX_ENTRIES 100000
#define MAX_DATE_CHARS 11
#define MAX_PERSON_CHARS 100
#define MAX_INDIVIDUAL_FEE_NUMBER_CHARS 21

enum exit_codes {SUCCESS,FAILURE};
enum columns {DATE_C, PERSON_C, AMOUNT_C, TOTAL_COLUMNS};
static GtkWidget *amount, *year, *person;
static GtkTextBuffer *error_buffer;
static GtkWidget *error_widget;

struct entry {
	char date_string[MAX_DATE_CHARS];
	char person_string[MAX_PERSON_CHARS];
	double amount_float;
	char amount_string[MAX_INDIVIDUAL_FEE_NUMBER_CHARS];
	unsigned char year;
	unsigned char day;
	unsigned char month;
};

struct entry entries[MAX_ENTRIES];

void do_add(GtkWidget *widget, gpointer model)
{

	double number;
	char *endptr;
	char *amount_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(amount));
	char *year_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(year));
	char *person_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(person));

	if(!strlen(year_ptr)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a date",-1);
		return;
	} else if (!strlen(person_ptr)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a person",-1);
		return;
	} else if (!strlen(amount_ptr)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a fee amount",-1);
		return;
	}

	number = strtod(amount_ptr,&endptr);

	if(endptr == amount_ptr) {
		gtk_text_buffer_set_text(error_buffer,"Fee amount entered has an invalid format."
				" Please enter ascii digits.  You can include a decimal.",-1);
		return;
	} else if (number >= HUGE_VAL || number <= -HUGE_VAL) {
		gtk_text_buffer_set_text(error_buffer,"Fee amount entered has too many digits",-1);
		return;
	} else {
		gtk_text_buffer_set_text(error_buffer," ",-1);
	}

	gtk_list_store_insert_with_values(model, NULL, -1,
					DATE_C, year_ptr,
					PERSON_C, person_ptr,
					AMOUNT_C, number,
					-1);

}

int main(int argc, char **argv)
{

	/* Init variables */
	GtkWidget *window, *grid, *scrolled_window, *add, *box, 
	*year_label, *person_label, *amount_label, *add_label, *tree_view;
	GtkListStore *model;
	GtkTreeViewColumn *column;

	/* Init GTK */
	gtk_init(&argc,&argv);
	
	/* Temp debug info */
	printf("%d\n",LDBL_DIG);
	memset(&entries,0,sizeof(struct entry) * MAX_ENTRIES);

	/* Create vertically oriented box to pack program widgets into */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);

	/* Create window, set title, border width, and size */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window) , "Fee Adder" );
	gtk_container_set_border_width (GTK_CONTAINER(window),10); gtk_window_set_default_size (GTK_WINDOW (window), 300, 500);

	/* Destroy window on close */
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	/* Create grid for value entry */
	grid = gtk_grid_new();

	/* Add grid to box */
	gtk_box_pack_start (GTK_BOX (box), grid, 0, 0, 0);

	/* Add margin to value entry grid */
	gtk_widget_set_margin_top(grid,15);
	gtk_widget_set_margin_start(grid,50);
	gtk_widget_set_margin_end(grid,50);
	gtk_widget_set_margin_bottom(grid,0);

	/* Add year entry to value entry grid */
	year_label = gtk_label_new("Year");
	gtk_grid_attach(GTK_GRID(grid),year_label,0,0,1,1);
	year = gtk_entry_new();
	gtk_widget_set_margin_top(year,2);
	gtk_grid_attach(GTK_GRID(grid),year,0,1,1,1);
	gtk_widget_set_margin_end(year,10);

	/* Add person entry to value entry grid */
	person_label = gtk_label_new("Person");
	gtk_grid_attach(GTK_GRID(grid),person_label,1,0,1,1);
	person = gtk_entry_new();
	gtk_widget_set_margin_top(person,2);
	gtk_grid_attach(GTK_GRID(grid),person,1,1,1,1);
	gtk_widget_set_margin_end(person,10);

	/* Add amount entry to value entry grid */
	amount_label = gtk_label_new("Amount");
	gtk_grid_attach(GTK_GRID(grid),amount_label,2,0,1,1);
	amount = gtk_entry_new();
	gtk_widget_set_margin_top(amount,2);
	gtk_grid_attach(GTK_GRID(grid),amount,2,1,1,1);
	gtk_widget_set_margin_end(amount,10);

	/* Create a scrollable window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	/* Create margin for scrollable window */
	gtk_widget_set_margin_top(scrolled_window,0);
	gtk_widget_set_margin_start(scrolled_window,50);
	gtk_widget_set_margin_end(scrolled_window,50);
	gtk_widget_set_margin_bottom(scrolled_window,0);
	/* Always include a vertical scroll cursor on scroll window */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	/* Fill all space */
	/* Add scrollable window to box */
	gtk_box_pack_start (GTK_BOX (box), scrolled_window, TRUE, TRUE, 0);

	/* Create model for table */
	model = gtk_list_store_new(
		TOTAL_COLUMNS,  /* required parameter, total columns */
		G_TYPE_STRING,  /* Second column, Date, DATE_C */
		G_TYPE_STRING,  /* Third column, Person, PERSON_C */
		G_TYPE_DOUBLE  /* Fourth column, AMOUNT, AMOUNT_C */
		);

	/* Test row */
	/*
	gtk_list_store_insert_with_values(model, NULL, -1,
		DATE_C, "Date",
		PERSON_C, "Person",
		AMOUNT_C, 100.20,
		-1);
	*/

	/* Create tree view for table using model */
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	/* Unref model */
	g_object_unref(model);

	/* Add columns to tree view */
	column = gtk_tree_view_column_new_with_attributes("Date",
		gtk_cell_renderer_spin_new(),
		"text", DATE_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	column = gtk_tree_view_column_new_with_attributes("Person",
		gtk_cell_renderer_text_new(),
		"text", PERSON_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	column = gtk_tree_view_column_new_with_attributes("Amount",
		gtk_cell_renderer_text_new(),
		"text", AMOUNT_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);


	/* Add 'add' button to value entry grid */
	add_label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid),add_label,3,0,1,1);
	add = gtk_button_new_with_label("Add");
	gtk_grid_attach(GTK_GRID(grid), add, 3, 1, 1, 1);

	/* error widget */
	error_buffer = gtk_text_buffer_new(NULL);
	error_widget = gtk_text_view_new_with_buffer(error_buffer);
	gtk_text_buffer_set_text(error_buffer," ",-1);
	gtk_box_pack_start (GTK_BOX (box), error_widget, 0, 0, 0);

	/* After clicking add, call 'do_add' function */
	g_signal_connect(add,"clicked",G_CALLBACK(do_add),model);

	/* Add tree view to scrolled window */
	gtk_container_add(GTK_CONTAINER(scrolled_window),tree_view);	
	gtk_widget_set_hexpand(scrolled_window, TRUE);
	gtk_widget_set_vexpand(scrolled_window, TRUE);
	/* Add box to window */
	gtk_container_add(GTK_CONTAINER(window),box);

	/* Show everything */
	gtk_widget_show_all(box);
	gtk_widget_show_all(scrolled_window);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}

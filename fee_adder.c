/* TODO add to bottom, two total label to add total of all, including unhidden */
/* TODO add buttons to right side: filter, search, edit, delete, hide, hide all, show all, save */
/* TODO add buttons to button: save open */
/* TODO less 0s https://docs.gtk.org/gtk3/treeview-tutorial.html#cell-data-functions */
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <string.h>
#define _ISOC99_SOURCE 
#include <math.h>
#define MAX_ENTRIES 100000
#define MAX_DATE_CHARS 9 
#define MAX_PERSON_CHARS 100 /* adjustable */
#define MAX_INDIVIDUAL_FEE_NUMBER_CHARS 21

enum exit_codes {SUCCESS,FAILURE};
enum columns {DATE_C, PERSON_C, AMOUNT_C, SHOW_C, TOTAL_COLUMNS};
static GtkWidget *amount, *date, *person;
static GtkTextBuffer *error_buffer;
static GtkWidget *error_widget;
static GtkWidget *scrolled_window;
static GtkAdjustment *adj;
static GtkWidget *window;
unsigned char scroll = 0;

/* I only need this if I can't just inactivate a row */
/* don't need it */
/* if uncommenting remember to memset to 0 */
/*
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
*/

void skip_whitespace(char **text_skip) {
	for(;**text_skip == ' ';(*text_skip)++) {}; 
}

gboolean keypress_function(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	GtkWidget *scrollbar_ptr;
	if (event->keyval == GDK_KEY_space){
		printf("test!\n");
		return TRUE;
	}
	return FALSE;
}

void scroll_to_end (GtkWidget *widget, GdkRectangle *allocate, gpointer user_data)
{
	if(scroll) {
		/*
		if (widget != NULL) { }
		if (allocate != NULL) { }

		adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window));
		adj2 = adj;
		gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj));
		*/
		//gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window),adj2);
		

    while(gtk_events_pending()) {
	    gtk_main_iteration();
    }
	gtk_widget_show_all(window);
		GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment (
        GTK_SCROLLED_WINDOW (scrolled_window));
    double upper = gtk_adjustment_get_upper (adjustment);
    double page_size = gtk_adjustment_get_page_size (adjustment);
   // gtk_adjustment_set_value (adjustment, upper - page_size);
    gtk_adjustment_set_value (adjustment, upper - page_size - 0);
	}
	scroll = 0;
}

unsigned char str_to_double(char *str, double *number)
{

	char *endptr;

	if (!strlen(str)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a fee amount",-1);
		return FAILURE;
	}

	*number = strtod(str,&endptr);

	if(endptr == str) {
		gtk_text_buffer_set_text(error_buffer,"Fee amount entered has an invalid format."
				" Please enter ascii digits.  You can include a decimal.",-1);
		return FAILURE;
	} else if (*number >= HUGE_VAL || *number <= -HUGE_VAL) {
		gtk_text_buffer_set_text(error_buffer,"Fee amount entered has too many digits",-1);
		return FAILURE;
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
	} else {
		gtk_text_buffer_set_text(error_buffer," ",-1);
	}

	return SUCCESS;

}

unsigned char validate_person(char *text)
{
	if(!strlen(text)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a person",-1);
		return FAILURE;
	} else if(strlen(text) > MAX_PERSON_CHARS - 1)  {
		gtk_text_buffer_set_text(error_buffer,"Name of person is too long, recompile to allow longer person names",-1);
		return FAILURE;
	}
	return SUCCESS;
}

unsigned char validate_date (char *text)
{
	if(!strlen(text)) {
		gtk_text_buffer_set_text(error_buffer,"Please enter a date",-1);
		return FAILURE;
	} else if(strlen(text) > MAX_DATE_CHARS - 1)  {
		gtk_text_buffer_set_text(error_buffer,"Date is too long, must be in yy/mm/dd format",-1);
		return FAILURE;
	}
	return SUCCESS;
}

void do_add(GtkWidget *widget, gpointer model)
{

	double number;
	/* These data at these pointers can't be modified */
	/* ideally we should copy this into a global array */
	char *date_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(date));
	char *person_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(person));
	char *amount_ptr = (char *)gtk_entry_get_text(GTK_ENTRY(amount));

	if(validate_date(date_ptr) == FAILURE) {
		return;
	}
	if(validate_person(person_ptr) == FAILURE) {
		return;
	}
	if(str_to_double(amount_ptr,&number) == FAILURE)
		return;

	scroll = 1;
	gtk_list_store_insert_with_values(model, NULL, -1,
					DATE_C, date_ptr,
					PERSON_C, person_ptr,
					AMOUNT_C, number,
					SHOW_C, 1, /* 1 for, yes show in tree */
					-1);

	gtk_text_buffer_set_text(error_buffer," ",-1);
	gtk_entry_set_text(GTK_ENTRY(date), "");
	gtk_entry_set_text(GTK_ENTRY(person), "");
	gtk_entry_set_text(GTK_ENTRY(amount), "");

}

int main(int argc, char **argv)
{

	/* Init variables */
	GtkWidget *grid, *add, *box, 
	*date_label, *person_label, *amount_label, *add_label, *tree_view;
	GtkListStore *model;
	GtkTreeViewColumn *column;
	GtkTreeModel *filter;

	/* Init GTK */
	gtk_init(&argc,&argv);
	
	/* Temp debug info */
	printf("Double has a precision of %d digits\n",LDBL_DIG);

	/* Create vertically oriented box to pack program widgets into */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);

	/* Create window, set title, border width, and size */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window) , "Fee Adder" );
	gtk_container_set_border_width (GTK_CONTAINER(window),10); gtk_window_set_default_size (GTK_WINDOW (window), 300, 500);
	gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
	g_signal_connect (G_OBJECT (window), "key_press_event",
        G_CALLBACK (keypress_function), NULL);

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

	/* Add date entry to value entry grid */
	date_label = gtk_label_new("Date (yy/mm/dd)");
	gtk_grid_attach(GTK_GRID(grid),date_label,0,0,1,1);
	date = gtk_entry_new();
	gtk_widget_set_margin_top(date,2);
	gtk_grid_attach(GTK_GRID(grid),date,0,1,1,1);
	gtk_widget_set_margin_end(date,10);

	/* Add month entry to value entry grid */
	/*
	month_label = gtk_label_new("Month");
	gtk_grid_attach(GTK_GRID(grid),month_label,0,0,1,1);
	month = gtk_entry_new();
	gtk_widget_set_margin_top(month,2);
	gtk_grid_attach(GTK_GRID(grid),month,0,1,1,1);
	gtk_widget_set_margin_end(month,10);
	*/

	/* Add day entry to value entry grid */
	/*
	day_label = gtk_label_new("Day");
	gtk_grid_attach(GTK_GRID(grid),day_label,0,0,1,1);
	day = gtk_entry_new();
	gtk_widget_set_margin_top(day,2);
	gtk_grid_attach(GTK_GRID(grid),day,0,1,1,1);
	gtk_widget_set_margin_end(day,10);
	*/

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

	/* Add 'add' button to value entry grid */
	add_label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid),add_label,3,0,1,1);
	add = gtk_button_new_with_label("Add");
	gtk_grid_attach(GTK_GRID(grid), add, 3, 1, 1, 1);

	/* Create a scrollable window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	/* Create margin for scrollable window */
	gtk_widget_set_margin_top(scrolled_window,0);
	gtk_widget_set_margin_start(scrolled_window,50);
	gtk_widget_set_margin_end(scrolled_window,50);
	gtk_widget_set_margin_bottom(scrolled_window,0);
	/* Always include a vertical scroll cursor on scroll window */
 	gtk_widget_set_size_request (scrolled_window, 50, 50);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	/* Add scrollable window to box */
	gtk_box_pack_start (GTK_BOX (box), scrolled_window, TRUE, TRUE, 0);
	//g_object_set (scrolled_window, "vexpand", TRUE, NULL);
        //g_object_set (scrolled_window, "margin", 10, NULL);
    	gtk_widget_show (scrolled_window);

	/* Create liststore for table */
	model = gtk_list_store_new(
		TOTAL_COLUMNS,  /* required parameter, total columns */
		G_TYPE_STRING,  /* Second column, Date, DATE_C */
		G_TYPE_STRING,  /* Third column, Person, PERSON_C */
		G_TYPE_DOUBLE,  /* Fourth column, AMOUNT, AMOUNT_C */
		G_TYPE_BOOLEAN  /* 1 for show, 0 for hide */
		);

	/* Test row */
	/*
	gtk_list_store_insert_with_values(model, NULL, -1,
		DATE_C, "Date",
		PERSON_C, "Person",
		AMOUNT_C, 100.20,
		-1);
	*/

	/* create treestore as filter of liststore */
	filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(model),NULL);
	gtk_tree_model_filter_set_visible_column(GTK_TREE_MODEL_FILTER(filter),3);

	/* Create treeview from treestore */
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(filter));

	/* Scroll the scrollable window to bottom when the size of treeview changes
	 * Default behaviour is to hide new rows for some reason */
	g_signal_connect (tree_view, "size-allocate", G_CALLBACK (scroll_to_end), NULL);

	/* Unref treestore */
	g_object_unref(filter);

	/* Add columns to treeview */
	/* Treeview column 0 */
	column = gtk_tree_view_column_new_with_attributes("Date",
		gtk_cell_renderer_spin_new(),
		"text", DATE_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	/* Treeview column 1 */
	column = gtk_tree_view_column_new_with_attributes("Person",
		gtk_cell_renderer_text_new(),
		"text", PERSON_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	/* Treeview column 2 */
	column = gtk_tree_view_column_new_with_attributes("Amount",
		gtk_cell_renderer_text_new(),
		"text", AMOUNT_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	/* error widget */
	error_buffer = gtk_text_buffer_new(NULL);
	error_widget = gtk_text_view_new_with_buffer(error_buffer);
	gtk_text_buffer_set_text(error_buffer," ",-1);
	gtk_box_pack_start (GTK_BOX (box), error_widget, 0, 0, 0);

	/* After clicking add, call 'do_add' function */
	g_signal_connect(add,"clicked",G_CALLBACK(do_add),model);

	/* Add tree view to scrolled window */
	gtk_container_add(GTK_CONTAINER(scrolled_window),tree_view);	
	//gtk_widget_set_hexpand(scrolled_window, TRUE);
	//gtk_widget_set_vexpand(scrolled_window, TRUE);
	
	/* Add box to window */
	gtk_container_add(GTK_CONTAINER(window),box);

	/* Show everything */
	gtk_widget_show_all(box);
	gtk_widget_show_all(scrolled_window);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}

/* TODO launch external window after filters button */
/* TODO add 'save', 'open' buttons */
/* TODO put border around entire value entry section, to remove awkward look */
/* TODO add 'show all' button */
/* TODO filter out special characters from input */
/* TODO add switch below 'set filters' */
/* TODO see where two numbers added messes up total at bottom, and adjust the range
 * of acceptable values in do_add accordingly */
/* TODO make totals BOLD and BIG */
/* TODO add payment method column */
/* TODO less 0s https://docs.gtk.org/gtk3/treeview-tutorial.html#cell-data-functions */
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <string.h>
#define _ISOC99_SOURCE 
#include <math.h>
#define MAX_ENTRIES 100000
#define MAX_DATE_CHARS 13 
#define MAX_METHOD_CHARS 20 /* adjustable */ 
#define MAX_PERSON_CHARS 100 /* adjustable */
#define MAX_INDIVIDUAL_FEE_NUMBER_CHARS 21
#define FILENAME_SIZE 250

/* enums */
enum exit_codes {SUCCESS,FAILURE};
enum columns {DATE_C, PERSON_C, PAYMENT_METHOD_C, AMOUNT_C, YEAR_C, MONTH_C, DAY_C, SHOW_C, TOTAL_COLUMNS};

/* Init global variables */
GtkWidget *amount_entry, *date_entry, *person_entry, *method_entry;
GtkTextBuffer *error_buffer;
GtkWidget *error_widget;
GtkWidget *scrolled_window;
GtkWidget *total_filtered_results_label;
GtkWidget *total_results_label;
GtkWidget *window;
unsigned char scrolling_to_end;
double filtered_amount_total;
double amount_total;
char filename[FILENAME_SIZE] = "purchase_log.csv";

/* Functions */
void skip_whitespace(char **text_skip) {
	for(;**text_skip == ' ';(*text_skip)++) {}; 
}

/* TODO check error on library functions and handle accordingly */
void save_items(GtkWidget *widget, gpointer model_void) {
	GtkTreeModel *model = (GtkTreeModel *)model_void;
	GtkTreeIter iter;
	char save_message[FILENAME_SIZE + 20] = {0};
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
	gdouble gitem_amount;
	double item_amount;
	FILE *the_file;

	the_file = fopen(filename,"w+");
	snprintf(save_message,sizeof(save_message),"File saved at %s",filename);

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
		item_amount = (double) gitem_amount;

		fprintf(the_file,"%u,%u,%u,%s,%s,%.2lf\n",day_s,month_s,year_s,name_s,method_s,item_amount);

	} while(gtk_tree_model_iter_next(model,&iter));


	fclose(the_file);
	gtk_text_buffer_set_text(error_buffer,save_message,-1);

}

unsigned char add_all_rows(GtkTreeModel *model)
{
	/* first parameter of this function is accepting our base GtkListStore cast to a GtkTreeModel */
	GtkTreeIter iter;
	gboolean is_visible;
	gdouble gitem_amount;
	double item_amount = 0;
	char string_from_double[50] = {0};
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

void scroll_to_end (GtkWidget *widget, GdkRectangle *allocate, gpointer user_data)
{
	if(scrolling_to_end) {
		while(gtk_events_pending()) {
		    gtk_main_iteration();
		}
		gtk_widget_show_all(window);
			GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment (
		GTK_SCROLLED_WINDOW (scrolled_window));
		double upper = gtk_adjustment_get_upper (adjustment);
		double page_size = gtk_adjustment_get_page_size (adjustment);
		gtk_adjustment_set_value (adjustment, upper - page_size - 0);
	}
	scrolling_to_end = 0;
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
	}else if(strlen(text) > MAX_PERSON_CHARS - 1)  {
		gtk_text_buffer_set_text(error_buffer,"Name of person is too long, "
				"recompile to allow longer person names",-1);
		return FAILURE;
	}
	if(strchr(text,',')) {
		gtk_text_buffer_set_text(error_buffer,"Customer name cannot contain commas",-1);
		return FAILURE;
	}
	if(strchr(text,'\n')) {
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
	number = strtol(text,&endptr,10);
	if(endptr == text || number < 0 || number > 32 || *endptr != '/') {
		gtk_text_buffer_set_text(error_buffer,"Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}
	*day_s = number;
	text = endptr + 1;
	/* find month */
	number = strtol(text,&endptr,10);
	if(endptr == text || number < 0 || number > 13 || *endptr != '/') {
		gtk_text_buffer_set_text(error_buffer,"Invalid date, must be in dd/mm/yy format",-1);
		return FAILURE;
	}
	*month_s = number;
	text = endptr + 1;
	/* find year */
	number = strtol(text,&endptr,10);
	if((endptr == text || number < 0 || number > 65534) && !(*endptr == '\0' || *endptr == ' ')) {
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
	if(str_to_double(amount_ptr,&number) == FAILURE)
		return;
	printf("added number: %lf\n",amount_total+number);
	if(amount_total + number >= 7777777777.55 || amount_total + number <= -7777777777.55) {
		gtk_text_buffer_set_text(error_buffer,"Cannot add amount. Amount total out of range, should be between"
				" -7777777777.55 and 7777777777.55",-1);
		return;
	}

	scrolling_to_end = 1;
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

/* Main */
int main(int argc, char **argv)
{

	/* Init local variables */
	GtkWidget *grid, *grid2, *box, *box2, *date_label, *person_label, 
		  *amount_label, *add_label, *tree_view;
	GtkWidget *filter_button, *search_button, *edit_button, *add_button, 
		*hide_button, *delete_button, *hide_all_button, *show_all_button, *save_button,
			*method_label;
	GtkTreeViewColumn *column, *column1, *column2, *column3;
	GtkListStore *model;
	GtkTreeModel *filter;

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
	g_signal_connect (G_OBJECT (window), "key_press_event",
        G_CALLBACK (keypress_function), NULL);
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
	person_label = gtk_label_new("Customer");
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
	GtkWidget *box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);
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
		G_TYPE_STRING,  /* Second column, date, DATE_C */
		G_TYPE_STRING,  /* Third column, person, PERSON_C */
		G_TYPE_STRING,  /* Fourth column, payment method, PAYMENT_METHOD_C */
		G_TYPE_DOUBLE,  /* Fifth column, amount, AMOUNT_C */
		G_TYPE_UINT,   /* Sixth column, year, YEAR_C */
		G_TYPE_UCHAR,   /* Seventh column, month, MONTH_C */
		G_TYPE_UCHAR,   /* Eight column, day, DAY_C */
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
	column3 = gtk_tree_view_column_new_with_attributes("Shekels",
		gtk_cell_renderer_text_new(),
		"text", AMOUNT_C,
		NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column3);
	gtk_tree_view_column_set_expand ( column3, TRUE);
	gtk_tree_view_column_set_fixed_width (column3,100);


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

	/* Create hide button */
	hide_button = gtk_button_new_with_label("Hide");
	//gtk_grid_attach(GTK_GRID(grid2), hide_button, 0, 4, 1, 1);

	/* Create Hide all button */
	hide_all_button = gtk_button_new_with_label("Hide all");
	//gtk_grid_attach(GTK_GRID(grid2), hide_all_button, 0, 5, 1, 1);

	/* Create Show all button */
	show_all_button = gtk_button_new_with_label("Show all");
	//gtk_grid_attach(GTK_GRID(grid2), show_all_button, 0, 6, 1, 1);

	/* Create Save button */
	save_button = gtk_button_new_with_label("Save");
	gtk_grid_attach(GTK_GRID(grid2), save_button, 0, 7, 1, 1);

	/* Pack grid2 to box2 */
	gtk_box_pack_start (GTK_BOX (box2), grid2, TRUE, TRUE, 0);

	/* Align grid2 */
        gtk_widget_set_halign (grid2, GTK_ALIGN_START);
        gtk_widget_set_valign (grid2, GTK_ALIGN_START);

	/* create seperator for before totals */
	GtkWidget *before_totals_seperator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
 	gtk_widget_set_size_request (before_totals_seperator, 40, 2);

	/* Create grid for totals */
	GtkWidget *grid3, *filters_applied_label, *filters_applied_results_label, *total_filtered_label,
		  *total_label;
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

	/* After clicking add, call 'do_add' function */
	g_signal_connect(add_button,"clicked",G_CALLBACK(do_add),model);
	g_signal_connect(save_button,"clicked",G_CALLBACK(save_items),GTK_TREE_MODEL(model));

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

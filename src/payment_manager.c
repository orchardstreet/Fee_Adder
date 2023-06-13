/* TODO change %ld to %llu everywhere!!! */
/* TODO launch external window after filters button */
/* TODO filter out special characters from input */
/* TODO add switch below 'set filters' */
/* TODO make totals BOLD and BIG */
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
#include "headers/filter.h"

/* Init global variables */
GtkWidget *amount_entry, *date_entry, *person_entry, *method_entry, *paid_check_button;
GtkTextBuffer *error_buffer;
GtkWidget *error_widget;
GtkWidget *scrolled_window;
GtkWidget *total_filtered_results_label;
GtkWidget *total_results_label;
unsigned long long filtered_amount_total;
unsigned long long amount_total;
char filename[FILENAME_SIZE] = "purchase_log.csv";
unsigned char is_scrolling = 0;
struct windows_list windows;



/* Main */
int main(int argc, char **argv)
{

	/* Init local variables */
	GtkWidget *grid, *grid2, *grid3, *box, *box2, *box3, *date_label, *person_label,
		*amount_label, *add_label, *tree_view, *filter_button, *search_button, *edit_button, *add_button,
		*delete_button, *save_button, *before_totals_seperator, *method_label,
		*total_filtered_label, *total_label, *paid_label, *paid_status_label;
	GtkTreeViewColumn *column0, *column1, *column2, *column3, *column4;
	GtkListStore *model;
	GtkTreeModel *filter;
	GtkTreeModel *sorted_model;
	struct treeview_models models;

	/* Init GTK */
	gtk_init(&argc,&argv);

	printf("\n~~~~~~~ Payment manager 0.1 ~~~~~~~\n\n");
	printf("%u\n",G_MAXUINT);

	/* Create vertically oriented box to pack program widgets into */
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	GtkStyleContext *window_context = gtk_widget_get_style_context(box);
	gtk_style_context_add_class(window_context,"custom_window");

	/* Create window, set title, border width, and size */
	windows.main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(windows.main_window) , "Payment Manager" );
	gtk_container_set_border_width (GTK_CONTAINER(windows.main_window),10);
	gtk_widget_add_events(windows.main_window, GDK_KEY_PRESS_MASK);
	gtk_window_set_default_size ( GTK_WINDOW(windows.main_window), 800, 580);

	/* Destroy window on close */
	g_signal_connect(windows.main_window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	/* init CSS */
	char *app_css =  "window {background-color:#f3f3f3}"
					".custom_frame {margin-top:10px;box-shadow: rgba(0, 0, 0, 0.1) 0px 1px 2px 0px;background-color:#fbfbfb;border-radius:8px;}"
				     ".custom_frame > border {border-radius: 8px; padding: 10px 87px 10px 87px;background-color:#fbfbfb;}"
					".custom_body {border:1px solid #d5d0cc;box-shadow: rgba(0, 0, 0, 0.1) 0px 1px 2px 0px;border-radius:8px;padding-left:50px;padding-right:50px;padding-bottom:50px;padding-top:20px;background-color:#fbfbfb;margin-bottom:10px;}"
					".custom_table {border:1px solid #abadb3;}"
					".custom_treeview {}";

	GtkCssProvider* css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css_provider,app_css,-1,NULL);
    GdkScreen* screen = gdk_screen_get_default();
	gtk_style_context_add_provider_for_screen (screen,GTK_STYLE_PROVIDER(css_provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


	/* Create a frame for value entries */
	GtkWidget *frame = gtk_frame_new("Add Row");
	gtk_box_pack_start (GTK_BOX (box), frame, FALSE, TRUE, 0);
	gtk_frame_set_label_align (GTK_FRAME(frame),0.5,0);
	/* align frame */
    gtk_widget_set_halign (frame, GTK_ALIGN_CENTER);
	/* style frame */
	GtkStyleContext *frame_context = gtk_widget_get_style_context(frame);
	gtk_style_context_add_class(frame_context,"custom_frame");


	/* Create grid for value entry */
	grid = gtk_grid_new();
	gtk_grid_set_row_spacing (GTK_GRID(grid),2);
	gtk_grid_set_column_spacing (GTK_GRID(grid),10);
	/* Add grid to frame */
	gtk_container_add(GTK_CONTAINER(frame),grid);

	/* Add date entry to value entry grid */
	date_label = gtk_label_new("Date");
        gtk_widget_set_halign (date_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(date_label,3);
	gtk_grid_attach(GTK_GRID(grid),date_label,0,0,1,1);
	date_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),date_entry,0,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(date_entry), 10);

	/* Add person entry to value entry grid */
	person_label = gtk_label_new("Customer");
        gtk_widget_set_halign (person_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(person_label,3);
	gtk_grid_attach(GTK_GRID(grid),person_label,1,0,1,1);
	person_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),person_entry,1,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(person_entry), 22);

	/* Add payment method to value entry grid */
	method_label = gtk_label_new("Method");
        gtk_widget_set_halign (method_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(method_label,3);
	gtk_grid_attach(GTK_GRID(grid),method_label,2,0,1,1);
	method_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),method_entry,2,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(method_entry), 9);

	/* Add amount entry to value entry grid */
	amount_label = gtk_label_new("Shekels");
        gtk_widget_set_halign (amount_label, GTK_ALIGN_START);
	gtk_widget_set_margin_start(amount_label,3);
	gtk_grid_attach(GTK_GRID(grid),amount_label,3,0,1,1);
	amount_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),amount_entry,3,1,1,1);
	gtk_entry_set_width_chars (GTK_ENTRY(amount_entry), 8);

	/* Add paid entry to value entry grid */
	paid_label = gtk_label_new("Paid?");
	paid_status_label = gtk_label_new("Yes");
	gtk_widget_set_margin_start(paid_status_label,4);
    gtk_widget_set_halign (paid_label, GTK_ALIGN_START);
	//gtk_grid_attach(GTK_GRID(grid),paid_label,4,0,1,1);
	paid_check_button = gtk_check_button_new_with_label("Paid");
	gtk_widget_set_margin_end(paid_check_button,3);
	gtk_grid_attach(GTK_GRID(grid),paid_check_button,4,1,1,1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(paid_check_button),1);

	/* Add 'add' button to value entry grid */
	add_label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid),add_label,5,0,1,1);
	add_button = gtk_button_new_with_label("Add");
	gtk_grid_attach(GTK_GRID(grid), add_button, 5, 1, 1, 1);

	/* Create horizontally oriented box to horizontally pack progam widgets into */
	box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,20);
	GtkStyleContext *body_context = gtk_widget_get_style_context(box2);
	gtk_style_context_add_class(body_context,"custom_body");
	/* Pack horizontal box into vertical box */
	gtk_box_pack_start (GTK_BOX (box), box2, TRUE, TRUE, 0);
	/* Align horizontal box */
        gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);

	/* Create another vertically oriented box to pack into last hbox
	 * this will contain the scrolled window and totals */
	box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);
	gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, TRUE, 0);

	/* Create a scrollable window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_margin_start(scrolled_window,40);
	/* Set preferred size of scrollable window */
 	gtk_widget_set_size_request (scrolled_window, 460, 300);
	/* Always include a vertical scroll cursor on scroll window */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_overlay_scrolling (GTK_SCROLLED_WINDOW(scrolled_window),FALSE);
	/* Add scrollable window to horizontal box */
	gtk_box_pack_start (GTK_BOX (box3), scrolled_window, TRUE, TRUE, 0);
	/* Align scrollable window */
        gtk_widget_set_halign (scrolled_window, GTK_ALIGN_START);
	//g_object_set (scrolled_window, "vexpand", TRUE, NULL);
	//g_object_set (scrolled_window, "hexpand", FALSE, NULL);
	/* set style for scrolled window */
	GtkStyleContext *table_context = gtk_widget_get_style_context(scrolled_window);
	gtk_style_context_add_class(table_context,"custom_table");
    //gtk_widget_show (scrolled_window);


	/* Create grid for right-side buttons */
	grid2 = gtk_grid_new();
	gtk_grid_set_row_spacing (GTK_GRID(grid2),20);
	gtk_widget_set_margin_top(grid2,25);

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
		G_TYPE_BOOLEAN,  /* seventh column, paid, PAID_C */
		G_TYPE_UINT,  /* eighth column, date sort DATE_SORT_C */
		G_TYPE_BOOLEAN  /* 1 for show, 0 for hide */
		);
	models.liststore = model;

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
	models.filtered_model = filter;

	/* Add box to window */
	gtk_container_add(GTK_CONTAINER(windows.main_window),box);


	/* Load items */
	printf("Loading file from disk...\n");
	if(load_items(model) == SUCCESS) {
		printf("[SUCCESS] file items loaded successfully\n\n");
	} else {
		printf("[FAILURE] file could not be loaded successfully\n\n");
	}

	/* Create sorted model */
	sorted_model = gtk_tree_model_sort_new_with_model(filter);
	/* Sort items by date using DATE_SORT_C column of liststore (which is in format of yyyyymmdd) */
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (sorted_model), DATE_SORT_C, GTK_SORT_ASCENDING);
	models.sorted_model = sorted_model;


	/* Create treeview from treestore */
	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(sorted_model));
	/* Add tree view to scrolled window */
	gtk_container_add(GTK_CONTAINER(scrolled_window),tree_view);
	/* Style for treeview */
	GtkStyleContext *treeview_context = gtk_widget_get_style_context(tree_view);
	gtk_style_context_add_class(treeview_context,"custom_treeview");

	/* Scroll the scrollable window to bottom when the size of treeview changes
	 * Default behaviour is to hide new rows at bottom for some reason */
	g_signal_connect (tree_view, "size-allocate", G_CALLBACK (scroll_to_end), NULL);

	/* Unref treestore */
	g_object_unref(filter);

	/* Add columns to treeview */
	/* Treeview column 0, Date */
	column0 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column0,"Date");
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),column0);
	GtkCellRenderer *renderer0;
	renderer0 = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column0,renderer0,TRUE);
	gtk_tree_view_column_add_attribute(column0,renderer0,"text",DATE_C);
	//gtk_tree_view_column_set_expand ( column0, TRUE);
	gtk_tree_view_column_set_fixed_width (column0,90);
	gtk_tree_view_column_set_sort_column_id(column0,DATE_SORT_C);
	gtk_tree_view_column_set_resizable (column0, TRUE);
	gtk_tree_view_column_set_sizing (column0, GTK_TREE_VIEW_COLUMN_FIXED);
	g_object_set(renderer0, "editable", TRUE, NULL);
	g_signal_connect(renderer0, "edited", G_CALLBACK(date_edited_callback), &models);


	/* Treeview column 1, Customer */
	column1 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column1,"Customer");
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),column1);
	GtkCellRenderer *renderer1;
	renderer1 = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column1,renderer1,TRUE);
	gtk_tree_view_column_add_attribute(column1,renderer1,"text",PERSON_C);
	gtk_tree_view_column_set_fixed_width (column1,160);
	gtk_tree_view_column_set_sort_column_id(column1,PERSON_C);
	gtk_tree_view_column_set_resizable (column1, TRUE);
	gtk_tree_view_column_set_sizing (column1, GTK_TREE_VIEW_COLUMN_FIXED);
	g_object_set(renderer1, "editable", TRUE, NULL);
	g_signal_connect(renderer1, "edited", G_CALLBACK(customer_edited_callback), &models);


	/* Treeview column 2, Method */
	column2 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column2,"Method");
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),column2);
	GtkCellRenderer *renderer2;
	renderer2 = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column2,renderer2,TRUE);
	gtk_tree_view_column_add_attribute(column2,renderer2,"text",PAYMENT_METHOD_C);
	//gtk_tree_view_column_set_expand ( column2, TRUE);
	gtk_tree_view_column_set_fixed_width (column2,90);
	gtk_tree_view_column_set_sort_column_id(column2,PAYMENT_METHOD_C);
	gtk_tree_view_column_set_resizable (column2, TRUE);
	gtk_tree_view_column_set_sizing (column2, GTK_TREE_VIEW_COLUMN_FIXED);
	g_object_set(renderer2, "editable", TRUE, NULL);
	g_signal_connect(renderer2, "edited", G_CALLBACK(method_edited_callback), &models);

	/* Treeview column 3, Amount */
	column3 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column3,"Shekels");
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),column3);
	GtkCellRenderer *renderer3;
	renderer3 = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column3,renderer3,TRUE);
	gtk_tree_view_column_add_attribute(column3,renderer3,"text",AMOUNT_C);
	/*
	g_object_set(renderer3,
				 "weight",PANGO_WEIGHT_BOLD,
				 "weight-set", TRUE,
				 NULL);
				 */
	gtk_tree_view_column_set_cell_data_func(column3,renderer3,amount_cell_data_func,NULL, NULL);
	gtk_tree_view_column_set_fixed_width (column3,60);
	gtk_tree_view_column_set_sort_column_id(column3,AMOUNT_C);
	gtk_tree_view_column_set_resizable (column3, TRUE);
	gtk_tree_view_column_set_sizing (column3, GTK_TREE_VIEW_COLUMN_FIXED);
	g_object_set(renderer3, "editable", TRUE, NULL);
	g_object_set (G_OBJECT (renderer3), "xalign", (gfloat) 1.0, NULL);
	g_signal_connect(renderer3, "edited", G_CALLBACK(amount_edited_callback), &models);

	/* Treeview column 4, Is paid */
	column4 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column4,"Paid");
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),column4);
	GtkCellRenderer *renderer4;
	renderer4 = gtk_cell_renderer_toggle_new();
	gtk_tree_view_column_pack_start(column4,renderer4,FALSE);
	gtk_tree_view_column_add_attribute(column4,renderer4,"active",PAID_C);
	//gtk_cell_renderer_toggle_set_active(GTK_CELL_RENDERER_TOGGLE(renderer4),TRUE);
	//gtk_cell_renderer_toggle_set_activatable (GTK_CELL_RENDERER_TOGGLE(renderer4),TRUE);
	gtk_tree_view_column_set_fixed_width (column4,40);
	gtk_tree_view_column_set_sort_column_id(column4,PAID_C);
	gtk_tree_view_column_set_resizable (column4, FALSE);
	gtk_tree_view_column_set_sizing (column4, GTK_TREE_VIEW_COLUMN_FIXED);
	//g_object_set(renderer4, "editable", TRUE, NULL);
	//g_object_set (G_OBJECT (renderer3), "xalign", (gfloat) 1.0, NULL);
	g_signal_connect(renderer4, "toggled", G_CALLBACK(toggle_paid), &models);

	//gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(tree_view,TRUE);

	if(check_system_compatibility() == SUCCESS) {
		printf("[SUCCESS] Using a compatible system\n\n");
	}
	/* load items from csv file into liststore, treestore, and treeview */

	/* After clicking add, call 'do_add' function */
	g_signal_connect(add_button,"clicked",G_CALLBACK(add_single_transaction_from_entryboxes),model);
	/* After clicking save, call 'save_items */
	g_signal_connect(save_button,"clicked",G_CALLBACK(save_items),GTK_TREE_MODEL(model));
	/* After pressing a keyboard button, called 'key_press_event */
	g_signal_connect (G_OBJECT (windows.main_window), "key_press_event", G_CALLBACK (keypress_function), NULL);
	g_signal_connect (filter_button, "clicked", G_CALLBACK (create_filter_window), NULL);

	//gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),NULL);
	/* Show everything */
	gtk_widget_show_all(windows.main_window);
	is_scrolling = 1;
	scroll_to_end();
	is_scrolling = 0;

	gtk_main();

	return 0;
}

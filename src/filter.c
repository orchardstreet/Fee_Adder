#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/filter.h"
#include "headers/config.h"
#define FILTER_WINDOW_WIDTH 400 /* adjustable */
#define FILTER_WINDOW_HEIGHT 300 /* adjustable */

struct filter_option_list {
    GtkWidget *show_only_paid_this_month_toggle;
    GtkWidget *show_only_unpaid_this_month_toggle;
    GtkWidget *no_paid_status_filter_toggle;
    GtkWidget *single_month_filter_toggle;
    GtkWidget *date_range_filter_toggle;
    GtkWidget *no_date_filter_toggle;
};
struct filter_settings_list {
	unsigned char show_only_paid_this_month;
};

struct filter_settings_list filter_settings;
struct filter_option_list filter_options;

static void close_child_window (GtkWidget *widget, GtkWidget **child)
{
    (void)widget;
	*child = NULL;
}

void toggle_show_only_paid_this_month(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
	gboolean is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(filter_options.show_only_paid_this_month_toggle));
    if(is_active){
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.show_only_unpaid_this_month_toggle),0);
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.no_paid_status_filter_toggle),0);
    }
}

void toggle_show_only_unpaid_this_month(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
	gboolean is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(filter_options.show_only_unpaid_this_month_toggle));
    if(is_active){
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.show_only_paid_this_month_toggle),0);
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.no_paid_status_filter_toggle),0);
    }
}

void toggle_no_paid_status_filter(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
	gboolean is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(filter_options.no_paid_status_filter_toggle));
    if(is_active){
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.show_only_paid_this_month_toggle),0);
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.show_only_unpaid_this_month_toggle),0);
    }
}

void destroy_widget (GtkWidget * widget, GtkWidget *widget_to_destroy)
{
    (void)widget;
	gtk_widget_destroy (widget_to_destroy);
}

static void apply_filter_settings(void) {

}

void create_filter_window(GtkWidget *widget, gpointer data) {

    GtkWidget *parent_vbox, *frame0, *frame0_vbox, *frame1_vbox,
    *frame1,*cancel_apply_hbox, *cancel_button, *apply_button,
    *single_month_grid, *date_range_grid, *single_month_label, *single_month_entry,
    *error_label, *date_range_label, *date_entry_start, *date_entry_end, *empty_label;
    (void)widget;
    (void)data;

    /* Was already initialized to NULL as a global variable
     * If the window exists, don't try to open it again */
    if(windows.filter_window) {
        gtk_window_present (GTK_WINDOW (windows.filter_window));
        return;
    }

    /* create the window */
    windows.filter_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (windows.filter_window), "Filter items");
    gtk_window_set_default_size (GTK_WINDOW (windows.filter_window), FILTER_WINDOW_WIDTH, FILTER_WINDOW_HEIGHT);
    gtk_window_set_transient_for (GTK_WINDOW (windows.filter_window), GTK_WINDOW (windows.main_window));
    gtk_window_set_destroy_with_parent (GTK_WINDOW (windows.filter_window), TRUE);

    /* create parent vbox */
    parent_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_container_add (GTK_CONTAINER (windows.filter_window), parent_vbox);

    /* create quick filter frame */
	frame0 = gtk_frame_new("Date filter");
 	gtk_widget_set_size_request (frame0, FILTER_WINDOW_WIDTH - (FILTER_WINDOW_WIDTH / 15), 100);
	gtk_box_pack_start (GTK_BOX (parent_vbox), frame0, FALSE, TRUE, 15);
	/* align frame */
	//gtk_frame_set_label_align (GTK_FRAME(frame0),0.5,0);
    gtk_widget_set_halign (frame0, GTK_ALIGN_CENTER);
    /* create vbox in frame0 */
    frame0_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_container_add (GTK_CONTAINER (frame0), frame0_vbox);

    /* no date filter checkmark */
	filter_options.no_date_filter_toggle = gtk_check_button_new_with_label("No date filter");
	gtk_widget_set_margin_top(filter_options.no_date_filter_toggle,9);
	gtk_widget_set_margin_start(filter_options.no_date_filter_toggle,7);
	gtk_container_add(GTK_CONTAINER(frame0_vbox),filter_options.no_date_filter_toggle);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.no_date_filter_toggle),1);

    /* date grids */
    /* single month grid */
	single_month_grid = gtk_grid_new();
	gtk_widget_set_margin_start(single_month_grid,10);
	gtk_widget_set_margin_top(single_month_grid,3);
	gtk_widget_set_margin_bottom(single_month_grid,0);
	gtk_grid_set_column_spacing (GTK_GRID(single_month_grid),10);
    /* date range grid */
	date_range_grid = gtk_grid_new();
	gtk_widget_set_margin_start(date_range_grid,10);
	gtk_widget_set_margin_top(date_range_grid,3);
	gtk_widget_set_margin_bottom(date_range_grid,16);
	gtk_grid_set_column_spacing (GTK_GRID(date_range_grid),10);

    /* single month entry */
    /* label */
	filter_options.single_month_filter_toggle = gtk_check_button_new_with_label("Single month filter: ");
 	gtk_widget_set_size_request (filter_options.single_month_filter_toggle, 30, 20);
    gtk_widget_set_halign (filter_options.single_month_filter_toggle, GTK_ALIGN_START);
	gtk_widget_set_margin_top(filter_options.single_month_filter_toggle,12);
	gtk_widget_set_margin_start(filter_options.single_month_filter_toggle,7);
	gtk_container_add(GTK_CONTAINER(frame0_vbox),filter_options.single_month_filter_toggle);
    /* entry */
	single_month_entry = gtk_entry_new();
	gtk_entry_set_width_chars (GTK_ENTRY(single_month_entry), 12);
    gtk_entry_set_placeholder_text(GTK_ENTRY(single_month_entry),"mm/yy");
	gtk_container_add(GTK_CONTAINER(frame0_vbox),single_month_grid);

    /* date range entry */
    /* label */
	filter_options.date_range_filter_toggle = gtk_check_button_new_with_label("Date range filter: ");
 	gtk_widget_set_size_request (filter_options.date_range_filter_toggle, 30, 20);
    gtk_widget_set_halign (filter_options.date_range_filter_toggle, GTK_ALIGN_START);
	gtk_widget_set_margin_start(filter_options.date_range_filter_toggle,7);
	gtk_widget_set_margin_top(filter_options.date_range_filter_toggle,12);
	gtk_container_add(GTK_CONTAINER(frame0_vbox),filter_options.date_range_filter_toggle);
    /* entry */
	date_entry_start = gtk_entry_new();
	date_entry_end = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(date_entry_start),"dd/mm/yy");
    gtk_entry_set_placeholder_text(GTK_ENTRY(date_entry_end),"dd/mm/yy");
	gtk_entry_set_width_chars (GTK_ENTRY(date_entry_start), 12);
	gtk_entry_set_width_chars (GTK_ENTRY(date_entry_end), 12);
	gtk_container_add(GTK_CONTAINER(frame0_vbox),date_range_grid);

    /* attach all date items to date grids */
    /* single month */
	gtk_grid_attach(GTK_GRID(single_month_grid),single_month_entry,0,0,1,1);
    /* date range */
	gtk_grid_attach(GTK_GRID(date_range_grid),date_entry_start,0,0,1,1);
	gtk_grid_attach(GTK_GRID(date_range_grid),date_entry_end,1,0,1,1);

    /* create custom filter frame */
	frame1 = gtk_frame_new("Paid status filter");
 	gtk_widget_set_size_request (frame1, FILTER_WINDOW_WIDTH - (FILTER_WINDOW_WIDTH / 15), 100);
	gtk_box_pack_start (GTK_BOX (parent_vbox), frame1, FALSE, TRUE, 0);
	/* align frame */
	//gtk_frame_set_label_align (GTK_FRAME(frame1),0.5,0);
    gtk_widget_set_halign (frame1, GTK_ALIGN_CENTER);
    /* create vbox in frame1 */
    frame1_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_widget_set_margin_top(frame1_vbox,5);
	gtk_widget_set_margin_bottom(frame1_vbox,5);
	gtk_widget_set_margin_start(frame1_vbox,7);
    gtk_container_add (GTK_CONTAINER (frame1), frame1_vbox);
	filter_options.no_paid_status_filter_toggle = gtk_check_button_new_with_label("No paid status filter");
	gtk_box_pack_start (GTK_BOX (frame1_vbox), filter_options.no_paid_status_filter_toggle, FALSE, TRUE, 3);
	filter_options.show_only_paid_this_month_toggle = gtk_check_button_new_with_label("Only paid transactions");
	gtk_box_pack_start (GTK_BOX (frame1_vbox), filter_options.show_only_paid_this_month_toggle, FALSE, TRUE, 3);
    /* create checkmark for paid this month filter */
	filter_options.show_only_unpaid_this_month_toggle = gtk_check_button_new_with_label("Only unpaid transactions");
	gtk_box_pack_start (GTK_BOX (frame1_vbox), filter_options.show_only_unpaid_this_month_toggle, FALSE, TRUE, 3);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(filter_options.no_paid_status_filter_toggle),1);


    /* create cancel/apply hbox */
    cancel_apply_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start (GTK_BOX (parent_vbox), cancel_apply_hbox, TRUE, TRUE, 10);
    /* add buttons to hbox */
	error_label = gtk_label_new("");
	gtk_box_pack_start (GTK_BOX (cancel_apply_hbox), error_label, TRUE, TRUE, 15);
    gtk_widget_set_valign (error_label, GTK_ALIGN_END);
    gtk_widget_set_halign (error_label, GTK_ALIGN_START);
	cancel_button = gtk_button_new_with_label("Cancel");
	apply_button = gtk_button_new_with_label("Apply");
	gtk_box_pack_start (GTK_BOX (cancel_apply_hbox), cancel_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (cancel_apply_hbox), apply_button, FALSE, TRUE, 10);
    gtk_widget_grab_focus (apply_button);
    gtk_widget_set_valign (cancel_button, GTK_ALIGN_END);
    gtk_widget_set_valign (apply_button, GTK_ALIGN_END);
    gtk_widget_set_halign (cancel_button, GTK_ALIGN_END);
    gtk_widget_set_halign (apply_button, GTK_ALIGN_END);

    /* end of function */
    gtk_widget_show_all(windows.filter_window);
	g_signal_connect (cancel_button, "clicked", G_CALLBACK (destroy_widget), windows.filter_window);
	g_signal_connect (apply_button, "clicked", G_CALLBACK (apply_filter_settings),NULL);
	g_signal_connect (apply_button, "clicked", G_CALLBACK (create_filter_window), NULL);
    g_signal_connect (G_OBJECT (windows.filter_window), "destroy", G_CALLBACK (close_child_window), &windows.filter_window);
	g_signal_connect(filter_options.show_only_paid_this_month_toggle, "toggled", G_CALLBACK(toggle_show_only_paid_this_month), NULL);
	g_signal_connect(filter_options.show_only_unpaid_this_month_toggle, "toggled", G_CALLBACK(toggle_show_only_unpaid_this_month), NULL);
	g_signal_connect(filter_options.no_paid_status_filter_toggle, "toggled", G_CALLBACK(toggle_no_paid_status_filter), NULL);
}

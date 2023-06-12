#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "headers/filter.h"
#include "headers/config.h"
#define FILTER_WINDOW_WIDTH 400 /* adjustable */
#define FILTER_WINDOW_HEIGHT 400 /* adjustable */

static void close_child_window (GtkWidget *widget, GtkWidget **child)
{
    (void)widget;
	*child = NULL;
}

void create_filter_window(GtkWidget *widget, gpointer data) {

    GtkWidget *parent_vbox, *frame0, *frame1;
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
	frame0 = gtk_frame_new("Quick filter");
 	gtk_widget_set_size_request (frame0, FILTER_WINDOW_WIDTH - (FILTER_WINDOW_WIDTH / 15), 100);
	gtk_box_pack_start (GTK_BOX (parent_vbox), frame0, FALSE, TRUE, 15);
	/* align frame */
	//gtk_frame_set_label_align (GTK_FRAME(frame0),0.5,0);
    gtk_widget_set_halign (frame0, GTK_ALIGN_CENTER);

    /* create custom filter frame */
	frame1 = gtk_frame_new("Custom filter");
 	gtk_widget_set_size_request (frame1, FILTER_WINDOW_WIDTH - (FILTER_WINDOW_WIDTH / 15), 100);
	gtk_box_pack_start (GTK_BOX (parent_vbox), frame1, FALSE, TRUE, 0);
	/* align frame */
	//gtk_frame_set_label_align (GTK_FRAME(frame1),0.5,0);
    gtk_widget_set_halign (frame1, GTK_ALIGN_CENTER);

    /* end of function */
    gtk_widget_show_all(windows.filter_window);
    g_signal_connect (G_OBJECT (windows.filter_window), "destroy", G_CALLBACK (close_child_window), &windows.filter_window);

}

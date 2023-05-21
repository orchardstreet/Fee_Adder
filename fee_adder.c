#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <string.h>
#define MAX_ENTRIES 100000
#define MAX_YEAR_CHARS 5
#define MAX_PERSON_CHARS 100
#define MAX_INDIVIDUAL_FEE_NUMBER_CHARS 21

enum exit_codes {SUCCESS,FAILURE};
static GtkWidget *year;
static GtkWidget *person;
static GtkWidget *amount;
static GtkWidget *year_label;
static GtkWidget *person_label;
static GtkWidget *amount_label;
static GtkWidget *add_label;
static GtkWidget *add_label2;
GtkWidget *listbox1;

struct entry {
	long double amount_float;
	char amount_string[MAX_INDIVIDUAL_FEE_NUMBER_CHARS];
	char year[MAX_YEAR_CHARS];
	char person[MAX_PERSON_CHARS];
};

struct entry entries[MAX_ENTRIES];

static GtkWidget * create_row (const gchar *text)
{
  GtkWidget *row, *handle, *box, *label, *image;

  printf("adding %s\n",text);
  row = gtk_list_box_row_new ();

  label = gtk_label_new (text);
  gtk_container_add (GTK_CONTAINER (row), label);


  return row;
}

void do_add(void)
{
	GtkWidget *row;
	/*
	if(row_index >= MAX_ENTRIES) {
		fprintf(stderr,"Max entries reached: %d\n",MAX_ENTRIES);
		exit(EXIT_FAILURE);
	}
	*/

	//rows[row_index] = create_row("new");
	row = create_row("new");
	gtk_list_box_insert (GTK_LIST_BOX (listbox1), row, -1 );
	gtk_widget_show_all(listbox1);
	//gtk_widget_show(listbox1);
	//row_index++;
}

//GtkWidget *rows[MAX_ENTRIES];
unsigned long row_index = 0;

int main(int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *scrolled_window;
	GtkWidget *add;
	GtkWidget *box;
	GtkWidget *row3;
	gtk_init(&argc,&argv);
	
	printf("%d\n",LDBL_DIG);
	memset(&entries,0,sizeof(struct entry) * MAX_ENTRIES);
	//memset(&rows,0,sizeof(rows) * MAX_ENTRIES);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

	grid = gtk_grid_new();
	//gtk_container_add(GTK_CONTAINER(window),grid);	
	gtk_box_pack_start (GTK_BOX (box), grid, 0, 0, 0);

	gtk_window_set_title (GTK_WINDOW(window) , "Fee Addr" );
	gtk_container_set_border_width (GTK_CONTAINER(window),10);

	gtk_widget_set_margin_top(grid,10);
	gtk_widget_set_margin_start(grid,50);
	gtk_widget_set_margin_end(grid,50);
	gtk_widget_set_margin_bottom(grid,30);

	year_label = gtk_label_new("Year");
	gtk_grid_attach(GTK_GRID(grid),year_label,0,0,1,1);
	year = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),year,0,1,1,1);
	gtk_widget_set_margin_end(year,10);

	person_label = gtk_label_new("Person");
	gtk_grid_attach(GTK_GRID(grid),person_label,1,0,1,1);
	person = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),person,1,1,1,1);
	gtk_widget_set_margin_end(person,10);

	amount_label = gtk_label_new("Amount");
	gtk_grid_attach(GTK_GRID(grid),amount_label,2,0,1,1);
	amount = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid),amount,2,1,1,1);
	gtk_widget_set_margin_end(amount,10);

	add_label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid),add_label,3,0,1,1);
	add = gtk_button_new_with_label("Add");
	gtk_grid_attach(GTK_GRID(grid), add, 3, 1, 1, 1);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_margin_top(scrolled_window,10);
	gtk_widget_set_margin_start(scrolled_window,50);
	gtk_widget_set_margin_end(scrolled_window,50);
	gtk_widget_set_margin_bottom(scrolled_window,100);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	gtk_box_pack_start (GTK_BOX (box), scrolled_window, 0, 0, 0);


	listbox1 = gtk_list_box_new();
	gtk_container_add(GTK_CONTAINER(scrolled_window),listbox1);	
	g_signal_connect(add,"clicked",G_CALLBACK(do_add),NULL);
	add_label2 = gtk_label_new("test");
	//gtk_container_add(GTK_CONTAINER(listbox1),add_label2);	
	//gtk_list_box_insert (GTK_LIST_BOX (listbox1), add_label2, -1 );
	//gtk_grid_attach(GTK_GRID(listbox1),add_label2,0,0,1,1);
  row3 = gtk_list_box_row_new ();

  //box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
  //g_object_set (box, "margin-start", 10, "margin-end", 10, NULL);
  //gtk_container_add (GTK_CONTAINER (row), box);
  //gtk_container_add_with_properties (GTK_CONTAINER (row), label, "expand", TRUE, NULL);
  gtk_container_add (GTK_CONTAINER (row3), add_label2);
	gtk_list_box_insert (GTK_LIST_BOX (listbox1), row3, -1 );

	gtk_container_add(GTK_CONTAINER(window),box);
    /* pack the table into the scrolled window */
	gtk_widget_show_all(box);
	gtk_widget_show_all(listbox1);
	gtk_widget_show_all(scrolled_window);
	//gtk_widget_show(listbox1);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}

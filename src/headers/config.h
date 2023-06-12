#ifndef CONFIG_H
#define CONFIG_H
#include <gtk/gtk.h>

enum exit_codes {SUCCESS,FAILURE,UNFINISHED,FINISHED};
enum status {NONE, START, FINISH};
enum columns {DATE_C, PERSON_C, PAYMENT_METHOD_C, AMOUNT_C, YEAR_C, MONTH_C, DAY_C, PAID_C, DATE_SORT_C, SHOW_C, TOTAL_COLUMNS};
#define DECIMAL_PLACES 2 /* how many decimal places to the right of the decimal we will parse */ /* should be adjustable */
#define MAX_ENTRIES 100000 /* unused */
#define MAX_DATE_CHARS 21 /* adjustable, includes '\0' */
#define MAX_METHOD_CHARS 21 /* adjustable, includes '\0' */
#define MAX_PERSON_CHARS 101 /* adjustable, includes '\0' */
#define MAX_AMOUNT_CHARS 34 + DECIMAL_PLACES + 1 /* Should always be larger than amount of digits in UINT64_MAX */
#define MAX_PAID_CHARS 7 /* should always be 4 or more */
#define FILENAME_SIZE 250 /* adjustable */
#define MAX_YEAR_CHARS 6 /* cannot exceed number of digits of MAX_YEAR_NUMBER + 1 or '\0' */
#define MAX_DAY_CHARS 4 /* cannot exceed number of digits of MAX_DAY_NUMBER + 1 for '\0' */
#define MAX_MONTH_CHARS 4 /* cannot exceed number of digits of MAX_MONTH_NUMBER + 1 for '\0' */
#define MAX_DAY_NUMBER 99 /* cannot exceed 99, MAX_DAY_CHARS is number of digits of this + 1 for '\0' */
#define MAX_MONTH_NUMBER 99 /* cannot exceed 99, MAX_MONTH_CHARS is number of digits of this + 1 for '\0' */
#define MAX_YEAR_NUMBER 65534 /* cannot exceed USHORT_MAX, MAX_YEAR_CHARS is number of digits of this + 1 for '\0' */


struct treeview_models {
	GtkTreeModel *sorted_model;
	GtkTreeModel *filtered_model;
	GtkListStore *liststore;
};

struct windows_list {
	GtkWidget *main_window;
	GtkWidget *filter_window;
};

extern GtkWidget *amount_entry, *date_entry, *person_entry, *method_entry, *paid_check_button, *error_widget, *scrolled_window, *total_filtered_results_label, *total_results_label, *window;
extern GtkTextBuffer *error_buffer;
extern GtkWidget *error_widget;
extern unsigned long long filtered_amount_total;
extern unsigned long long amount_total;
extern char filename[FILENAME_SIZE];
extern unsigned char is_scrolling;
extern struct windows_list windows;

#endif

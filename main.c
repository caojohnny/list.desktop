#include <cjson/cJSON.h>
#include <gtk/gtk.h>
#include <string.h>

GtkEntry *text_entry;
GtkListBox *item_list;

void init() {
    gtk_entry_grab_focus_without_selecting(text_entry);

    // TODO: Load JSON
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_resource(builder, "/com/gmail/woodyc40/list-desktop/template.ui", NULL);
    gtk_builder_connect_signals(builder, NULL);

    text_entry = GTK_ENTRY(gtk_builder_get_object(builder, "text_entry"));
    item_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "item_list"));

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_object_unref(builder);

    gtk_widget_show(window);
    init();

    gtk_main();

    return 0;
}

void quit() {
    gtk_main_quit();

    // TODO: Save to JSON
}

void box_clicked_m3(GtkWidget *event_box, GdkEventButton *ev, gpointer user_data) {
    if (ev->button != 2) {
        return;
    }

    GtkWidget *list_box_row_widget = gtk_widget_get_parent(event_box);
    gtk_widget_destroy(list_box_row_widget);
}

void add_button_clicked() {
    const gchar *text_entry_string = gtk_entry_get_text(text_entry);
    if (strlen(text_entry_string) == 0) {
        gtk_entry_grab_focus_without_selecting(text_entry);
        return;
    }

    GtkWidget *event_box_widget = gtk_event_box_new();
    GtkContainer *event_box_container = GTK_CONTAINER(event_box_widget);
    gtk_widget_set_events(event_box_widget, GDK_BUTTON3_MASK);
    g_signal_connect(event_box_widget, "button-press-event", G_CALLBACK(box_clicked_m3), NULL);

    GtkWidget *box_widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkBox *box = GTK_BOX(box_widget);
    GtkContainer *box_container = GTK_CONTAINER(box_widget);

    GtkWidget *label_widget = gtk_label_new(text_entry_string);
    GtkLabel *label = GTK_LABEL(label_widget);
    gtk_label_set_xalign(label, 0);

    GtkWidget *check_button_widget = gtk_check_button_new();

    gtk_box_pack_start(box, label_widget, 1, 1, 0);
    gtk_container_add(box_container, check_button_widget);
    gtk_container_add(event_box_container, box_widget);

    gtk_widget_show(label_widget);
    gtk_widget_show(check_button_widget);
    gtk_widget_show(box_widget);
    gtk_widget_show(event_box_widget);

    gtk_list_box_insert(item_list, event_box_widget, -1);

    gtk_entry_set_text(text_entry, "");
    gtk_entry_grab_focus_without_selecting(text_entry);
}

gboolean enter_pressed(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    gboolean text_entry_focused = gtk_widget_has_focus(GTK_WIDGET(text_entry));
    if (!text_entry_focused) {
        return FALSE;
    }

    GdkEventKey *event_key = (GdkEventKey *) event;
    if (event_key->keyval != GDK_KEY_Return) {
        return FALSE;
    }

    add_button_clicked();
    return FALSE;
}

#include <gtk/gtk.h>

GtkEntry *text_entry;
GtkBox *scroll_window_box;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_resource(builder, "/com/gmail/woodyc40/list-desktop/template.ui", NULL);
    gtk_builder_connect_signals(builder, NULL);

    text_entry = GTK_ENTRY(gtk_builder_get_object(builder, "text_entry"));
    scroll_window_box = GTK_BOX(gtk_builder_get_object(builder, "scroll_window_box"));

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

void quit() {
    gtk_main_quit();

    // TODO: Save entries as JSON
}

void add_button_clicked(GtkButton *button, gpointer user_data) {
    gchar *text_entry_string = gtk_entry_get_text(text_entry);

    // TODO: Add entry to scroll box
}

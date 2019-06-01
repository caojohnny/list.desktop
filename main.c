#include <cjson/cJSON.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

typedef struct id_data {
    int id;
} id_data;

int id_counter = 0;
GtkEntry *text_entry;
GtkListBox *item_list;
cJSON *data;

void init();

int add_list_line(const char *, const int, const int);

void box_clicked_m3(GtkWidget *, GdkEventButton *, gpointer);

void check_clicked(GtkWidget *, gpointer);

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

FILE *get_data_file(const char *flags) {
    char *path = getcwd(NULL, PATH_MAX);
    if (!path) {
        puts("Error finding CWD");
        return NULL;
    }

    strcat(path, "/list.desktop/");

    struct stat st;
    if (stat(path, &st) != 0) {
        if (mkdir(path, S_IRWXU) != 0) {
            printf("Error performing mkdir for %s\n", path);
            return NULL;
        }
    }

    strcat(path, "data.json");
    return fopen(path, flags);
}

void init() {
    gtk_entry_grab_focus_without_selecting(text_entry);

    FILE *file = get_data_file("r");
    if (file == NULL) {
        data = cJSON_CreateObject();
        cJSON_AddArrayToObject(data, "entries");
        cJSON_AddNumberToObject(data, "id_counter", 0);

        return;
    }

    int content_len = 0;
    char *contents = NULL;

    static const int BUF_LEN = 8192;
    char *buffer = malloc(BUF_LEN);
    while (1) {
        size_t read_size = fread(buffer, sizeof(*contents), BUF_LEN, file);
        int begin_idx = content_len;
        content_len += read_size + 1;

        char *new_contents = realloc(contents, content_len * sizeof(*contents));
        if (new_contents == NULL) {
            puts("Error allocating more space for the config contents");
            fclose(file);
            return;
        }

        contents = new_contents;

        if (!memcpy(contents + begin_idx, buffer, read_size * sizeof(*contents))) {
            puts("Error copying contents of the buffer to the file contents");
            fclose(file);
            return;
        }

        if (read_size != BUF_LEN) {
            break;
        }
    }

    fclose(file);

    *(contents + content_len - 1) = '\0';
    cJSON *root = cJSON_Parse(contents);
    free(buffer);
    free(contents);

    if (root == NULL) {
        puts("Error reading from the data file");
        return;
    }

    id_counter = cJSON_GetObjectItem(root, "id_counter")->valueint;

    cJSON *entries_array = cJSON_GetObjectItem(root, "entries");
    int entries_len = cJSON_GetArraySize(entries_array);
    for (int i = 0; i < entries_len; i++) {
        cJSON *entry = cJSON_GetArrayItem(entries_array, i);

        cJSON *text_item = cJSON_GetObjectItem(entry, "text");
        char *text = text_item->valuestring;

        cJSON *checked_item = cJSON_GetObjectItem(entry, "checked");
        int checked = checked_item->valueint;

        cJSON *id_item = cJSON_GetObjectItem(entry, "id");
        int id = id_item->valueint;

        add_list_line(text, checked, id);
    }

    data = root;
}

void quit() {
    gtk_main_quit();

    if (data == NULL) {
        return;
    }

    cJSON *counter_item = cJSON_CreateNumber(id_counter);
    cJSON_ReplaceItemInObject(data, "id_counter", counter_item);

    char *string = cJSON_Print(data);
    cJSON_free(data);

    FILE *file = get_data_file("w");
    if (file == NULL) {
        puts("Failed to open data file");
        return;
    }

    fwrite(string, sizeof(string), strlen(string), file);
    fclose(file);
}


int add_list_line(const char *string, const int checked, int id) {
    GtkWidget *event_box_widget = gtk_event_box_new();
    GtkContainer *event_box_container = GTK_CONTAINER(event_box_widget);
    gtk_widget_set_events(event_box_widget, GDK_BUTTON3_MASK);
    g_signal_connect(event_box_widget, "button-press-event", G_CALLBACK(box_clicked_m3), NULL);

    GtkWidget *box_widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GObject *box_obj = G_OBJECT(box_widget);
    GtkBox *box = GTK_BOX(box_widget);
    GtkContainer *box_container = GTK_CONTAINER(box_widget);
    id_data *id_data_value = malloc(sizeof(id_data));
    id_data_value->id = id;
    g_object_set_data(box_obj, "id_data", id_data_value);

    GtkWidget *label_widget = gtk_label_new(string);
    GtkLabel *label = GTK_LABEL(label_widget);
    gtk_label_set_xalign(label, 0);

    GtkWidget *check_button_widget = gtk_check_button_new();
    if (checked) {
        GtkToggleButton *toggle_button = GTK_TOGGLE_BUTTON(check_button_widget);
        gtk_toggle_button_set_active(toggle_button, 1);
    }
    g_signal_connect(check_button_widget, "toggled", G_CALLBACK(check_clicked), NULL);

    gtk_box_pack_start(box, label_widget, 1, 1, 0);
    gtk_container_add(box_container, check_button_widget);
    gtk_container_add(event_box_container, box_widget);

    gtk_widget_show(label_widget);
    gtk_widget_show(check_button_widget);
    gtk_widget_show(box_widget);
    gtk_widget_show(event_box_widget);

    gtk_list_box_insert(item_list, event_box_widget, -1);
    return id_data_value->id;
}

void box_clicked_m3(GtkWidget *event_box, GdkEventButton *ev, gpointer user_data) {
    if (ev->button != 2) {
        return;
    }

    int found = 0;
    GtkContainer *list_box_row_container = GTK_CONTAINER(event_box);
    GList *event_box_child = gtk_container_get_children(list_box_row_container);
    for (; event_box_child != NULL; event_box_child = event_box_child->next) {
        if (!GTK_IS_BOX(event_box_child->data)) {
            continue;
        }

        GtkContainer *box_container = GTK_CONTAINER(event_box_child->data);
        GObject *box_obj = G_OBJECT(box_container);
        id_data *id_data_value = g_object_get_data(box_obj, "id_data");
        cJSON *entries = cJSON_GetObjectItem(data, "entries");
        int entries_len = cJSON_GetArraySize(entries);
        for (int i = 0; i < entries_len; i++) {
            cJSON *item = cJSON_GetArrayItem(entries, i);
            cJSON *id_item = cJSON_GetObjectItem(item, "id");
            if (id_data_value->id == id_item->valueint) {
                cJSON_DeleteItemFromArray(entries, i);
                found = 1;
                break;
            }
        }

        if (found) {
            break;
        }
    }

    GtkWidget *list_box_row_widget = gtk_widget_get_parent(event_box);
    gtk_widget_destroy(list_box_row_widget);
}

void check_clicked(GtkWidget *widget, gpointer user_data) {
    GtkContainer *box_container = GTK_CONTAINER(gtk_widget_get_parent(widget));
    GObject *box_obj = G_OBJECT(box_container);
    id_data *id_data_value = g_object_get_data(box_obj, "id_data");

    cJSON *entries = cJSON_GetObjectItem(data, "entries");
    int entries_len = cJSON_GetArraySize(entries);
    for (int i = 0; i < entries_len; i++) {
        cJSON *item = cJSON_GetArrayItem(entries, i);
        cJSON *id_item = cJSON_GetObjectItem(item, "id");
        if (id_item->valueint == id_data_value->id) {
            GtkToggleButton *toggle_button = GTK_TOGGLE_BUTTON(widget);
            gboolean active = gtk_toggle_button_get_active(toggle_button);
            cJSON *checked_item = cJSON_CreateNumber(active);
            cJSON_ReplaceItemInObject(item, "checked", checked_item);
            break;
        }
    }
}

void add_button_clicked() {
    const gchar *text_entry_string = gtk_entry_get_text(text_entry);
    if (strlen(text_entry_string) == 0) {
        gtk_entry_grab_focus_without_selecting(text_entry);
        return;
    }

    int id = add_list_line(text_entry_string, 0, id_counter++);

    cJSON *entries_array = cJSON_GetObjectItem(data, "entries");
    cJSON *entry = cJSON_CreateObject();
    cJSON_AddNumberToObject(entry, "id", id);
    cJSON_AddStringToObject(entry, "text", text_entry_string);
    cJSON_AddNumberToObject(entry, "checked", 0);
    cJSON_AddItemToArray(entries_array, entry);

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

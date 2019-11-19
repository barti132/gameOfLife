#include <gtk-3.0/gtk/gtk.h>
#include <stdio.h>

const gchar* path;
double cycle = 0.2;

static void checkConditions(GtkWidget *widget, GtkWidget *button){
    if(path != NULL && cycle != 0){
        g_print("path: %s, time: %f", path, cycle);
    }
}

static void getPath_callback(GtkWidget *widget, GtkFileChooser *chooser){
    path = gtk_file_chooser_get_filename(widget);
}

static void on_changed(GtkComboBox *widget, gpointer   user_data){
    GtkComboBox *combo_box = widget;

    if (gtk_combo_box_get_active (combo_box) != 0) {
        cycle = atof(gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(combo_box)));
    }
}

static void update_preview_cb(GtkFileChooser *file_chooser, gpointer data){
    GtkWidget *preview;
    char *filename;
    GdkPixbuf *pixbuf;
    gboolean have_preview;

    preview = GTK_WIDGET (data);
    filename = gtk_file_chooser_get_preview_filename (file_chooser);

    pixbuf = gdk_pixbuf_new_from_file_at_size (filename, 128, 128, NULL);
    have_preview = (pixbuf != NULL);
    g_free (filename);

    gtk_image_set_from_pixbuf (GTK_IMAGE (preview), pixbuf);
    if (pixbuf)
        g_object_unref (pixbuf);

    gtk_file_chooser_set_preview_widget_active (file_chooser, have_preview);
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data){
    guint width, height;
    GdkRGBA color;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (widget);

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    gtk_render_background (context, cr, 0, 0, width, height);
    cairo_arc (cr, width / 2.0, height / 2.0, MIN (width, height) / 2.0, 0, 2 * G_PI);
    gtk_style_context_get_color (context, gtk_style_context_get_state (context), &color);
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_fill (cr);

    return FALSE;
}

static void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window;
    GtkWidget *vbox, *hbox1;
    GtkWidget *choosebutton;
    GtkWidget *button1;
    GtkWidget *combo_box;
    GtkWidget *drawing_area;


    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Game of life");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);


    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);

    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);

    choosebutton = gtk_file_chooser_button_new (("Select a file"), GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (choosebutton), "/home/barti/CLionProjects");
    g_signal_connect(choosebutton, "selection-changed", G_CALLBACK(getPath_callback), NULL);
    gtk_box_pack_start(GTK_BOX(hbox1), choosebutton, FALSE, FALSE, 5);

    combo_box = gtk_combo_box_text_new ();
    const char *time[] = {"0.2", "0.5", "1", "1.5", "2"};
    for (int i = 0; i < G_N_ELEMENTS (time); i++){
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), time[i]);
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
    g_signal_connect (combo_box, "changed", G_CALLBACK (on_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox1), combo_box, FALSE, FALSE, 5);

    button1 = gtk_button_new_with_label("Load settings and start");
    g_signal_connect(button1, "clicked", G_CALLBACK(checkConditions), NULL);
    gtk_box_pack_start(GTK_BOX(hbox1), button1, FALSE, FALSE, 5);

    drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (drawing_area, 700, 600);
    g_signal_connect (G_OBJECT (drawing_area), "draw", G_CALLBACK (draw_callback), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, FALSE, FALSE, 5);

    gtk_widget_show_all (window);
}

int main (int argc, char **argv){
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
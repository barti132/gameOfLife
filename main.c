#include <gtk-3.0/gtk/gtk.h>
#include <stdio.h>

const gchar* path;
double cycle = 0.2;
int width = 120, height = 80;
gboolean status = FALSE;
char** map = NULL;
GtkWidget *drawingArea;

void checkConditions(GtkButton *);
void getPath_callback(GtkWidget*, GtkFileChooser*);
void on_changed(GtkComboBox*, gpointer);
gboolean on_draw_event(GtkWidget*, cairo_t*, gpointer);
void activate(GtkApplication*, gpointer);
void readMap(FILE*);
void freeMap(char**);
char** initMap();
int countNeighbours(int, int);

char** initMap(){
    char** map = NULL;
    if(map == NULL){
        map = malloc(sizeof(char*) * width);
        for(int i = 0; i < width; i++){
            map[i] = malloc(sizeof(char) * height);
        }
    }

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            map[i][j] = '.';
        }
    }
    return map;
}

void readMap(FILE* file){
    fseek (file, 0, SEEK_END);
    int size = ftell (file);
    fseek (file, 0, SEEK_SET);

    char text[255];
    fgets(text, 255, file);

    int fileWidth = strlen(text);
    int fileHeight = size / fileWidth;

    fseek (file, 0, SEEK_SET);
    int yoffset = (height - fileHeight) / 2;
    int xoffset = (width - fileWidth) / 2;

    while(fgets(text, 255, file) != NULL) {
        for (int i = 0; i < fileWidth - 1; i++) {
            map[yoffset][xoffset + i] = text[i];
        }
        yoffset++;
    }
}

int countNeighbours(int x,int y){
    int neighbours = 0;

    for(int i = x - 1; i <= x + 1; i++){
        for(int j = y - 1; j <= y + 1; j++){
            if(i < 0 || j < 0 || i >= width || j >= height || (i == x && j == y)){
                continue;
            }

            if(map[j][i] == '#'){
                neighbours++;
            }
        }
    }

    return neighbours;
}

char** nextMap(){
    char** newMap = initMap();

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            int tmp = countNeighbours(i, j);

            if(tmp == 3 && map[j][i] == '.') {
                newMap[j][i] = '#';
            }
            else if(tmp == 2 || tmp == 3 && map[j][i] == '#'){
                newMap[j][i] = '#';
            }
        }
    }

    return newMap;
}

gboolean simulation(gpointer button){
    if(status) {
        gtk_widget_queue_draw_area(drawingArea, 0, 0, gtk_widget_get_allocated_width(drawingArea),
                                   gtk_widget_get_allocated_height(drawingArea));

        char** newMap = nextMap();
        if(newMap != NULL){
            free(map);
            map = newMap;
        }

        return TRUE;
    }
    return FALSE;
}

void checkConditions(GtkButton *button){
    if(path != NULL && cycle != 0 && !status){
        FILE *file = fopen(path, "r");
        if(!file==NULL){
            map = initMap();
            readMap(file);
            status = TRUE;
            g_timeout_add (cycle * 1000, simulation, (gpointer *) button);
        }
        else{
            g_print("Error: reading new file");
        }

    }
    else{
        status = FALSE;
    }
}

void freeMap(char** map){
    if(map != NULL) {
        for (int i = 0; i < width; i++) {
            free(map[i]);
        }
        free(map);
    }
}

void getPath_callback(GtkWidget *widget, GtkFileChooser *chooser){
    path = gtk_file_chooser_get_filename(widget);
}

void on_changed(GtkComboBox *widget, gpointer user_data){
    GtkComboBox *combo_box = widget;

    if (gtk_combo_box_get_active (combo_box) != 0) {
        cycle = atof(gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(combo_box)));
    }
}

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data){
    GtkWidget *win = gtk_widget_get_toplevel(widget);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
    if(map != NULL) {
        for (int i = 0, x = 0; i < width; i++, x += 8) {
            for (int j = 0, y = 0; j < height; j++, y += 8) {
                if (map[i][j] == '#') {
                    cairo_set_line_width(cr, 1);
                    cairo_set_source_rgb(cr, 1.f, 0.f, 0.f);

                    cairo_rectangle(cr, y, x, 8, 8);
                    cairo_stroke_preserve(cr);
                    cairo_set_source_rgb(cr, 0.f, 1.f, 0.f);
                    cairo_fill(cr);
                }
            }
        }
    }
    return FALSE;
}

void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window;
    GtkWidget *vbox, *hbox1;
    GtkWidget *chooseButton;
    GtkWidget *startButton;
    GtkWidget *comboBox;
    GtkWidget *label;


    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Game of life");
    gtk_window_set_default_size (GTK_WINDOW (window), 960, 700);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);

    chooseButton = gtk_file_chooser_button_new (("Select a file"), GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooseButton), "/home/barti/CLionProjects");
    g_signal_connect(chooseButton, "selection-changed", G_CALLBACK(getPath_callback), NULL);
    gtk_box_pack_start(GTK_BOX(hbox1), chooseButton, FALSE, FALSE, 5);

    comboBox = gtk_combo_box_text_new ();
    const char *time[] = {"0,1", "0,2", "0,5", "1", "1,5", "2"};
    for (int i = 0; i < G_N_ELEMENTS (time); i++){
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (comboBox), time[i]);
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (comboBox), 0);
    g_signal_connect (comboBox, "changed", G_CALLBACK (on_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox1), comboBox, FALSE, FALSE, 5);

    startButton = gtk_button_new_with_label("Start");
    g_signal_connect(startButton, "clicked", G_CALLBACK(checkConditions), NULL);
    gtk_box_pack_start(GTK_BOX(hbox1), startButton, FALSE, FALSE, 5);

    label = gtk_label_new("Number of gen: 0");
    gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, FALSE, 5);


    drawingArea = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(vbox), drawingArea, TRUE, TRUE, 5);
    g_signal_connect(drawingArea, "draw", G_CALLBACK(on_draw_event), NULL);

    gtk_widget_show_all (window);
}

int main (int argc, char **argv){
    GtkApplication *app;
    int status;

    app = gtk_application_new ("pl.barti132", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    freeMap(map);
    return status;
}
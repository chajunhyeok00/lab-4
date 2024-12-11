#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

// 계산기 상태
char display[256] = "";
double current_value = 0;
char operator = 0;
int reset_display = 0;

// 디스플레이 업데이트
void update_display(GtkWidget *label) {
    gtk_label_set_text(GTK_LABEL(label), display);
}

// 숫자 버튼 클릭 이벤트
void on_number_clicked(GtkWidget *widget, gpointer label) {
    const char *number = gtk_button_get_label(GTK_BUTTON(widget));

    if (reset_display) {
        strcpy(display, "");
        reset_display = 0;
    }

    strcat(display, number);
    update_display(GTK_WIDGET(label));
}

// 연산 버튼 클릭 이벤트
void on_operator_clicked(GtkWidget *widget, gpointer label) {
    const char *op = gtk_button_get_label(GTK_BUTTON(widget));

    if (strlen(display) > 0) {
        current_value = atof(display);
        operator = op[0];
        reset_display = 1;
    }
}

// 결과 버튼 클릭 이벤트
void on_equal_clicked(GtkWidget *widget, gpointer label) {
    if (operator && strlen(display) > 0) {
        double second_value = atof(display);

        switch (operator) {
            case '+': current_value += second_value; break;
            case '-': current_value -= second_value; break;
            case '*': current_value *= second_value; break;
            case '/': current_value /= second_value; break;
        }

        snprintf(display, sizeof(display), "%g", current_value);
        operator = 0;
        reset_display = 1;
    }

    update_display(GTK_WIDGET(label));
}

// 초기화 버튼 클릭 이벤트
void on_clear_clicked(GtkWidget *widget, gpointer label) {
    strcpy(display, "");
    current_value = 0;
    operator = 0;
    reset_display = 0;
    update_display(GTK_WIDGET(label));
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *grid, *label;
    GtkWidget *buttons[16];
    const char *labels[16] = {"7", "8", "9", "+", "4", "5", "6", "-", "1", "2", "3", "*", "C", "0", "=", "/"};

    gtk_init(&argc, &argv);

    // 창 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 디스플레이 레이블
    label = gtk_label_new("0");
    gtk_widget_set_halign(label, GTK_ALIGN_END);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

    // 그리드 생성
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 디스플레이 추가
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 4, 1);

    // 버튼 추가
    for (int i = 0; i < 16; i++) {
        buttons[i] = gtk_button_new_with_label(labels[i]);
        int row = (i / 4) + 1;
        int col = i % 4;

        gtk_grid_attach(GTK_GRID(grid), buttons[i], col, row, 1, 1);

        if (i == 12) { // Clear
            g_signal_connect(buttons[i], "clicked", G_CALLBACK(on_clear_clicked), label);
        } else if (i == 14) { // Equal
            g_signal_connect(buttons[i], "clicked", G_CALLBACK(on_equal_clicked), label);
        } else if (i % 4 == 3) { // Operators
            g_signal_connect(buttons[i], "clicked", G_CALLBACK(on_operator_clicked), label);
        } else { // Numbers
            g_signal_connect(buttons[i], "clicked", G_CALLBACK(on_number_clicked), label);
        }
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

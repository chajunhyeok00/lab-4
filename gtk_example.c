#include <gtk/gtk.h>

// 버튼 클릭 시 호출되는 콜백 함수
void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Hello, GTK!\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;   // 창
    GtkWidget *button;   // 버튼

    gtk_init(&argc, &argv); // GTK 초기화

    // 창 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // 버튼 생성
    button = gtk_button_new_with_label("Click Me!");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // 창에 버튼 추가
    gtk_container_add(GTK_CONTAINER(window), button);

    // 창 닫기 이벤트 처리
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window); // 모든 위젯 표시
    gtk_main(); // GTK 이벤트 루프 실행

    return 0;
}

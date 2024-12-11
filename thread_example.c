#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 쓰레드에서 실행할 함수
void* print_numbers(void* arg) {
    char* thread_name = (char*)arg;
    for (int i = 1; i <= 5; i++) {
        printf("%s: %d\n", thread_name, i);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // 쓰레드 생성
    pthread_create(&thread1, NULL, print_numbers, "Thread-1");
    pthread_create(&thread2, NULL, print_numbers, "Thread-2");

    // 쓰레드 종료 대기
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("모든 쓰레드 작업이 완료되었습니다.\n");
    return 0;
}

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_CLIENTS 3  // 클라이언트 쓰레드 수
#define BUFFER_SIZE 256  // 메시지 버퍼 크기

// 데이터 공유를 위한 전역 변수
char message[BUFFER_SIZE];
int message_ready = 0;  // 메시지가 준비되었는지 여부

pthread_mutex_t mutex;
pthread_cond_t cond_var;
pthread_t clients[NUM_CLIENTS];  // 클라이언트 쓰레드 배열

// 서버 쓰레드 함수
void* server_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 메시지가 준비될 때까지 대기
        while (!message_ready) {
            pthread_cond_wait(&cond_var, &mutex);
        }

        // 메시지를 방송
        printf("Server broadcasting message: %s\n", message);

        // 모든 클라이언트에게 메시지를 방송한 후 상태 초기화
        message_ready = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 클라이언트 쓰레드 함수
void* client_thread(void* arg) {
    int client_id = *((int*)arg);
    char input_message[BUFFER_SIZE];

    while (1) {
        // 각 클라이언트는 사용자 입력을 받아 메시지를 전송
        printf("Client %d: Enter a message to send: ", client_id);
        fgets(input_message, BUFFER_SIZE, stdin);
        input_message[strcspn(input_message, "\n")] = 0;  // 개행 문자 제거

        pthread_mutex_lock(&mutex);
        strcpy(message, input_message);
        message_ready = 1;

        // 서버 스레드에 메시지가 준비되었음을 알리기
        pthread_cond_signal(&cond_var);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t server;
    int client_ids[NUM_CLIENTS];

    // 뮤텍스 및 조건 변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < NUM_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, (void*)&client_ids[i]);
    }

    // 서버 쓰레드와 클라이언트 쓰레드가 종료되지 않도록 대기
    pthread_join(server, NULL);
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    // 뮤텍스 및 조건 변수 자원 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    return 0;
}

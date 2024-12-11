#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5  // 버퍼의 크기
#define NUM_PRODUCERS 2  // 생산자 스레드 수
#define NUM_CONSUMERS 2  // 소비자 스레드 수

int buffer[BUFFER_SIZE];
int in = 0;  // 생산자 포인터
int out = 0; // 소비자 포인터

pthread_mutex_t mutex;  // 뮤텍스
pthread_cond_t not_full;  // 버퍼가 가득 차지 않은 경우
pthread_cond_t not_empty; // 버퍼가 비어있지 않은 경우

void* producer(void* arg) {
    int item;
    while (1) {
        item = rand() % 100;  // 생산할 데이터 생성
        pthread_mutex_lock(&mutex);  // 뮤텍스 잠금

        // 버퍼가 가득 차면 대기
        while ((in + 1) % BUFFER_SIZE == out) {
            pthread_cond_wait(&not_full, &mutex);
        }

        buffer[in] = item;
        printf("Producer %ld produced %d at index %d\n", (long)arg, item, in);
        in = (in + 1) % BUFFER_SIZE;  // 생산자 포인터 이동

        pthread_cond_signal(&not_empty);  // 소비자 스레드 깨우기
        pthread_mutex_unlock(&mutex);  // 뮤텍스 해제

        sleep(1);  // 생산자 스레드가 데이터를 생산하는 시간 지연
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    while (1) {
        pthread_mutex_lock(&mutex);  // 뮤텍스 잠금

        // 버퍼가 비어있으면 대기
        while (in == out) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        item = buffer[out];
        printf("Consumer %ld consumed %d from index %d\n", (long)arg, item, out);
        out = (out + 1) % BUFFER_SIZE;  // 소비자 포인터 이동

        pthread_cond_signal(&not_full);  // 생산자 스레드 깨우기
        pthread_mutex_unlock(&mutex);  // 뮤텍스 해제

        sleep(1);  // 소비자 스레드가 데이터를 소비하는 시간 지연
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    // 뮤텍스 및 조건 변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    // 생산자 스레드 생성
    for (long i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, (void*)i);
    }

    // 소비자 스레드 생성
    for (long i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void*)i);
    }

    // 스레드가 종료될 때까지 대기
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // 뮤텍스 및 조건 변수 자원 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}

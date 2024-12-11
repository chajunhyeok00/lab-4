#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            printf("Client disconnected\n");
        } else {
            perror("recv");
        }
        close(client_sock);
        return;
    }

    buffer[bytes_received] = '\0'; // 문자열 끝에 널 문자 추가
    printf("Received: %s\n", buffer);

    // 모든 클라이언트에게 메시지를 방송
    // 추후 구현: 모든 클라이언트 소켓을 저장하고 이를 통해 메시지를 전송
}

int main() {
    int server_sock, client_sock, epoll_fd, num_events;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct epoll_event ev, events[MAX_EVENTS];

    // 서버 소켓 생성
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 소켓 바인딩
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 클라이언트의 연결을 수락할 준비
    if (listen(server_sock, 5) < 0) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // epoll 생성
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 서버 소켓을 epoll 인스턴스에 등록
    ev.events = EPOLLIN;
    ev.data.fd = server_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &ev) < 0) {
        perror("epoll_ctl");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0) {
            perror("epoll_wait");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_sock) {
                // 새로운 클라이언트 연결 처리
                client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
                if (client_sock < 0) {
                    perror("accept");
                    continue;
                }

                printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

                // 클라이언트 소켓을 epoll에 등록
                ev.events = EPOLLIN | EPOLLET; // EPOLLET는 Edge Triggered 모드
                ev.data.fd = client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &ev) < 0) {
                    perror("epoll_ctl");
                    close(client_sock);
                    continue;
                }
            } else {
                // 클라이언트 소켓에서 데이터 처리
                handle_client(events[i].data.fd);
            }
        }
    }

    close(server_sock);
    close(epoll_fd);
    return 0;
}

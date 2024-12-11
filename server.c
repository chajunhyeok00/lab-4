#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 소켓 생성
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Server socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 소켓 바인딩
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 클라이언트의 연결 요청 대기
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // 클라이언트 연결 수락
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("Server accept failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");

    // 클라이언트로부터 메시지 수신
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Received from client: %s\n", buffer);

        // 클라이언트에 응답 전송
        send(client_sock, buffer, bytes_received, 0);
    }

    close(client_sock);
    close(server_sock);
    return 0;
}

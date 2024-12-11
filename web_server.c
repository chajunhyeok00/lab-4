#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void handle_request(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // 클라이언트로부터 데이터 받기
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_sock);
        return;
    }

    buffer[bytes_received] = '\0';
    printf("Received request:\n%s\n", buffer);

    // 요청 메소드 확인
    if (strncmp(buffer, "GET", 3) == 0) {
        // GET 메소드 처리
        send(client_sock, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 41, 0);
        send(client_sock, "<html><body><h1>GET request received</h1></body></html>", 49, 0);
    } else if (strncmp(buffer, "POST", 4) == 0) {
        // POST 메소드 처리
        send(client_sock, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 41, 0);
        send(client_sock, "<html><body><h1>POST request received</h1></body></html>", 50, 0);
    } else {
        // 지원하지 않는 메소드
        send(client_sock, "HTTP/1.1 405 Method Not Allowed\r\n\r\n", 31, 0);
    }

    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

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

    printf("Server is listening on port %d\n", PORT);

    // 클라이언트 요청 처리
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
        handle_request(client_sock);
    }

    close(server_sock);
    return 0;
}


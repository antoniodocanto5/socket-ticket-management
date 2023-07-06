#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int client = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(5000)
    };

    if (inet_pton(AF_INET, "127.0.0.1", &(saddr.sin_addr)) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    if (connect(client, (struct sockaddr *)&saddr, sizeof saddr) < 0) {
        perror("Connection failed");
        return 1;
    }

    char command[150];
    char response[256];

    while (1) {
        printf("Enter a command ('display' or 'purchase <ticket_id> <quantity>'): \n");
        fgets(command, sizeof(command), stdin);

        send(client, command, strlen(command), 0);

        int bytes_received = recv(client, response, sizeof(response), 0);
        if (bytes_received <= 0) {
            break;
        }

        response[bytes_received] = '\0';
        printf("Server response: %s", response);
    }

    close(client);

    return 0;
}

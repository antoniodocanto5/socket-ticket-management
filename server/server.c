#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_TICKETS 10

struct MovieTicket {
    int id;
    char title[100];
    int quantity;
};

struct MovieTicket tickets[MAX_TICKETS];

void initializeTickets() {
    for (int i = 0; i < MAX_TICKETS; i++) {
        tickets[i].id = i + 1;
        sprintf(tickets[i].title, "Movie %d", i + 1);
        tickets[i].quantity = 10;
    }
}

void displayTickets(int client) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Available Tickets:\n");

    for (int i = 0; i < MAX_TICKETS; i++) {
        char ticketInfo[100];
        snprintf(ticketInfo, sizeof(ticketInfo), "Ticket ID: %d\tTitle: %s\tQuantity: %d\n",
                 tickets[i].id, tickets[i].title, tickets[i].quantity);
        strncat(buffer, ticketInfo, sizeof(buffer));
    }

    send(client, buffer, strlen(buffer), 0);
}

void purchaseTicket(int client, int ticketId, int quantity) {
    if (ticketId < 1 || ticketId > MAX_TICKETS) {
        char message[] = "Invalid ticket ID.\n";
        send(client, message, strlen(message), 0);
        return;
    }

    int index = ticketId - 1;
    struct MovieTicket *ticket = &tickets[index];

    if (ticket->quantity == 0) {
        char message[] = "Ticket is sold out.\n";
        send(client, message, strlen(message), 0);
        return;
    }

    if (ticket->quantity < quantity) {
        char message[] = "Insufficient quantity available.\n";
        send(client, message, strlen(message), 0);
        return;
    }

    ticket->quantity -= quantity;

    char message[256];
    snprintf(message, sizeof(message), "Successfully purchased %d ticket(s) for %s.\n",
             quantity, ticket->title);
    send(client, message, strlen(message), 0);
}

int main() {
    initializeTickets();

    struct sockaddr_in caddr;
    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(5000)
    };
    int server = socket(AF_INET, SOCK_STREAM, 0);

    bind(server, (struct sockaddr *)&saddr, sizeof saddr);

    listen(server, 5);

    int csize = sizeof caddr;
    int client;
    char buff[150];

    while (1) {
        client = accept(server, (struct sockaddr *)&caddr, &csize);

        while (1) {
            int bytes_received = recv(client, buff, sizeof buff, 0);
            if (bytes_received <= 0) {
                break;
            }

            buff[bytes_received] = '\0';

            if (strcmp(buff, "display\n") == 0) {
                displayTickets(client);
            } else if (strncmp(buff, "purchase", 8) == 0) {
                int ticketId, quantity;
                sscanf(buff, "purchase %d %d", &ticketId, &quantity);
                purchaseTicket(client, ticketId, quantity);
            } else {
                char message[] = "Invalid command\n";
                send(client, message, strlen(message), 0);
            }
        }

        close(client);
    }
	 
}



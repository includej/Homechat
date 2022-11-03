/*
C libraries
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Sockets libraries
*/
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
Threads libraries
*/
#include <pthread.h>


/*
Data structures
*/
#include "common.h"

#define SLEEP_TIME 3


char client_id_ascii[100] = { 0 };
bool is_logged_in = false;

/*
Function response handling 
*/

void process_received_message(chat_message_t *response) {
    if (response->type != RESPONSE) {
        printf("Unexpected response\n");
        return;
    }

    switch (response->command) {

        case LOGIN:
            if (response->u.response.status == SUCCESS) {
                is_logged_in = true;
            }
            for (int i = 0; i < response->u.response.num_of_responses; i++) {
                printf("%s\n", response->u.response.response[i]);
            }
            break;
        case LOGOUT:
        case EXIT:
        case REGISTER:
        case SEND:
            for (int i = 0; i < response->u.response.num_of_responses; i++) {
                printf("%s\n", response->u.response.response[i]);
            }

            break;

        case RETRIEVE:
            printf("\n----------Total messages: %d----------\n\n", response->u.response.num_of_responses);

            /*receiving till all the message from user inbox is not fetched */
            for (int i = 0; i < response->u.response.num_of_responses; i=i+2) {
                printf("[%s]:%s\n", response->u.response.response[i], response->u.response.response[i + 1]);
            }

            printf("\n----Press any key to return back to Home----\n%c", getchar());
            getchar();
            break;

        case LIST:
            printf("\n----------Online user available: %d----------\n\n", response->u.response.num_of_responses);
            for (int i = 0; i < response->u.response.num_of_responses; i++) {
                printf("User: %s\n", response->u.response.response[i]);
            }
            printf("\n----Press any key to return back to Home----\n%c", getchar());
            getchar();
            break;


        default:
            printf("Unexpected response\n");
            break;

    }
}


void send_message(int socket_fd, struct sockaddr_in *server_address, chat_message_t *message) {
    ssize_t bytes_sent = sendto(socket_fd, message, sizeof(chat_message_t), 0, (struct sockaddr *) server_address,
                                sizeof(struct sockaddr_in));
    if (bytes_sent < 0) {
        perror("sendto() failed");
        exit(1);
    }
}

void receive_message(int socket_fd, chat_message_t *message) {
    ssize_t bytes_received = recvfrom(socket_fd, message, sizeof(chat_message_t), 0, NULL, NULL);
    if (bytes_received < 0) {
        perror("recvfrom() failed");
        exit(1);
    }
}

/*
Client side user signUp
*/
void signUp(struct User *user) {
    printf("Enter username: ");
    scanf("%s", user->userName);
    printf("Enter password: ");
    scanf("%s", user->password);
}


/*
Client side user login
*/
void login(struct User *user) {
    printf("Enter username: ");
    scanf("%s", user->userName);
    strncpy(client_id_ascii, user->userName, sizeof(client_id_ascii));
    printf("Enter password: ");
    scanf("%s", user->password);
}

void prepareMessage(struct messageSent *msg) {
    printf("Enter username of the receiver: ");
    scanf("%s", msg->to);
    printf("Enter message: ");
    getchar();
    fgets(msg->message, sizeof(msg->message), stdin); //Taking string input of message

}


/*
Client Side Thread
*/
void clienthread(int client_r) {
    /*Initializing socket*/
    int client_request = client_r;
    int sockfd;

    char *ip = "127.0.0.1";

    /*Creating socket*/
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Socket Creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created...\n");

    /*Initializing IP and PORT for connection */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(8989); //port for socket connection

    int choice = 1;

    system("clear");

    /* Loop to deal with user choice*/

    while (1) {
        printf("\n----------Homechat----------\n\n");
        printf("(1) SignUp\n");
        printf("(2) Login\n");
        printf("(3) Exit\n");
        scanf("%d", &choice);

        /*SignUp choice*/
        if (choice == 1) {
            chat_message_t message;
            message.command = REGISTER;
            message.type = COMMAND;
            signUp(&message.u.user);
            send_message(sockfd, &server_address, &message);
            printf("\nSignin In...\n");

            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);
            sleep(SLEEP_TIME);
        }
            /*Login choice*/
        else if (choice == 2) {
            chat_message_t message;
            message.command = LOGIN;
            message.type = COMMAND;

            login(&message.u.user);
            send_message(sockfd, &server_address, &message);

            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);
            sleep(SLEEP_TIME);
            break;
        }
            /*Exit choice*/
        else if (choice == 3) {
            chat_message_t message;
            message.command = LOGOUT;
            message.type = COMMAND;
            printf("\n Goodbye User\n");
            send_message(sockfd, &server_address, &message);
            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);
            exit(1); //Exiting from client program
        } else {
            printf("\nInvalid Option...\n");
            sleep(SLEEP_TIME);;

        }
        system("clear");
    }

    while (choice != 3) {
        system("clear");
        printf("\n----HC----Welcome to HOME----HC----\n");
        printf("(4) Send a private message to User\n");
        printf("(5) Check your Inbox\n");
        printf("(6) Registered User Online in Server\n");
        printf("(7) Exit\n");

        scanf("%d", &choice);

        //Send message choice
        if (choice == 4) {
            chat_message_t message;
            message.command = SEND;
            message.type = COMMAND;
            strncpy(message.client_id, client_id_ascii, sizeof(client_id_ascii));

            prepareMessage(&message.u.message);
            send_message(sockfd, &server_address, &message);

            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);
            sleep(SLEEP_TIME);

        }

            /*Received message choice*/
        else if (choice == 5) {
            chat_message_t message;
            message.command = RETRIEVE;
            message.type = COMMAND;
            strncpy(message.client_id, client_id_ascii, sizeof(client_id_ascii));
            send_message(sockfd, &server_address, &message);

            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);
            sleep(SLEEP_TIME);
        }

            /*Online user choice*/
        else if (choice == 6) {
            chat_message_t message;
            message.command = LIST;
            message.type = COMMAND;
            strncpy(message.client_id, client_id_ascii, sizeof(client_id_ascii));
            send_message(sockfd, &server_address, &message);

            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);
            sleep(SLEEP_TIME);

        }

            /*Exit choice*/
        else if (choice == 7) {
            chat_message_t message;
            message.command = EXIT;
            message.type = COMMAND;
            strncpy(message.client_id, client_id_ascii, sizeof(client_id_ascii));

            send_message(sockfd, &server_address, &message);
            memset(&message, 0, sizeof(chat_message_t));
            receive_message(sockfd, &message);
            process_received_message(&message);

            exit(1); //Exiting from client program

        } else {

        }

    }
}


/*
Main code
*/
int main() {
    int client_request = 1;
    clienthread(client_request);
}



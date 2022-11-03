/* Libraries for C program */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/*Libraries for Sockets*/
#include <sys/socket.h>
#include <arpa/inet.h>

/*Libraries for Threading */
#include <pthread.h>
#include <semaphore.h>

/*Importing data struct*/
#include "common.h"


int CreateServerSocket(int port);

//////////////

pthread_t tid;
pthread_t readerthreads[numberW];
int logIndex = -1;


int onlineUser[numberW]; //array that keep track of online users
int onlineCounter = 0; //counter to keep track of number of online users
struct User users[numberW]; //Structure User for storing UserData
int userCounter = 0; //counter to keep track of number of users

// Send message to client
void send_message(int socket_fd, struct sockaddr_in *client_address, chat_message_t *message) {
    if (sendto(socket_fd,
               message,
               sizeof(*message),
               0,
               (struct sockaddr *) client_address,
               sizeof(*client_address)) < 0) {
        perror("sendto");
        exit(1);
    }
}

/*
Function for response handling 
*/
void send_response_to_client(int socket_fd, struct sockaddr_in *client_address,
                             supported_commands_t command, packet_type_t type, char *data) {
    chat_message_t response;
    memset(&response, 0, sizeof(chat_message_t));
    response.command = command;
    response.type = type;
    switch (command) {
        case REGISTER: {

            response.u.response.num_of_responses = 1;
            strcpy(response.u.response.response[0], data);
            send_message(socket_fd, client_address, &response);

        }
            break;
        case LOGIN: {

            response.u.response.num_of_responses = 1;
            strcpy(response.u.response.response[0], data);
            send_message(socket_fd, client_address, &response);

        }
            break;
        case LOGOUT:

            response.u.response.num_of_responses = 1;
            strcpy(response.u.response.response[0], data);
            send_message(socket_fd, client_address, &response);

            break;
        case SEND:

            response.u.response.num_of_responses = 1;
            strcpy(response.u.response.response[0], data);
            send_message(socket_fd, client_address, &response);

            break;
        case RETRIEVE:

            response.u.response.num_of_responses = users[logIndex].recvCounter * 2;
            for (int i = 0, j = 0; i < users[logIndex].recvCounter*2; i++, j=j+2) {
                strcpy(response.u.response.response[j], users[logIndex].recv[i].from);
                strcpy(response.u.response.response[j + 1], users[logIndex].recv[i].message);
            }
            send_message(socket_fd, client_address, &response);

            break;
        case LIST:

            response.u.response.num_of_responses = onlineCounter;
            for (int i = 0; i < onlineCounter; i++) {
                strcpy(response.u.response.response[i], users[onlineUser[i]].userName);
            }
            send_message(socket_fd, client_address, &response);

            break;
        case EXIT:

            response.u.response.num_of_responses = 1;
            strcpy(response.u.response.response[0], data);
            send_message(socket_fd, client_address, &response);

            break;

        default:
            break;
    }
}


/*
Function to add user to server Database 
*/

void addUserToDB(struct User tmp) {
    FILE *fp = fopen("db.txt", "a");  //append

    if (fp == NULL) {
        printf("Error opening DB File\n");
    }

    strcpy(users[userCounter].userName, tmp.userName);
    printf("Username: %s\n", users[userCounter].userName);
    strcpy(users[userCounter].password, tmp.password);
    printf("Pwd:%s\n", users[userCounter].password);

    //print on DB usr and pwd
    fprintf(fp, "%s %s\n", users[userCounter].userName, users[userCounter].password);
    fclose(fp);

    //initializing usr
    users[userCounter].recvCounter = 0;
    users[userCounter].sentCounter = 0;
    users[userCounter].isLoggedIn = false;
    userCounter++;                          //increment the counter 

}

/*
Function for Reading Database
*/
void readDatabase() {
    FILE *fp = fopen("db.txt", "r"); //read

    if (fp == NULL) {
        printf("Error opening DB file \n");
    }

    printf("Reading database...\n");
    while (fscanf(fp, "%s %s", users[userCounter].userName, users[userCounter].password) != -1) {

        //initialize usr
        users[userCounter].recvCounter = 0;
        users[userCounter].sentCounter = 0;
        userCounter++;

    }
    fclose(fp);

}


/*
Function to verify user in server
*/
int LoginVerify(struct User tmp) {
    for (int i = 0; i < userCounter; i++) {
        if (strcmp(users[i].userName, tmp.userName) == 0 && strcmp(users[i].password, tmp.password) == 0) {
            return i;
        }
    }

    return -1;
}


/*
Function to send message to receiver in server and updates receiver inbox 
*/
int sendMessage(int index, struct messageSent msg) {

    /*Updating user receiver Message*/
    for (int i = 0; i < userCounter; i++) {
        if (strcmp(users[i].userName, msg.to) == 0) {
            //User find updating user recv

            int k = users[i].recvCounter;
            strcpy(users[i].recv[k].from, users[index].userName);
            strcpy(users[i].recv[k].message, msg.message);
            users[i].recvCounter++;

            //Updating user sent

            int j = users[index].sentCounter;
            strcpy(users[index].sent[j].to, msg.to);
            strcpy(users[index].sent[j].message, msg.message);
            users[index].sentCounter++;

            return i;
        }
    }

    return -1;
}

/*
    Server Side Thread to handle each client on server
*/
void reader(int socket, struct sockaddr_in *client_address, chat_message_t param) {
    readDatabase();


    supported_commands_t command = param.command;


    if (command > LOGIN) {
        // user must exist and must be logged in
        for (int i = 0; i < userCounter; i++) {
            if (strcmp(users[i].userName, param.client_id) == 0) {
                if (users[i].isLoggedIn) {
                    logIndex = i;
                    break;
                }
            }
        }
        if (logIndex == -1) {
            send_response_to_client(socket, client_address, command, RESPONSE,
                                    "You must be logged in to perform this action");
            return;
        }
    }

    switch (command) {
        case REGISTER:
            addUserToDB(param.u.user);
            printf("User registered in server succesfully...\n");
            send_response_to_client(socket, client_address, REGISTER, RESPONSE,
                                    "User registered in server succesfully...\n");
            break;
        case LOGIN: {
            //compare inserted user with my data struct users
            int ret = LoginVerify(param.u.user);
            if (ret >= 0) {
                //ret -> index of the current user
                logIndex = ret;
                users[ret].isLoggedIn = true;
                onlineUser[onlineCounter] = logIndex;
                onlineCounter++;

                printf("User credentials verified succesfully.\n");
                send_response_to_client(socket, client_address, LOGIN, RESPONSE,
                                        "User credentials verified succesfully.\n");
                break;

            } else {
                printf("User credentials not verified succesfully\n");
                send_response_to_client(socket, client_address, LOGIN, RESPONSE,
                                        "User credentials not verified succesfully\n");
            }
        }
        case LOGOUT:
            printf("\n Goodbye User\n");
            send_response_to_client(socket, client_address, LOGOUT, RESPONSE, "Goodbye User\n");
            break;

        case SEND:
            //send message to receiver
        {
            int ret = sendMessage(logIndex, param.u.message);
            if (ret >= 0) {
                printf("Message sent succesfully\n");
                send_response_to_client(socket, client_address, SEND, RESPONSE, "Message sent succesfully\n");
            } else {
                printf("Message not sent succesfully\n");
                send_response_to_client(socket, client_address, SEND, RESPONSE, "Message not sent succesfully\n");
            }
        }
            break;

        case RETRIEVE:
            //reterive message from server
            send_response_to_client(socket, client_address, RETRIEVE, RESPONSE, NULL);

            break;

        case LIST:
            //list all users
            printf("Listing all users...\n");
            send_response_to_client(socket, client_address, LIST, RESPONSE, NULL);
            break;

        case EXIT: {
            int flag = -1;
            for (int i = 0; i < onlineCounter; i++) {
                onlineUser[i] = logIndex || flag != -1;
                if (onlineUser[i]) {
                    onlineUser[i] = onlineUser[i + 1];
                    flag = 1;
                }
            }

            onlineCounter--;
            printf("Goodbye User\n");
            send_response_to_client(socket, client_address, EXIT, RESPONSE, "Goodbye User\n");
        }
            break;

        default:
            break;

    }


}


/*
Main Code
*/

int main() {
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    struct sockaddr_in client_address;
    socklen_t addr_size;
    int server_socket_fd;
    socklen_t client_address_len;

    //Create the socket.
    /*Create the server socket */
    server_socket_fd = CreateServerSocket(8989);

    //Initialize the client list
    memset(users, 0, sizeof(users));
    memset(onlineUser, 0, sizeof(onlineUser));


    while (1) {

        chat_message_t message;
        client_address_len = sizeof client_address;
        if (recvfrom(server_socket_fd,
                     &message,
                     sizeof message,
                     0,
                     (struct sockaddr *) &client_address,
                     &client_address_len) < 0) {
            perror("recvfrom");
            exit(1);
        }

        reader(server_socket_fd, &client_address, message);
    }

    /*
    //close the socket
    close(serverAddr);
    */

    return 0;

}

int CreateServerSocket(int port) {
    struct sockaddr_in address;
    int socket_fd;

    /* Initialise IPv4 address. */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    /* Create UDP socket. */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* Bind address to socket. */
    if (bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("bind");
        exit(1);
    }

    // Configure server socket
    int enable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    return socket_fd;
}

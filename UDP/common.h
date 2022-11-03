#include <netinet/in.h> //INETADDSTRLEN
#include <stdbool.h>
#pragma once

#define messageLength 100
#define userData 30
#define sendLength 100
#define numberW 100 //default size 100
#define SUCCESS 0
#define FAILURE 1



//DATA STRUCTURES

/*
struct for received messages
*/

struct messagesRecv{
    char from[userData];
    char message[messageLength];
};

/*
struct for messages sent
*/
struct messageSent{
    char to[userData];
    char message[messageLength];
};

/*
struct for User
*/
struct User{
    char userName[userData];
    char password[userData];
    bool isLoggedIn;
    struct messagesRecv recv[sendLength];
    struct messageSent sent[messageLength];
    int recvCounter;
    int sentCounter;
};

typedef struct response{
    int num_of_responses;
    int status;
    char response[numberW*2][messageLength];
}response_t;


/*
struct for Commands
*/
typedef enum supported_commands {
    REGISTER = 1,
    LOGIN = 2,
    LOGOUT = 3,
    SEND = 4,
    RETRIEVE = 5,
    LIST = 6,
    EXIT = 7,
    INVALID
} supported_commands_t;


/*
struct for packet
*/
typedef enum packet_type {
    COMMAND = 1,
    RESPONSE
} packet_type_t;


/*
struct for chat message
*/
typedef struct chat_message{
    supported_commands_t command;
    packet_type_t type;
    char client_id[userData];
    union {
        struct User user;
        struct messageSent message;
        response_t response;
    }u;
}chat_message_t;
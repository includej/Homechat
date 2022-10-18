#include <netinet/in.h> //INETADDSTRLEN
#pragma once

#define messageLength 300
#define userData 30
#define sendLength 300
#define pwd 20

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
    char username[userData];
    char password[pwd];
    struct messagesRecv recv[sendLength];
    struct messageSent sent[messageLength];
    int recvCounter;
    int sentCounter;
};
#include <netinet/in.h> //INETADDSTRLEN
#pragma once

#define messageLength 100
#define userData 30
#define sendLength 100


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
    struct messagesRecv recv[sendLength];
    struct messageSent sent[messageLength];
    int recvCounter;
    int sentCounter;
};
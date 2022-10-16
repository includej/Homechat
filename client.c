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
#include <netinet/in.h> //INETADDSTRLEN

/*
Threads libraries
*/
#include <pthread.h>


/*
Data structures
*/
#include "common.h"


/*
Client side user signUp
*/
void signUp(struct User *user)
{
    printf("Enter username: ");
    scanf("%s", user->username);
    printf("Enter password: ");
    scanf("%s", user->password);
}


/*
Client side user login
*/
void login(struct User *user){
    printf("Enter username: ");
    scanf("%s", user->username);
    printf("Enter password: ");
    scanf("%s", user->password);
}



/*
Client Side Thread
*/
void clienthread(int cl_request){

}




int main()
{
    int client_request = 1;
    clienthread(client_request);
}



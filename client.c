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
void login(struct User *user)
{
    printf("Enter username: ");
    scanf("%s", user->username);
    printf("Enter password: ");
    scanf("%s", user->password);
}

void prepareMessage(struct messageSent *msg){
    printf("Enter username of the receiver: ");
    scanf("%s",msg->to);
    printf("Enter message: ");
    getchar();
    fgets(msg->message, sizeof(msg->message), stdin); //Taking string input of message

}



/*
Client Side Thread
*/
void clienthread(int client_r){
    /*Initializing socket*/
    int client_request = client_r;
    int sockfd;
g
    char *ip = "127.0.0.1";

    /*Creating socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1){
        printf("Socket Creation failed...\n");
        exit(0);
    }else
        printf("Socket successfully created...\n");

    /*Initializing IP and PORT for connection */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(8989); //port for socket connection

    int connection_status = connect(sockfd, 
                            (struct sockaddr*)&server_address, 
                            sizeof(server_address));

    if(connection_status<0){
        printf("Connection with the server failed");
        exit(0);
    }
    else
        /*Connection Established Here*/
        printf("Connected to the server...\n");


    int choice=0;

    system("clear");

    /* Loop to deal with user choice*/

    while(1)
    {
        printf("\n----------Homechat----------\n\n");
        printf("(1) SignUp\n");
        printf("(2) Login\n");
        printf("(3) Exit\n");
        scanf("%d", &choice);
        
        /*SignUp choice*/
        if(choice==1){
            send(sockfd, &choice, sizeof(choice),0); //sending user choice to server [1]
            struct User user; //struct for saving userdetails
            signUp(&user);
            send(sockfd,&user,sizeof(user),0); //sending user to be saved in the server
            printf("\nSignin In...\n");
            sleep(2);
        }
        /*Login choice*/
        else if(choice==2){
            send(sockfd,&choice,sizeof(choice),0); //sending user choice [2]
            struct User user; // preparing struct to be send on the server
            login(&user);
            send(sockfd, &user, sizeof(user),0); // user to the server
            
            int ret=0;
            recv(sockfd, &ret, sizeof(ret),0); //receiving response from server (if -1 user not verfied else user verified)
        
            if(ret>=0)
            {
                printf("\nLogging In...\n");
                sleep(2);
                break;
            }
            else{
                printf("**Invalid credentials of user does not exists in server**...\n");
                sleep(2);
            }
        
        }
        /*Exit choice*/
        else if(choice==3){
            send(sockfd, &choice, sizeof(choice), 0); //sending user choice [3]
            printf("\n Goodbye User\n");
            sleep(1);

            exit(1); //Exiting from client program
        }
        else
        {
            printf("\nInvalid Option...\n");
            sleep(1);
            
        }
        system("clear");
    }

    close();
    pthread_exit(NULL);
}


/*
Main code
*/
int main()
{
    int client_request = 1;
    clienthread(client_request);
}



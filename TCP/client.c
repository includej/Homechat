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


/*
Client side user signUp
*/
void signUp(struct User *user)
{
    printf("Enter username: ");
    scanf("%s", user->userName);
    printf("Enter password: ");
    scanf("%s", user->password);
}


/*
Client side user login
*/
void login(struct User *user)
{
    printf("Enter username: ");
    scanf("%s", user->userName);
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
        puts("Error\n");
        return;
    }
    
    /* Connection established here */
    printf("Connection established\n");

    int choice=1;

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

    while(choice!=3)
    {
        system("clear");
        printf("\n----HC----Welcome to HOME----HC----\n");
        printf("(4) Send a private message to User\n");
        printf("(5) Check your Inbox\n");
        printf("(6) Registered User Online in Server\n");
        printf("(7) Exit\n");

        scanf("%d", &choice);

        //Send message choice
        if(choice==4){
            send(sockfd, &choice, sizeof(choice), 0); //sending user choice to server [4]
            struct messageSent tmp; //message to be sent to user
            prepareMessage(&tmp);
            send(sockfd, &tmp, sizeof(tmp),0); //sending user message to dest

            /* receiving the response of the server */
            int ret=0;
            recv(sockfd, &ret, sizeof(ret), 0); 
            if(ret>=0){
                //if response >= 0 MESSAGE SENT
                printf("\nMessage sent to user...\n");
            }
            else
            {
                printf("\n User not found in the database...\n");
            }
            sleep(2);

        }
        
        /*Received message choice*/
        else if (choice==5){
            send(sockfd, &choice, sizeof(choice), 0); //sending user choice to server [5]

            /* receiving inbox count of user recCounter*/
            int rec=0;
            recv(sockfd, &rec, sizeof(rec),0);
            printf("\n----------Total messages: %d----------\n\n", rec);

            /*receiving till all the message from user inbox is not fetched */
            for(int i = 0; i < rec; i++)
            {
                struct messagesRecv fr;
                recv(sockfd,&fr,sizeof(fr),0);
                printf("[%s]:%s\n", fr.from, fr.message);

            }

            printf("\n----Press any key to return back to Home----\n%c", getchar());
            getchar();
        }
    
        /*Online user choice*/
        else if(choice==6)
        {
            send(sockfd, &choice, sizeof(choice), 0); //sending user choice to server [6]    
            int rec=0;
            recv(sockfd, &rec, sizeof(rec), 0);
            printf("\n----------Online user available: %d----------\n\n", rec);
            for(int i = 0; i < rec; i++){
                struct User fr;
                recv(sockfd, &fr, sizeof(fr),0);
                printf("User: %s\n", fr.userName);
            }

            printf("\n----Press any key to return back to Home----\n%c", getchar());
            getchar();

        }

        /*Exit choice*/
        else if(choice==7){
            send(sockfd, &choice, sizeof(choice), 0); //sending user choice to server [7]
            /*Exiting client*/
            printf("\n Good bye User\n");
            sleep(1);
            break;
        }
        else{

        }

    }

    /*Closing connection*/
    close(sockfd);
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



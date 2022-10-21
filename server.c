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

#define numberW 100 //default size 100

pthread_t tid;
pthread_t readerthreads[numberW];

struct User users[numberW]; //Structure User for storing UserData

int onlineUser[numberW];
int userCounter = 0; //counter to keep track of number of users



/*
Function for Reading Database
*/
void readDatabase(){
    FILE *fp = fopen("db.txt", "r"); //read

    if(fp==NULL){
        printf("Error opening DB file \n");
    }
    
    printf("Reading database...\n");
    while(fscanf(fp, "%s %s", users[userCounter].username, users[userCounter].password)!=-1)
    {
        
        //initialize usr
        users[userCounter].recvCounter = 0;
        users[userCounter].sentCounter = 0;
        userCounter++;

    }
    fclose(fp);
}

void addUserToDB(struct User tmp)
{
    FILE *fp=fopen("db.txt", "a");  //append

    if(fp == NULL){
        printf("Error opening DB File\n");
    }

    strcpy(users[userCounter].username, tmp.username);
    printf("Username: %s\n", users[userCounter].username);
    strcpy(users[userCounter].password, tmp.password);
    printf("Pwd: %s\n", users[userCounter].password, tmp.password);

    //print on DB usr and pwd
    fprintf(fp, "%s %s\n", users[userCounter].username, users[userCounter].password);
    

    //initializing usr
    users[userCounter].recvCounter = 0;
    users[userCounter].sentCounter = 0;
    userCounter++;                          //increment the counter 

}



/*
Function to verify user in server
*/

int LoginVerify(struct User tmp)
{
    for(int i=0; i < userCounter; i++)
    {
        if(strcmp(users[i].username, tmp.username)==0 && strcmp(users[i].password, tmp.password)==0)
        {
            return i;
        }
    }
    
    return -1;
}

/*
    Server Side Thread to handle each client on server
*/

void* reader(void* param)
{
    
    

}



/*
Main Code
*/

int main()
{
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    int serverSocket, sockfd;

    char *ip = "127.0.0.1";

    //socket create and verification
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == -1){
        printf("Socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket succesfully created...\n");
    
    bzero(&serverAddr, sizeof(serverAddr));

    //assign IP, PORT
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    //Binding newly created socket to given IP and verification

    if(bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr))!=0) {
        printf("Socket bind failed...\n");
        exit(0);
    };
    else   
        printf("Socket succesfully binded...\n");

    
    pthread_t tid[60];

    int i = 0;
    for(int i = 0; i < numberW; i++){
        onlineUser[i] = 0;
    }


    while(1){

        //Now server is ready to listen and verification

        if(listen(serverAddr, 1) >= 0){  //1 -> backlog
            printf("Listening...\n");
        }
        else
            printf("Error\n");

        addr_size = sizeof(serverStorage);

        //accept the data packet from client and verification

        sockfd = accept(serverAddr, (struct sockaddr*)&serverStorage, &addr_size);
        if(sockfd < 0){
            printf("Server accept failed...\n");
            exit(0);            
        }
        else
            printf("Server accept the client...\n");
        
        int choice = 0;
        printf("[DBG]Thread created...\n");

        if(pthread_create(&readerthreads[i], NULL,reader,&sockfd)!= 0)
        {
            printf("Failed to create thread\n");
        }

        i++;
        pthread_detach(readerthreads[i]);


    }

    /*
    //close the socket
    close(serverAddr);
    */

    return 0;

}
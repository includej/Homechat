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

//////////////

pthread_t tid;
pthread_t readerthreads[numberW];


int onlineUser[numberW]; //array that keep track of online users
int onlineCounter = 0; //counter to keep track of number of online users
struct User users[numberW]; //Structure User for storing UserData
int userCounter = 0; //counter to keep track of number of users



/*
Function to add user to server Database 
*/

void addUserToDB(struct User tmp)
{
    FILE *fp=fopen("db.txt", "a");  //append

    if(fp == NULL){
        printf("Error opening DB File\n");
    }

    strcpy(users[userCounter].userName, tmp.userName);
    printf("Username: %s\n", users[userCounter].userName);
    strcpy(users[userCounter].password, tmp.password);
    printf("Pwd:%s\n", users[userCounter].password);

    //print on DB usr and pwd
    fprintf(fp,"%s %s\n", users[userCounter].userName, users[userCounter].password);
    fclose(fp);

    //initializing usr
    users[userCounter].recvCounter = 0;
    users[userCounter].sentCounter = 0;
    userCounter++;                          //increment the counter 

}

/*
Function for Reading Database
*/



void readDatabase(){
    FILE *fp = fopen("db.txt", "r"); //read

    if(fp==NULL){
        printf("Error opening DB file \n");
    }
    
    printf("Reading database...\n");
    while(fscanf(fp, "%s %s", users[userCounter].userName, users[userCounter].password)!=-1)
    {

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

int LoginVerify(struct User tmp)
{
    for(int i=0; i < userCounter; i++)
    {
        if(strcmp(users[i].userName, tmp.userName)==0 && strcmp(users[i].password, tmp.password)==0)
        {
            return i;
        }
    }
    
    return -1;
}


/*
Function to send message to receiver in server and updates receiver inbox 
*/

int sendMessage(int index, struct messageSent msg){

    /*Updating user receiver Message*/
    for(int i = 0; i < userCounter; i++)
    {
        if(strcmp(users[i].userName, msg.to)==0)
        {
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

void* reader(void* param)
{
    readDatabase();
    int newSocket=*((int*)param);
    int logIndex = -1;

    printf("\n[DBG]Connection established\n..");
    int choice = 0;

    while(1){
        int i = 0;

        //receive the choice of the user
        recv(newSocket, &choice, sizeof(choice), 0);

        /* Choice checked -> what user has chosen */
        /* Register (1) */
        if(choice == 1)
        {
            printf("Server is registering user\n");
            struct User tmp;

            //receiving the user data struct
            recv(newSocket, &tmp, sizeof(tmp), 0);
            addUserToDB(tmp);

            printf("User registered in server succesfully...\n");

        }
        /* Login (2) */
        else if(choice==2)
        {
            printf("Server is verifying user credentials...\n");
            struct User tmp;

            //receiving the credentials of the user using a data structure
            recv(newSocket, &tmp, sizeof(tmp), 0);
            

            //compare inserted user with my data struct users
            int ret = LoginVerify(tmp);
            if(ret>=0)
            {
                //ret -> index of the current user
                logIndex = ret;
                onlineUser[onlineCounter] = logIndex;
                onlineCounter++;

                printf("User credentials verified succesfully.\n");
                send(newSocket, &ret, sizeof(ret), 0);
                break;

            }
            else
            {
                printf("User credentials not verified succesfully\n");
                send(newSocket, &ret, sizeof(ret), 0);
            }

        }
        /* Exit (3) */
        else if(choice == 3)

        {
            printf("\n Goodbye User\n");
            break;
        }



    }
    while(choice != 3) //(choice == 3 exit | other case Login Verified -> Jump here)
    {
        recv(newSocket, &choice, sizeof(choice),0); //receiving choice of user

        /*Sending message to user (4)*/
        if(choice == 4)
        {
            printf("Sending message to User\n");
            struct messageSent tmp;
            recv(newSocket, &tmp, sizeof(tmp), 0); //receive message struct
            int ret = sendMessage(logIndex, tmp);
            send(newSocket, &ret, sizeof(ret), 0); //sending whether user found or not
        }

        /*Check inbox of user (5)*/
        else if(choice == 5) 
        {
            send(newSocket, &users[logIndex].recvCounter, sizeof(users[logIndex].recvCounter),0);
            for(int i = 0; i < users[logIndex].recvCounter;i++){

                send(newSocket, &users[logIndex].recv[i], sizeof(users[logIndex].recv[i]),0);
            }
        }
        /* Check number of online Users with name(6)*/
        else if(choice == 6)
        {
            send(newSocket, &onlineCounter, sizeof(onlineCounter), 0); //send number of users available

            for(int i=0; i < onlineCounter; i++)
            {
                //send data struct of onlineUser[i]
                send(newSocket, &users[onlineUser[i]], sizeof(users[onlineUser[i]]), 0);
            }
        }
        /* Exiting from server (7) */
        else if(choice == 7)
        {
            int flag = -1;
            for(int i = 0; i < onlineCounter; i++){
                if(onlineUser[i] = logIndex || flag!=-1)
                {
                    onlineUser[i] = onlineUser[i+1];
                    flag = 1;
                }
            }

            onlineCounter--;
            printf("\n Goodbye user\n");
            break;
        }

    }
    pthread_exit(NULL);    

}


/*
Main Code
*/

int main()
{
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    int serverSocket, newSocket;

    char *ip = "127.0.0.1";

    //socket create and verification
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == -1){
        printf("Socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket succesfully created...\n");
    

    //assign IP, PORT
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    //Binding newly created socket to given IP and verification

    if(bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr))!=0) {
        printf("Socket bind failed...\n");
        exit(0);
    }
    else   
        printf("Socket succesfully binded...\n");

    
    pthread_t tid[60];

    int i = 0;
    for(int i = 0; i < numberW; i++){
        onlineUser[i] = 0;
    }


    while(1){

        //Now server is ready to listen and verification

        if(listen(serverSocket, 1) >= 0){  //1 -> backlog
            printf("Listening...\n");
        }
        else
            printf("Error\n");

        addr_size = sizeof(serverStorage);

        //accept the data packet from client and verification

        newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);
        if(newSocket < 0){
            printf("Server accept failed...\n");
            exit(0);            
        }
        else
            printf("Server accept the client...\n");
        
        int choice = 0;
        printf("[DBG]Thread created...\n");

        if(pthread_create(&readerthreads[i], NULL,reader,&newSocket)!= 0)
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
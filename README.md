# SoProject

## HomeChat

HomeChat is a simple **private chat**;
Basically *text messages* are exchanged between two or more hosts. 
There are two different version:
- TCP  
- UDP

User will have multiple choices at the beginning:
1) SignUp 
2) Login 
3) Exit

After the user has logged  in he can choose between:

4) Send a new message to a dest
5) Check messages in the inbox
6) Check who is online at that precise moment
7) Exit

## Functions

*server.c*

**Function** | **Info**
-- | -- 
LoginVerify | verifying user in server
sendMessage |sending a message to receiver in server and updates receiver inbox
readDatabase |  reading database
addUserToDB | add user to a server database

*client.c*

**Function** | **Info**
-- | -- 
signUp | get registration info of the user
login | get login info of the user
prepareMessage | set info of a prepared message [dest]:[message] 

## Data structures

*common.h*

**Data structure** | **Info**
-- | --
messageRecv | received messages handling
messageSent | sent messages handling
User | user info


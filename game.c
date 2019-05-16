#include "game.h"

int *currPlayers;
struct playerInfo *pArray;

//struct playerInfo pArray[MAX_PLAYERS];
//This specifies all our info for our clients, ie.
//playerInfo[ID number][client_fd][number of lives][bool: ID number taken]
//int playerInfo[MAX_PLAYERS][4];

/**
* Based on code found at https://github.com/mafintosh/echo-servers.c (Copyright (c) 2014 Mathias Buus)
* Copyright 2019 Nicholas Pritchard, Ryan Bunney
**/

/**
 * @brief A simple example of a network server written in C implementing a basic echo
 * 
 * This is a good starting point for observing C-based network code but is by no means complete.
 * We encourage you to use this as a starting point for your project if you're not sure where to start.
 * Food for thought:
 *   - Can we wrap the action of sending ALL of out data and receiving ALL of the data?
 */


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int server_fd, client_fd, opt_val, err, pid;
    struct sockaddr_in server, client;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        fprintf(stderr, "Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    opt_val = 1;

    struct timeval tv;
    //Set the timeout value to 30 seconds
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);


    err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if (err < 0)
    {
        fprintf(stderr, "Could not bind socket\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); 
    //WORKING ON THIS CURRENTLY
    init_game_data();

    //Adding this new function to listen
    //listenForPlayers(server_fd, port);
    err = listen(server_fd, 128);
    if (err < 0)
    {
        fprintf(stderr, "Could not listen on socket\n");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %d\n", port);

    socklen_t client_len = sizeof(client);

    bool gameInSession = false;
    time_t start = time(NULL);
    //This timeout isn't needed, the accept function will timeout after 30 seconds anyway
    while (time(NULL) - start < 30) {  //Loop for accepting multiple clients

        if(pArray[0].taken){
            printf("Client %d has been allocated\n", pArray[0].playerID);
        }

        printf("Current players: %d\n", *currPlayers);

        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);        

        if(*currPlayers > MAX_PLAYERS){            //This checks whether game is full or not
            send_message(client_fd, REJECT);      //Could replace the currPlayers>MAX_PLAYERS with the gameInSession bool
            printf("Another player attempted to join, was rejected\n");
            close(client_fd);
            *currPlayers = *currPlayers - 1;
            continue;
        }

        if (client_fd < 0)
        {
            fprintf(stderr, "Could not establish connection with new client\n");
            close(client_fd);
            continue;
        }

        switch(pid = fork()) {
            case -1:
                perror("Fork error\n");
                exit(EXIT_FAILURE);
            case 0:
                close(server_fd);
                handleInit(client_fd);
                *currPlayers = *currPlayers + 1;                
                exit(EXIT_SUCCESS);
                break;
            default:
                //Handle parent process
                //Set up some shared memory here, so that when you fork() and call generateNewPlayer(), 
                //the variables that it changes will be changed in the parent process.
        
                //playGame() should only ever be called once.
                
                if(!gameInSession && *currPlayers == MAX_PLAYERS) {
                    gameInSession = true;
                    playGame();
                }
                break;
        }
        
    }  

    /*
    * After you call play game you are going to have to deal with other clients who try to 
    * enter the game. You will have to continue listening in a while(true) loop, and then fork()
    * the process whenever you catch someone wanting to join. 
    */

    exit(EXIT_SUCCESS); 
}
#include "game.h"

int *currPlayers; 
struct playerInfo *pArray;
bool *gameFinished;
int MAX_PLAYERS;

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
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [port] [numPlayers]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    MAX_PLAYERS = atoi(argv[2]);
    if(MAX_PLAYERS < 1) {
        fprintf(stderr, "There must be at least 1 player in game\n");
        exit(EXIT_FAILURE);
    }

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

    struct timeval tv;
    //Set the timeout value to TIMEOUT, defined in header file
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    opt_val = 1;    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);


    err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if (err < 0)
    {
        fprintf(stderr, "Could not bind socket\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); 

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
    //bool gameCompleted = false;
    //time_t start = time(NULL);
    //This timeout isn't needed, the accept function will timeout after 30 seconds anyway
    while (true) {  //Loop for accepting multiple clients

        if(*gameFinished) {
            printf("Game completed, closing server\n");
            close(server_fd);
            exit(EXIT_SUCCESS);
        }

        if(!gameInSession && *currPlayers == MAX_PLAYERS) {
            gameInSession = true;

            //Set timeout in parent to be much shorter
            struct timeval t;
            //Set the timeout value to 5
            t.tv_sec = 5;
            t.tv_usec = 0;
            setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof t);

            printf("%d players have joined, game has started\n", MAX_PLAYERS);

            switch(pid = fork()) {
                case -1:
                    perror("Fork error\n");
                    exit(EXIT_FAILURE);
                case 0:
                    //Play singleplayer game if MAX_PLAYERS == 1, else play normal game
                    if (MAX_PLAYERS == 1) {
                        playSpGame();
                    }
                    else {
                        playGame();
                    }
                    *gameFinished = true;
                    exit(EXIT_SUCCESS);
                default:
                    break;
            }

        }

        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

        // Here we deal with players who are joining late -- the game is already in session
        // We send them a REJECT message
        if(gameInSession && client_fd != -1) { 
            switch(pid = fork()) {
                case -1:
                    perror("Fork error");
                    exit(EXIT_FAILURE);
                case 0:
                    handleInit(client_fd);
                    printf("Another player attempted to join, was rejected\n");
                    close(client_fd);
                    exit(EXIT_SUCCESS);
                default:
                    break; 
            }
        }
       
        //This is for cancelling a game when too few players show up before the timeout
        if (client_fd < 0 && errno == EAGAIN && !gameInSession)
        {
            fprintf(stderr, "Took too long to set up lobby\n");
            for(int i = 0; i < MAX_PLAYERS; i++) {
                if(pArray[i].taken){
                    send_message(pArray[i].client_fd, CANCEL);
                    close(pArray[i].client_fd);
                }
            }
            close(server_fd);           //Tear down server
            exit(EXIT_FAILURE);
        }

        if(!gameInSession){
            switch(pid = fork()) {
                case -1:
                    perror("Fork error\n");
                    exit(EXIT_FAILURE);
                case 0:
                    //close(server_fd);
                    handleInit(client_fd);
                    *currPlayers = *currPlayers + 1;     
                    exit(EXIT_SUCCESS);                
                default:
                    wait(NULL);   //Still seems to work with multiple clients joining at same time with this wait               
                    break;
            }
        }
        
    }  

    /*
    * After you call play game you are going to have to deal with other clients who try to 
    * enter the game. You will have to continue listening in a while(true) loop, and then fork()
    * the process whenever you catch someone wanting to join. 
    */

    exit(EXIT_SUCCESS); 
}
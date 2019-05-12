#include "game.h"

int numLives = 3;
int currPlayers = 0;
int playerInfo[MAX_PLAYERS][2];

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

void message_pass(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,PASS", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "PASS message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}    

void message_fail(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,FAIL", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "FAIL message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

void message_welcome(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,WELCOME", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "WELCOME message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

void message_elim(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,ELIM", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "ELIM message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}


//Provides an id for a client that isn't taken
int generateNewId(void){
    int id;
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(!playerInfo[i][1]){
            continue;
        }
        else{
            id = playerInfo[i][0];
            break;
        }
    }
    return id;
}


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

    err = listen(server_fd, 128);
    if (err < 0)
    {
        fprintf(stderr, "Could not listen on socket\n");
        exit(EXIT_FAILURE);
    }

    init_game_data();

    printf("Server is listening on %d\n", port);

    socklen_t client_len = sizeof(client);
    
    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char));

    srand(time(NULL));      //Outside of any loop

    while (true) {  //Loop for accepting multiple clients
        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
        
        if(currPlayers < MAX_PLAYERS){     //This checks whether game is full or not, 
            currPlayers++;                //if full it rejects the client attempting to join
        }
        else {            
            sprintf(buf, "REJECT");
            int err = send(client_fd, buf, strlen(buf), 0);
            if(err < 0) {
                fprintf(stderr, "REJECT message failed to send\n"); //Doesn't really matter as it will be closed anyway
            }
            close(client_fd);
            continue;
        }

        if (client_fd < 0)
        {
            fprintf(stderr, "Could not establish connection with new client\n");
            exit(EXIT_FAILURE);
        }

        pid = fork();

        switch(pid) {

            case -1 :                   //Failure to fork
                perror("Fork error");
                exit(EXIT_FAILURE);
                break;

            case 0:                     //Client process
                close(server_fd);
                handleClient(client_fd);
                exit(EXIT_SUCCESS);            

            default:                    //Parent process
                close(client_fd);
        }
    }   
}
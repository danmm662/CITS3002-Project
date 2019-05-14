#include "game.h"

int numLives = 3;
int currPlayers = 0;
//This specifies all our info for our clients, ie.
//playerInfo[ID number][client_fd][number of lives][bool: ID number taken]
int playerInfo[MAX_PLAYERS][4];

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

/*
 * This is currently a big function, with lots of things in it
 * It needs to be broken down... and I am doing it with smaller functions, but keeping this big one here
 */
void handleClient(int client_fd){
    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); // Clear our buffer so we don't accidentally send/print garbage
    int read = recv(client_fd, buf, BUFFER_SIZE, 0); // Try to read from the incoming client, expecting INIT

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, not sure if necessary
        fprintf(stderr, "Client timed out\n");
        exit(EXIT_FAILURE);
    }
    else if (read <= 0){
        fprintf(stderr, "Client read failed\n");
        //message_elim(client_fd);
        send_message(client_fd, ELIM);
        exit(EXIT_FAILURE);
    }

    printf("New client's message: %s\n", buf);

    struct messageProperties p;
    p = parse_message(buf);

    if(p.flag == INIT) {
        generateNewPlayer(client_fd, numLives);   //If client sends INIT, give them an id and send it to them
        send_message(client_fd, p.flag);   //Need to pass id to the message() function, ie message(int flag, int id)
    } else {
        fprintf(stderr, "INIT was not the first message received\n");
        exit(EXIT_FAILURE);
    }

    send_message(client_fd, START);

    while(true)
    {   
        //Should this be checked before or after numLives < 1?
        if(currPlayers < 2){
            printf("The winner(s) have been found\n");
            send_message(client_fd, VICT);
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        //sleep(1);
        if (numLives < 1)
        {
            printf("Client lost all lives, eliminated\n");
            send_message(client_fd, ELIM);
            //message_elim(client_fd);
            close(client_fd); 
            currPlayers--;       
            exit(EXIT_SUCCESS);
        }

        //sleep(1); //Wait 3 seconds
        memset(buf, 0, BUFFER_SIZE);
        read = recv(client_fd, buf, BUFFER_SIZE, 0); // See if we have a response
        
        if (read <= 0 && errno == EAGAIN) {                  //Timeout for client messages, if read fails then client
            printf("Client took too long, lost a life\n");  //client loses a life and starts loop again
            numLives--;
            continue;
        }
        else if (read <= 0) {
            fprintf(stderr, "Client read failed\n");
            exit(EXIT_FAILURE);
        }

        printf("Client's Message: %s\n", buf);

        p = parse_message(buf); //parse the new message and return it to p
        int *diceRoll = roll_dice();

        switch (p.flag)
        {
        case INIT:
            exit(EXIT_FAILURE); //Here, the client sends another INIT message--not expected so exit()
        case EVEN:
            if(check(diceRoll, p.flag, 0)) {
                send_message(client_fd, p.flag);
                //message_pass(client_fd);
            } else {
                send_message(client_fd, p.flag);
                //message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case ODD:
            if(check(diceRoll, p.flag, 0)) {
                send_message(client_fd, p.flag);
                //message_pass(client_fd);
            } else {
                send_message(client_fd, p.flag);
                //message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case DOUB:
            if(check(diceRoll, p.flag, 0)) {
                send_message(client_fd, p.flag);
                //message_pass(client_fd);
            } else {
                send_message(client_fd, p.flag);
                //message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case CON:
            if (check(diceRoll, p.flag, p.conChoice)) {
                send_message(client_fd, p.flag);
                //message_pass(client_fd);
            } else {
                send_message(client_fd, p.flag);
                //message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case ERR:
            printf("Invalid move, client loses a life\n");
            numLives--;
            break;
        }
    }
}
    // while (true) {//Do stuff with fork to stop other clients from joining } //Loop for accepting multiple clients
        
    // }
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

    srand(time(NULL)); 
    //WORKIGN ON THIS CURRENTLY
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
    
    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char));

    bool gameInSession = false;
    time_t start = time();
    while (time() - start < 30) {  //Loop for accepting multiple clients

        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

        if (client_fd < 0)
        {
            fprintf(stderr, "Could not establish connection with new client\n");
            //exit(EXIT_FAILURE);
        }

        if(pid = fork() == 0) {
            //This function automatically calls generateNewPlayer()
            handleInit(client_fd);
            currPlayers++;
        } else {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }

        //Set up some shared memory here, so that when you fork() and call generateNewPlayer(), 
        //the variables that it changes will be changed in the parent process.
        
        //playGame() should only ever be called once.
        if(gameInSession == false && currPlayers == MAX_PLAYERS) {
            playGame();
            gameInSession = true;
        }
    }  

    /*
    * After you call play game you are going to have to deal with other clients who try to 
    * enter the game. You will have to continue listening in a while(true) loop, and then fork()
    * the process whenever you catch someone wanting to join. 
    */

    exit(EXIT_SUCCESS); 
}

        //SOME LEFTOVER CODE FROM THE FORK(), WHICH WAS IN THE WRONG PLACE
        // pid = fork();

        // switch(pid) {

        //     case -1 :                   //Failure to fork
        //         perror("Fork error");
        //         exit(EXIT_FAILURE);
        //         break;

        //     case 0:                     //Client process
        //         close(server_fd);
        //         handleClient(client_fd);
        //         exit(EXIT_SUCCESS);            

        //     default:                    //Parent process
        //         close(client_fd);
        // }


        
        //SOME LEFTOVER CODE FOR THE REJECT STATEMENT, WHICH WAS IN THE WRONG PLACE
        // else {
        //     pid = fork();
        //     if(pid == 0) {            
        //     sprintf(buf, "REJECT");
        //     int err = send(client_fd, buf, strlen(buf), 0);
        //     if(err < 0) {
        //         fprintf(stderr, "REJECT message failed to send"); //Doesn't really matter as it will be closed anyway
        //     }
        //     close(client_fd);
        //     exit(EXIT_SUCCESS);
        //     }
        //     break;
        // }
//
//  messages.c
//  
//
//  Created by Kieren Underwood on 1/5/19.
//

#include "game.h"


void send_message(int client_fd, int flag) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));

    int playerID;
    
    //New way to search through the player info to find player ID
    //not sure if best method
    struct playerInfo p;

    for(int i = 0; i < MAX_PLAYERS; i++) {
        p = pArray[i];
        if(p.client_fd == client_fd){
            break;
        }
        //This is for when there is no record of the client_fd being stored
        //Don't know if it is worth testing for
        else if(i == (MAX_PLAYERS - 1)) { //Problem occurs when client joins when lobby is full as they have not 
            fprintf(stderr,"Unable to find the client's info\n"); //received a playerID yet
            exit(EXIT_FAILURE);
        }
    }

    playerID = p.playerID;
    
    switch(flag) {
        case PASS :
            sprintf(messbuf, "%d,PASS", playerID);
            break;
        case FAIL :
            sprintf(messbuf, "%d,FAIL", playerID);
            break;
        case ELIM :
            sprintf(messbuf, "%d,ELIM", playerID);
            break;
        case WELCOME :
            sprintf(messbuf, "WELCOME,%d", playerID);
            break;
        case VICT :
            sprintf(messbuf, "%d,VICT", playerID);
            break;
        case CANCEL :
            sprintf(messbuf, "CANCEL");
            break;
        case REJECT :
            sprintf(messbuf, "REJECT");
            break;
        case START :
            sprintf(messbuf, "START,%d,%d", MAX_PLAYERS, MAX_LIVES);
            break;
        default:
            fprintf(stderr, "Invalid message\n");
            exit(EXIT_FAILURE);
    }  

    int err = send(client_fd, messbuf, strlen(messbuf), 0);
    if(err < 0) {
        fprintf(stderr, "Message failed to send\n");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

/*
* If successful this will @return a struct containing the guess of the player
* Right now, it DOES NOT handle the case where it sends a message which was corrupt
*/
//Changed this so it takes an int argument, not sure how it would work
//just taking void
struct messageProperties getGuess(int client_fd) {
    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); 

    int read = recv(client_fd, buf, BUFFER_SIZE, 0);

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, not sure if necessary
        fprintf(stderr, "Client timed out\n");
        //exit(EXIT_FAILURE);
    }
    else if (read <= 0){
        fprintf(stderr, "Client read failed\n");
        //message_elim(client_fd);
        //send_message(client_fd, ELIM);
        //PROBABLY SHOULD MAKE A WAY FOR THIS TO RETURN A STATE WHERE THEY LOSE A LIFE IF 
        //THE MESSAGE IS NOT READABLE
        //exit(EXIT_FAILURE);
    }

    

    struct messageProperties p;
    //char *message = calloc(strlen(buf), sizeof(char)); 
    //strcpy(message, buf);
    p = parse_message(buf);
    //printf("Client %d's chose %d\n", p.id, p.flag);

    return p;
}
/*
* @returns a bool which is true if we received an INIT, false if we do not
* The whole function will only be called when we have forked() another process,
* so this is why we are calling exit(EXIT_SUCCESS) or exit(EXIT_FAILURE) upon completion
*/
void handleInit(int client_fd) {

    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); 

    int read = recv(client_fd, buf, BUFFER_SIZE, 0);

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, not sure if necessary
        fprintf(stderr, "New client timed out\n");
        exit(EXIT_FAILURE);
    }
    else if (read <= 0){
        fprintf(stderr, "Client read failed\n");
        //message_elim(client_fd);
        send_message(client_fd, ELIM);
        exit(EXIT_FAILURE);
    }

    printf("Client %d's message: %s\n", *currPlayers + 100, buf);

    struct messageProperties p;
    p = parse_message(buf);

    if(p.flag == INIT && *currPlayers < MAX_PLAYERS) {
        generateNewPlayer(client_fd, *currPlayers);     //If client sends INIT, give them an id and send it to them
        send_message(client_fd, WELCOME);               //Need to pass id to the message() function, ie message(int flag, int id)
        return;
    } 
    else if(p.flag == INIT && *currPlayers >= MAX_PLAYERS){
        send_message(client_fd, REJECT);
        
    }
    else {
        fprintf(stderr, "INIT was not the first message received\n");
        exit(EXIT_FAILURE);
    }

}
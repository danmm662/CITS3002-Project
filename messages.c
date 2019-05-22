// CITS3002 Project 2019
// Names:            Kieren Underwood,   Daniel Maitland
// Student Numbers:  21315543            21986102
// Compiles on Linux

#include "game.h"

/* A message specifically for reject as, unlike other messages, 
*  this is sent to the client without a corresponding ID number */
void send_reject(int client_fd) {
    
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "REJECT");

    int err = send(client_fd, messbuf, strlen(messbuf), 0);
    if(err < 0) {
        perror("Reject message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

void send_message(int client_fd, int flag) {
    //Set up buffer to send string
    char * messbuf; 
    messbuf = calloc(BUFFER_SIZE, sizeof(char));

    int playerID;
    struct playerInfo p;

    //Search through our array to find if the client_fd matches the playerID
    for(int i = 0; i < MAX_PLAYERS; i++) {
        p = pArray[i];
        if (p.client_fd == client_fd){
            break;
        }
        //This is for when there is no record of the client_fd being stored
        else if (i == (MAX_PLAYERS - 1)) {                        
            fprintf(stderr,"Unable to find the client's info\n"); 
            exit(EXIT_FAILURE);
        }
    }

    playerID = p.playerID;
    
    //Search through our flags, and send the client the appropriate message
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
        case START :
            sprintf(messbuf, "START,%d,%d", MAX_PLAYERS, MAX_LIVES);
            break;
        default:
            fprintf(stderr, "Invalid message\n");
            exit(EXIT_FAILURE);
    }  

    //Check if the message sends
    int err = send(client_fd, messbuf, strlen(messbuf), 0);
    if(err < 0) {
        fprintf(stderr, "Message failed to send\n");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}
 
/*
* Handles and INIT message from a client that has joined
* If the client times out before sending an INIT, we exit(EXIT_FAILURE)
* If the read fails, we also exit(EXIT_FAILURE)
* Else, we generate a new player and send them a WELCOME message along with a new playerID
* Just in case we have too many players, we have another REJECT message to send the client
*/
void handleInit(int client_fd) {

    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); 

    //Read a message from the client
    //Hangs until receives or times out
    int read = recv(client_fd, buf, BUFFER_SIZE, 0);

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, send reject message
        fprintf(stderr, "New client timed out\n");
        send_reject(client_fd);
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    else if (read < 0) {
        perror("Recv failure");
        send_reject(client_fd);
        exit(EXIT_FAILURE);
    }

    //Copy buf into initialMessage so we can print to the screen
    char * initialMessage = calloc(strlen(buf), sizeof(char));
    strcpy(initialMessage, buf);

    struct messageProperties p;
    p = parse_message(buf, -1);
    
    if(p.flag == INIT && *currPlayers < MAX_PLAYERS) {
        printf("Client %d's message: %s\n", *currPlayers + 100, initialMessage);
        generateNewPlayer(client_fd, *currPlayers);     //If client sends INIT, give them an id and send it to them
        send_message(client_fd, WELCOME);               
        return;
    } 
    else if(p.flag == INIT && *currPlayers >= MAX_PLAYERS){     //Handling of REJECT message happens here
        send_reject(client_fd);        
    }
    //If INIT is not the first message we receive, we simply kick the client
    else {
        fprintf(stderr, "INIT was not the first message received\n");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

}
//
//  headers.h
//  
//
//  Created by Kieren Underwood on 1/5/19.
//

#ifndef headers_h
#define headers_h


#endif /* headers_h */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <ctype.h>

//Global variables/data structures
#define BUFFER_SIZE  1024
#define MAX_LIVES    3        
#define TIMEOUT      5
#define MAX_PLAYERS  3


extern int *currPlayers;

extern enum {INIT, EVEN, ODD, DOUB, CON, WELCOME, START, 
             PASS, FAIL, ELIM, VICT, REJECT, CANCEL, ERR //don't know if ERR flag is needed
}  flag;

extern struct messageProperties {
    int id; 
    int flag; 
    int conChoice; 
} messageProperties;

extern struct playerInfo {
    int playerID;
    int client_fd;
    int numLives;
    bool taken;
    bool won_last_round;
    int eliminated;               //Cases for eliminated: 0 for dropped out, -1 still alive, >-1 is round elimmed
} playerInfo;

//extern struct playerInfo player1, player2, player3, player4;    Don't think we need this
extern struct playerInfo *pArray;


//Functions from parse_message
extern struct messageProperties parse_message(char *);

//Functions from play_game_round.c
extern int*                     roll_dice(void);
extern bool                     check_even(int *);
extern bool                     check_odd(int *);
extern bool                     check_doubles(int[]);
extern bool                     check_contains(int *, int);
extern bool                     check(int *, int, int);
extern void                     init_game_data(void);
extern void                     generateNewPlayer(int, int);
extern void                     playGame(void);
extern void                     playRound(int, int, int *);

//Functions from messages.c
extern void                     send_message(int, int);
extern void                     send_reject(int);
extern void                     handleInit(int);

//Functions from handleClient.c
extern void                     handleClient(int);
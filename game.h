//
//  headers.h
//  
//
//  Created by Kieren Underwood on 1/5/19.
//

#ifndef headers_h
#define headers_h


#endif /* headers_h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

//Global variables/data structures
#define BUFFER_SIZE 1024
//#define maxLives    5         If you #DEFINE this, then you are not able to change it.
#define playerCode  100
#define TIMEOUT     30
#define MAX_PLAYERS  4

extern int numLives, currPlayers;
extern int playerInfo[][5];
extern struct player1, player2, player3, player4;
//extern int idCode[900]; //May not have to use this at all...
extern enum {INIT, EVEN, ODD, DOUB, CON, WEL, START, PASS, FAIL, ELIM, VICT, REJ, CAN, ERR
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
    bool eliminated;
};

//Functions from parse_message
extern struct messageProperties parse_message(char *);

//Functions from play_game_round.c
extern int*                     roll_dice(void);
extern bool                     check_even(int *);
extern bool                     check_odd(int *);
extern bool                     check_doubles(int *);
extern bool                     check_contains(int *, int);
extern bool                     check(int *, int, int);
extern void                     init_game_data(void);
extern void                     generateNewPlayer(int, int);
extern void                     playGame(void);
extern void                     playRound(int, int);

//
//  Created by Kieren Underwood on 2/5/19.
//

#include "game.h"

//typedef enum {INIT, EVEN, ODD, DOUB, CON} flag; 
void playGame() {

    int playersLeft = MAX_PLAYERS;
    bool victorFound = false;
    int elimsSent = 0;

    struct playerInfo pArray[MAX_PLAYERS] = {player1, player2, player3, player4};

    while(!victorFound) {
    
    pid_t child_pid, wpid;
    int status = 0;

    for(int i = 0; i < MAX_PLAYERS, i++) {
        
        //WE ARE forking() HERE AGAIN. SO WE NEED TO SET UP SOME SHARED MEMORY, 
        //SUCH THAT WE CAN RETURN WHETHER OR NOT THEY WON OR LOST THE ROUND...
        if(child_pid = fork() == 0) {
            int player = pArray[i].playerID;
            int client_no = pArray[i].client_fd;
            //Should our play round return a bool here?

            //THIS PLAY ROUND HAS TO WAIT FOR THE CLIENT TO SEND A MESSAGE FIRST
            //AND THEN PROCESS THE ROUND... SO MAYBE HAVE A FUNCTION THAT JUST SITS THERE AND WAITS FOR A CLIENT 
            //TO CHOOSE AN OPTION
            playRound(player, client_no);
            exit(EXIT_SUCCESS);
        }
    }

    while((wpid = wait(&staus)) > 0 );

    //Check to see whether or not the players won the last round
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if(!pArray[i].won_last_round) {
            pArray[i].numLives--;
        }

        //Then check if they should be eliminated
        if(pArray[i].numLives < 1) {
            send_message(pArray[i].client_fd, ELIM);
            elimsSent++;
        }
    }
    
    //Check if there is a victor and send victory message if there is one
    if(elimsSent >= 3) {
        victorFound = true;
        for(int i = 0; i < MAX_PLAYERS; i++) {
            if(!pArray[i].eliminated) {
                send_message(pArray[i].client_fd, VICT);
            }
        }
    }

    }
}

void playRound(int player, int client_no) {
    
    //HERE WE MUST WAIT FOR THE INCOMING SIGNAL FROM THE CLIENT 

    //THEN WE PROCESS IT
    
    int * diceRoll = roll_dice();

    //DO stuff

}

int * roll_dice() {
    sleep(1);
    int max = 6;
    int* dice = calloc(2, sizeof(int));
    dice[0] = (rand() % max) + 1;
    dice[1] = (rand() % max) + 1;
    return dice;
}

bool check_even(int * dice) {
    //sum of the dice is an even number (but not doubles)
    if(dice[0] == dice[1]) {
        return false;
    }
    int sum = dice[0] + dice[1];
    return(sum % 2 == 0);
}

bool check_odd(int * dice) {
    //sum of the dice is odd number that is > 5
    int sum = dice[0] + dice[1];
    return(sum > 5 && (sum % 2 == 1));
}

bool check_doubles(int * dice) {
    //two of the same numbers
    return(dice[0] == dice[1]);
}

bool check_contains(int * dice, int n) {
    //at least one dice is n
    return(dice[0] == n || dice[1] == n);
}

bool check(int * dice, int flag, int con_choice) {
    switch(flag) {
        case EVEN :
            return check_even(dice);
        case ODD :
            return check_odd(dice);
        case DOUB :
            return check_doubles(dice);
        case CON :
            return check_contains(dice, con_choice);
        default :
            printf("Get fucked");
            exit(EXIT_FAILURE);
    }
}

/*
* This function is called at the start of the program
* It makes 4 ID numbers: 100, 101, 102, 103
* These can then be attached to our client_fd's
* playerInfo[0] contains ID number
* playerInfo[1] contains client_fd
*/
void init_game_data() {
    player1.playerID = 100
    player1.numLives = numLives;
    player1.taken = false;
    player1.eliminated = false;

    player2.playerID = 100
    player2.numLives = numLives;
    player2.taken = false;
    player2.eliminated = false;

    player3.playerID = 100
    player3.numLives = numLives;
    player3.taken = false;
    player3.eliminated = false;

    player4.playerID = 100
    player4.numLives = numLives;
    player4.taken = false;
    player4.eliminated = false;
}


void generateNewPlayer(int client_fd, currPlayers) {
    
    switch(currPlayers) {
        case 0:
            player1.client_fd = client_fd;
            player1.taken = true;
        case 1:
            player2.client_fd = client_fd;
            player2.taken = true;
        case 2:
            player3.client_fd = client_fd;
            player3.taken = true;
        case 3:
            player4.client_fd = client_fd;
            player4.taken = true;
        default :
            fprintf(stderr, "Too many players have been accepted");
    }
}

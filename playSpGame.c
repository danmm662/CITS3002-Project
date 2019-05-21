#include "game.h" 

//For playing game with a single player, implementing tier 1
void playSpGame(void) {

	struct playerInfo p = pArray[0];
	struct messageProperties mess;

	char *buf;
	int *diceRoll = calloc(2, sizeof(int));
	int round = 0;

	send_message(p.client_fd, START);

	while(true) {

		sleep(1);

		diceRoll = roll_dice();

		buf = calloc(BUFFER_SIZE, sizeof(char));

		int read = recv(p.client_fd, buf, BUFFER_SIZE, 0);

		round++;
		printf("\nRound %d:\n", round);
		printf("Dice roll is %d,%d\n", diceRoll[0], diceRoll[1]);

		if (read == 0) {
			printf("Player dropped out\n");
			p.eliminated = 0;
		}
		else if (read < 0 && errno == EAGAIN) {         //Timeout for client's move
			printf("Player timed out\n");
			p.won_last_round = false;       //Set to false so they lose a life                                  
		}
		else if (read < 0){                          
			fprintf(stderr, "Client read failed\n");
			p.eliminated = 0;
		}
		else {						//Normal case, successful read from client
			mess = parse_message(buf, p.client_fd);

			if(mess.flag == ERR) {             //Lose a life for an invalid move
				printf("Player made an invalid move\n");
				p.won_last_round = false;
			}


			bool checkedGuess = check(diceRoll, mess.flag, mess.conChoice);

			if (checkedGuess) {
				printf("Player made a correct guess\n");
				p.won_last_round = true;
			} 
			else {
				printf("Player made an incorrect guess\n");
				p.won_last_round = false;
			}
		}  

		if (p.won_last_round) {
			send_message(p.client_fd, PASS);
		}
		else {
			send_message(p.client_fd, FAIL);
			p.numLives--;
		}

		if (p.numLives < 1) {			//Normal case, when player runs out of lives
			printf("Player lost all lives, eliminated\n");
			send_message(p.client_fd, ELIM);
			close(p.client_fd);
			return;
		}
		else if (p.eliminated == 0) {	//For when player drops out
			close(p.client_fd);
			return;
		}

	}

}
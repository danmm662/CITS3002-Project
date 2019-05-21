#include "game.h" 

//Determines what type of message is received, checks the user id,
//returns the message type and int choice if it is the contains choice

struct messageProperties parse_message(char *s){
	
	//Initialise the properties structure, sets default values for it
	struct messageProperties properties;
	properties.id = -1;
	properties.flag = ERR;
	properties.conChoice = 0; 

	char *buf1;
	buf1 = calloc(BUFFER_SIZE, sizeof(char));

	if (strcmp(s, "INIT") == 0) {
		properties.flag = INIT;
	}
	else {  
		// need to first check if the first 3 characters of received message is a valid user id
		
		int id;
		buf1 = strtok(s, ",");
		if(buf1 == NULL) {			//Make sure message contains commas
			return properties;	
		}

		id = atoi(buf1);  //This will cause an error if buf1 is not an int, may need to test for that first

		//Check if id provided is a valid id or not, tell client if it isn't
		//Need to modify this so it checks that the playerID and the client_fd match as well
		for(int i = 0; i < MAX_PLAYERS; i++){
			if(pArray[i].playerID == id) {
				break;
			}
			else if(i == (MAX_PLAYERS - 1)) {	//If id isn't found, exit out
				fprintf(stderr, "Unable to find client's id\n");
				exit(EXIT_FAILURE);
			}
		}
		
		properties.id = id;

		buf1 = strtok(NULL, ",");
		if(buf1 == NULL) {				//Make sure message contains commas
			return properties;	
		}

		if(strcmp(buf1, "MOV") != 0){ 	//Checks that message contains MOV, returns the struct with the
			return properties;			//ERR flag if it doesn't
		}

		buf1 = strtok(NULL, ",");
		if(buf1 == NULL) {				//Make sure message contains commas
			return properties;	
		}

		if(strcmp(buf1, "EVEN") == 0){		
			properties.flag = EVEN;			
		}
		else if(strcmp(buf1, "ODD") == 0){
			properties.flag = ODD;
		}
		else if(strcmp(buf1, "DOUB") == 0){
			properties.flag = DOUB;
		}
		else if(strcmp(buf1, "CON") == 0){

			buf1 = strtok(NULL, ",");
			if(buf1 == NULL) {			 //Make sure message contains commas
				return properties;	
			}
			
			if (!isdigit(buf1[0])) {	 //Makes sure that it is actually a digit before calling atoi()
				return properties;
			}

			int num = atoi(buf1);
			
			if(num < 1 || num > 6) {		//Returns the properties struct with the ERR flag if invalid number
				return properties;
			}
			properties.flag = CON;
			properties.conChoice = num;			
		}
	}	
	return properties;
}
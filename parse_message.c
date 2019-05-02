#include "game.h" 

//Determines what type of message is received, checks the user id,
//returns the message type and int choice if it is the contains choice

struct messageProperties parse_message(char *s){
	
	//Initialise the properties structure, set default values for it
	struct messageProperties properties;
	properties.id = -1;
	properties.flag = ERR;
	properties.conChoice = 0;

	if(strcmp(s, "INIT") == 0){
		properties.flag = INIT;
	}
	else {  
		// need to first check if the first 3 characters of received message is a valid user id
		char *buf;
		buf = calloc(sizeof(s), sizeof(char));
		int id;
		buf = strtok(s, ",");

		/*if(isnumber(buf) != 0){
			//Tell client that they must provide their 3 digit id at start of their message
		}*/

		id = atoi(buf);

		//Check if id provided is a valid id or not, tell client if it isn't
		/*if(!isValidId(id)){
			tell client their id is invalid
		}*/
		
		properties.id = id;

		buf = strtok(NULL, ",");

		if(strcmp(buf, "MOV") != 0){
			//tell client that message is invalid, must have MOV after id
		}

		buf = strtok(NULL, ",");

		if(strcmp(buf, "EVEN") == 0){		//Can't use switches with strings, maybe another way to check what move the client used?
			properties.flag = EVEN;
		}
		else if(strcmp(buf, "ODD") != 0){
			properties.flag = ODD;
		}
		else if(strcmp(buf, "DOUB") != 0){
			properties.flag = DOUB;
		}
		else if(strcmp(buf, "CON") != 0){
			properties.flag = CON;
			
			buf = strtok(NULL, ",");
			int num = atoi(buf);
			
			if(num < 1 || num > 6){
				//let client know they picked an invalid dice number
			}
			properties.conChoice = num;			
		}
		else {
			properties.flag = ERR;
		}
	}	
	return properties;
}
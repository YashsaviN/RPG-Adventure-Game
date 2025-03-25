#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//-----------------------------------------
// NAME: Yashsavi Negi 
// STUDENT NUMBER: 7995962
// COURSE: COMP 2160, SECTION: A01
// INSTRUCTOR: Tim Zapp
// ASSIGNMENT: assignment 2, QUESTION: question 1
// 
// REMARKS: this code designs a game where it takes input from the users and on roads it encounters strangers which the user can join to talk with or fight with while the strangers also have choice of fighting or joining the party. The character fights until the enemy or the whoel party has died. When encountering a town, they all heal. This goes on until the encounters are all done or the whole aprty has died. 
//
//-----------------------------------------

#define BUFFER 256
#define MAX_PLAYER 100
//Structure to define a player in the game
//player details
typedef struct
{
	char name[256];  
	int currHealth;
	int maxHealth;
	int damage;
}Player;

//Structure to define encounters in the game
typedef struct
{
	char type[10]; // town or road
	int time; // time that will help in sorting
	char detail[256]; 
	char name[256];   // For road encounters: name of teh stranger
    int health; // health of the stranger
    int damage; // damage of the stranger
}Encounter;

// all Functions prototypes
Encounter* readfile(int *encounterCount);
void sortEncounter(Encounter encounterList[], int size);
Player playerCreation();
void displayParty(Player *party, int partySize);
int encountering(Encounter encounter, Player *player, Player *party, int *partySize);
int addStranger(Player *party, int *partySize, Encounter encounter);
void removeMember(Player *party, int *partySize, int memberNum);
int fight(Player *party, int *partySize, Encounter encounter);

int main(){
	srand(time(NULL)); // Generating random numbers
	printf("Hello! Welcome to Generic Adventure Game (TM)! ");

	//Create the main player
	Player player = playerCreation();
	Player party[MAX_PLAYER] = {player};
	party[0] = player; // the first index will always be the main charcter
	int partySize = 1; // starting with the main character in the party

	displayParty(party, partySize); // Printing the players in the party

	// Reading the encounters
	int encounterCount = 0;
	Encounter *encounterList = readfile(&encounterCount);

	//sorting the encounters by their time
	sortEncounter(encounterList, encounterCount);

	// Processing the encounters and generating the game
	for (int i = 0; i < encounterCount; i++) {
        if(encountering(encounterList[i], &player, party, &partySize) == 1){
			printf("Unfortunately, your journey tragically comes to an end early, with your death. \n");
			free(encounterList);
			return 0;
		}
    }
	free(encounterList); // Free the memeory allocated for encounters
	return 0;
}

// Reads encounters from config.txt and split the line into their respective variables 
Encounter* readfile(int *encounterCount){
	FILE *f = fopen("config.txt", "r");
	char line[BUFFER];

	// Checking if the file exists or not
	if (f == NULL)
	{
		printf("file not found\n");
		return NULL;
	}
	
	//Allocating memory by getting the number of encounters
	*encounterCount = 0; 
	while (fgets(line, BUFFER, f) != NULL)
	{
		(*encounterCount)++;
	}
	Encounter *encounterList = (Encounter*)malloc((*encounterCount)*sizeof(Encounter));
	fclose(f);
	
	// Reopening the file to read the actual data inside
	// can't use the same fgets as it already reached the last line of the txt file.
	f = fopen("config.txt", "r");
	int index = 0; 
	while (fgets(line, BUFFER, f) != NULL)
	{
		int lastCharIndex = strlen(line) - 1;
		if (line[lastCharIndex] == '\n')
		{
			line[lastCharIndex] = '\0';
		}
		Encounter encounter;
        
        // Tokenize the first two parts: type and time
        char *token = strtok(line, " ");
        strcpy(encounter.type, token); // Copying type

        token = strtok(NULL, " "); // Get time
        encounter.time = atoi(token); // Converting it to integer

        // Now process the remaining details
        token = strtok(NULL, "\n"); // Get the rest of the line

        if (strcmp(encounter.type, "town") == 0) {
            strcpy(encounter.detail, token); // Store town name
        } 
        else if (strcmp(encounter.type, "road") == 0) {
            // Tokenize road encounter details (name, health, damage)
            char *name = strtok(token, " ");
            char *healthStr = strtok(NULL, " ");
            char *damageStr = strtok(NULL, " ");

			// Converting it to integers and copying the name
            if (name && healthStr && damageStr) {
                strcpy(encounter.name, name);
                encounter.health = atoi(healthStr);
                encounter.damage = atoi(damageStr);
            }
        }

        // Add encounter to list
        encounterList[index] = encounter;
        index++;
    }
	fclose(f); // closing the file to avoid memory leaks

	return encounterList;
}

// Function to sort the enounters by their time by using the bubble sort
void sortEncounter(Encounter encounterList[], int size){
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < size - i - 1; j++)
		{
			if (encounterList[j].time > encounterList[j + 1].time)
			{
				Encounter temp = encounterList[j];
				encounterList[j] = encounterList[j + 1];
				encounterList[j + 1] = temp;
			}
		}
	}
}

// Function to create the main character
Player playerCreation(){
	printf("Please enter your name: ");
	Player player;
	fgets(player.name, sizeof(player.name), stdin);
	int lastCharIndex = strlen(player.name) - 1;
	// removing the new line and replacing it with the null terminator
	if (player.name[lastCharIndex] == '\n')
	{
		player.name[lastCharIndex] = '\0';
	}
	// Main character details
	player.currHealth = 50;
	player.maxHealth = 50;
	player.damage = 6;

	return player;
}

// Function to print the party memebers
void displayParty( Player *party, int partySize){
	printf("Your current party:\n");
	for(int i =0; i< partySize; i++){
		printf("Party member %d: %s, health: %d/%d, damage: %d\n", i + 1, party[i].name, party[i].currHealth, party[i].maxHealth, party[i].damage);
	}
	printf("\n");
}
// function to process the encounters and act accordingly, for town, the players heal and for roads they fight or add members
int encountering(Encounter encounter, Player *player, Player *party, int *partySize){
	// When they encounter a town, heal all the players to full health
	if (strcmp(encounter.type, "town") == 0)
	{
		printf("You are resting in the town of %s. All of your party members have been healed to full health. \n", encounter.detail);
		for (int i = 0; i < *partySize; i++)
		{
			party[i].currHealth = party[i].maxHealth;
		}
		displayParty(party, *partySize);
		return 0;
	}
	
	// when the encounter is a road, meet the stranger from th econfig file.
	if (strcmp(encounter.type, "road") == 0)
	{
		char strangerName[BUFFER];
		strcpy(strangerName, encounter.name); 

		printf("You have encountered a stranger, whose name seems to be %s, on the road! \n", strangerName);
		printf("Would you like to try to talk to them or fight them? Enter 1 to talk and 2 to fight.");
		
		int playerChoice;
		scanf("%d", &playerChoice);

		// stranger has the choice to choose to fight or join the party as well
		int strangerChoice = (rand()% 2) + 1;

		// if choice is 1 then the player wants to talk or else it has chosen to fight
		if (playerChoice == 1) {
            printf("You have chosen to talk ");
			if (strangerChoice == 2) {
				printf("but the stranger has chosen to fight, so a fight it is!\n");
			}
        } else {
            printf("You have chosen to fight! \n");
        }

		// if both the player and the stranger wants to talk, it means the stranger has joined the party. so both pick 1.
        if (playerChoice == 1 && strangerChoice == 1) {
            if (*partySize < 4) {
                printf("%s will join your party!\n", strangerName);
                addStranger(party, partySize, encounter);
				displayParty(party, *partySize);
            } else {
				// When party is full, the user gets the choice to remove a member and the stranger joins
                printf("Your party is full! Choose a member (2-%d) to remove \n", *partySize);
                displayParty(party, *partySize);
                int removeIndex;
                scanf("%d", &removeIndex);
                
                if (removeIndex > 0 && removeIndex <= *partySize) {
                    removeMember(party, partySize, removeIndex);
                    addStranger(party, partySize, encounter);
                    printf("%s has joined your party!\n", strangerName);
                }
            }
			return 0;
        } 
		// When the main chracter dies it will return 1 to let the main function know to end the game
        else {
            if(fight(party, partySize, encounter) == 1){
				return 1;
			}
        }
    }
	return 0; 
}

// Function to add a new character to the party
int addStranger(Player *party, int *partySize, Encounter encounter){
	// if there is space
	if (*partySize < 4){
		strcpy(party[*partySize].name, encounter.name);
        party[*partySize].currHealth = encounter.health;
        party[*partySize].maxHealth = encounter.health;
        party[*partySize].damage = encounter.damage;
		(*partySize)++; // increate the size of the party
		return 1;
	}
}

// Function to remove a character from the party
void removeMember(Player *party, int *partySize, int memberNum){
	for(int i = memberNum - 1; i < *partySize -1; i++){
		party[i] = party[i + 1]; 
	}
	(*partySize)--; 
}

// Function for generating the fight between the character and the stranger
int fight(Player *party, int *partySize, Encounter encounter) {
    // Initialize the enemy
    Player enemy;
    strcpy(enemy.name, encounter.name);
    enemy.currHealth = encounter.health;
    enemy.maxHealth = encounter.health;
    enemy.damage = encounter.damage;

    while (enemy.currHealth > 0){
		// choosing the member to fight and doing this until the stranger hasn't died
		printf("Which of your party members would you like to attack the enemy? \n");
        displayParty(party, *partySize);
		printf("Enter any number 1 - %d\n", *partySize);
        int choice;
        scanf("%d", &choice);

        // Get the selected party member
        Player *attacker = &party[choice - 1];

        // Attacker deals damage
        int damageDealt = rand() % attacker->damage + 1;
        enemy.currHealth -= damageDealt;
        printf("%s attacks %s for %d damage!\n", attacker->name, enemy.name, damageDealt);

        // Check if the enemy is defeated
        if (enemy.currHealth <= 0) {
            printf("You have defeated %s!\n\n", enemy.name);
            return 0;
        }

        // Enemy retaliates
        int enemyDamage = rand() % enemy.damage + 1;
        attacker->currHealth -= enemyDamage;
        printf("%s deals %d damage to %s!\n", enemy.name, enemyDamage, attacker->name);

        // Check if the attacked party member dies
        if (attacker->currHealth <= 0) {
            printf("%s has died :( \n", attacker->name);
            
            // If the main player dies, end the game
            if (choice == 1) {
				return 1; 
            } else {
                // Remove the defeated member from the party
                removeMember(party, partySize, choice);
                printf("%s has been removed from the party.\n", attacker->name);
            }
        }
    }
	return 0;
}
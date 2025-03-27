#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER 256
#define MAX_PLAYER 100
#define MAX_PARTY_SIZE 4

// Structure to define a player in the game
typedef struct {
    char name[BUFFER];  
    int currHealth;
    int maxHealth;
    int damage;
} Player;

// Structure to define encounters in the game
typedef struct {
    char type[10]; // town or road
    int time; // time for sorting
    char detail[BUFFER]; 
    char name[BUFFER]; // For road encounters: name of the stranger
    int health;
    int damage;
} Encounter;

// Function prototypes
Encounter* readfile(int *encounterCount);
void sortEncounter(Encounter encounterList[], int size);
Player playerCreation();
void displayParty(Player *party, int partySize);
bool encountering(Encounter encounter, Player *player, Player *party, int *partySize);
bool addStranger(Player *party, int *partySize, Encounter encounter);
void removeMember(Player *party, int *partySize, int memberNum);
bool fight(Player *party, int *partySize, Encounter encounter);

int main() {
    srand(time(NULL)); // Random seed
    printf("Hello! Welcome to Generic Adventure Game (TM)!\n");

    // Create main player
    Player player = playerCreation();
    Player party[MAX_PLAYER] = {player};
    int partySize = 1;
    displayParty(party, partySize);

    // Read and sort encounters
    int encounterCount = 0;
    Encounter *encounterList = readfile(&encounterCount);
    if (!encounterList) {
        printf("No encounters loaded. Exiting...\n");
        return 1;
    }
    sortEncounter(encounterList, encounterCount);

    // Process encounters
    for (int i = 0; i < encounterCount; i++) {
        if (encountering(encounterList[i], &player, party, &partySize)) {
            printf("Game Over.\n");
            free(encounterList);
            return 0;
        }
    }
    free(encounterList);
    return 0;
}

// Function to read encounters from file
Encounter* readfile(int *encounterCount) {
    FILE *f = fopen("config.txt", "r");
    if (!f) {
        printf("Error: File not found\n");
        return NULL;
    }
    
    // Count lines
    char line[BUFFER];
    *encounterCount = 0;
    while (fgets(line, BUFFER, f)) (*encounterCount)++;
    rewind(f);

    Encounter *encounterList = malloc((*encounterCount) * sizeof(Encounter));
    if (!encounterList) {
        printf("Memory allocation failed\n");
        fclose(f);
        return NULL;
    }
    
    int index = 0;
    while (fgets(line, BUFFER, f)) {
        Encounter encounter;
        sscanf(line, "%s %d %[^]", encounter.type, &encounter.time, encounter.detail);
        if (strcmp(encounter.type, "road") == 0) {
            sscanf(encounter.detail, "%s %d %d", encounter.name, &encounter.health, &encounter.damage);
        }
        encounterList[index++] = encounter;
    }
    fclose(f);
    return encounterList;
}

// Sort encounters using Bubble Sort
void sortEncounter(Encounter encounterList[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (encounterList[j].time > encounterList[j + 1].time) {
                Encounter temp = encounterList[j];
                encounterList[j] = encounterList[j + 1];
                encounterList[j + 1] = temp;
            }
        }
    }
}

// Function to create the main character
Player playerCreation() {
    Player player;
    printf("Enter your name: ");
    fgets(player.name, sizeof(player.name), stdin);
    player.name[strcspn(player.name, "\n")] = '\0';
    player.currHealth = 50;
    player.maxHealth = 50;
    player.damage = 6;
    return player;
}

// Function to display the party
void displayParty(Player *party, int partySize) {
    printf("Your current party:\n");
    for (int i = 0; i < partySize; i++) {
        printf("%d: %s (Health: %d/%d, Damage: %d)\n", i + 1, party[i].name, party[i].currHealth, party[i].maxHealth, party[i].damage);
    }
}

// Function to process encounters
bool encountering(Encounter encounter, Player *player, Player *party, int *partySize) {
    if (strcmp(encounter.type, "town") == 0) {
        printf("Resting in %s. Party fully healed!\n", encounter.detail);
        for (int i = 0; i < *partySize; i++) party[i].currHealth = party[i].maxHealth;
        displayParty(party, *partySize);
        return false;
    }
    if (strcmp(encounter.type, "road") == 0) {
        printf("You encountered %s! Fight (1) or Talk (2)?\n", encounter.name);
        int choice;
        scanf("%d", &choice);
        getchar(); // Clear newline

        int strangerChoice = rand() % 2 + 1;
        if (choice == 1 || strangerChoice == 2) {
            return fight(party, partySize, encounter);
        } else if (*partySize < MAX_PARTY_SIZE) {
            return addStranger(party, partySize, encounter);
        }
    }
    return false;
}

// Function to add a new character to the party
bool addStranger(Player *party, int *partySize, Encounter encounter) {
    if (*partySize < MAX_PARTY_SIZE) {
        strcpy(party[*partySize].name, encounter.name);
        party[*partySize].currHealth = encounter.health;
        party[*partySize].maxHealth = encounter.health;
        party[*partySize].damage = encounter.damage;
        (*partySize)++;
        return false;
    }
    return false;
}

// Function to handle fights
bool fight(Player *party, int *partySize, Encounter encounter) {
    Player enemy = {"", encounter.health, encounter.health, encounter.damage};
    strcpy(enemy.name, encounter.name);

    while (enemy.currHealth > 0) {
        printf("Choose a party member to attack: \n");
        displayParty(party, *partySize);
        int choice;
        scanf("%d", &choice);
        getchar();

        Player *attacker = &party[choice - 1];
        enemy.currHealth -= attacker->damage;
        if (enemy.currHealth <= 0) return false;
        attacker->currHealth -= enemy.damage;
        if (attacker->currHealth <= 0) removeMember(party, partySize, choice);
    }
    return false;
}

void removeMember(Player *party, int *partySize, int memberNum) {
    for (int i = memberNum - 1; i < *partySize - 1; i++) {
        party[i] = party[i + 1];
    }
    (*partySize)--;
}

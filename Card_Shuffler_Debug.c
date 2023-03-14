#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

typedef struct
{
    int cardNumber;
    int cardBoeuf;
} card;

typedef struct
{
    card firstCard;
    card followingCards[104];
    int followingCardsSize;
} cardRow;

typedef struct
{
    int id;
    int robot; //1 if yes, 0 otherwise
    card cards[104];
    int bullCount;
} player;

typedef struct
{
    player * players;
} partyPlayers;

card * createDeck()
{
    srand((unsigned)time(NULL));
    static card CardShuffle[104];
    for (int i = 0; i < 104; i++)
    {
        CardShuffle[i].cardNumber = i + 1;
        CardShuffle[i].cardBoeuf = 1 + rand() % 7;
    }
    return CardShuffle;
}

card * shuffleDeck(card * cards)
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < 104; i++)
    {
        int temp1 = cards[i].cardNumber;
        int temp2 = cards[i].cardBoeuf;
        int randomIndex = rand() % 104;
        cards[i] = cards[randomIndex];
        cards[randomIndex].cardNumber = temp1;
        cards[randomIndex].cardBoeuf = temp2;
        //printf("%d:%d ", CardShuffle[i].cardNumber, CardShuffle[i].cardBoeuf); //for debugging
    }
    return cards;
}

int numberOfPlayers()
{
    int nbPlayers;
    do 
    {
        printf("Entrer le nombre de joueurs:\n");
        scanf("%d", &nbPlayers);
    } while (nbPlayers > 10);
    system("clear");
    return nbPlayers;
}

void sendNbPlayers(int nbP)
{
    //mkfifo
    int fd = open("../FIFOS/nbPlayers", O_WRONLY);
    write(fd, &nbP, sizeof(int));
    close(fd);
}

player * createPlayers(int nbPlayers) 
{
    player * players;
    players = (player *)malloc(nbPlayers * sizeof(player));
    for (int i = 0; i < nbPlayers; i++) 
    {
        players[i].id = i + 1;
        players[i].bullCount = 0;
    }
    return players;
}

card * partyCards(card * shuffleDeck)
{
    static card beginningCards[104];
    cardRow cRow;
    for (int i = 0; i < 4; i++)
    {
        beginningCards[i] = shuffleDeck[i];
        cRow.firstCard = beginningCards[i];
        cRow.followingCardsSize = 0;
    }
    return beginningCards;
}

void displayCards(card * begginningCards)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%d) %d:%d\n", i + 1, begginningCards[i].cardNumber, begginningCards[i].cardBoeuf);
    }
}

/* cardRow * partyCards(card * shuffleDeck)
{
    cardRow * cRow;
    for (int i = 0; i < 4; i++)
    {
        cRow[i].firstCard = shuffleDeck[i];
        cRow[i].followingCardsSize = 0;
    }
    return cRow;
}

void displayCards(cardRow * begginningCards)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%d) %d:%d\n", i + 1, begginningCards[i].firstCard.cardNumber, begginningCards[i].firstCard.cardBoeuf);
        for(int j = 0; j < begginningCards->followingCardsSize; j++)
        {
            printf("%d) %d:%d\n", i + 1, begginningCards[i].followingCards[j].cardNumber, begginningCards[i].followingCards[j].cardBoeuf);
        }
    }
} */

void forEachPlayer(int nb, player * players, card * cards) // for debugging
{
    int index;
    for (int i = 0; i < nb; i++)
    {
        char name_of_file[30] = "../CardFiles/";
        char temp[30];
        sprintf(temp, "Player_%d_File", i + 1);
        strcat(name_of_file, temp);
        //../CardFiles/Player_%d_File
        FILE * f1 = fopen(name_of_file, "ab"); //append
        fprintf(f1, "Player : %d\n", players[i].id);
        fclose(f1);
        for (int j = 0; j < 10; j++)
        {
            FILE * f1 = fopen(name_of_file, "ab");
            if (f1 == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            index = (4 + i * 10) + j;
            players[i].cards[j] = cards[index];
            fprintf(f1, "%d) %d:%d\n", j + 1, players[i].cards[j].cardNumber, players[i].cards[j].cardBoeuf); 
            fclose(f1);
        }
    }
}

int receiveCardChoice()
{
    int choice;
    int fd = open("../FIFOS/cardChoice", O_RDONLY);
    read(fd, &choice, sizeof(int));
    close(fd);
    return choice;
}

int receiveRowChoice()
{
    int nb;
    int fd = open("../FIFOS/rowChoice", O_RDONLY);
    read(fd, &choice, sizeof(int));
    close(fd);
    return choice;
}

/* card choiceOfCardsHumanVersion(player * players, int i)
{
    int cardChoice;
    printf("Player %d : Choose a card by its index :\n", players[i].id);  
    scanf("%d", &cardChoice);
    card chosenCard = players[i].cards[cardChoice - 1];
    printf("Chosen Card : %d:%d\n", chosenCard.cardNumber, chosenCard.cardBoeuf);
    return chosenCard;
}

int choiceOfRow()
{
    printf("Choose row :\n");  
    int rowChoice;
    scanf("%d", &rowChoice);
    return rowChoice;
}

//not working
void choiceOfCardsBotVersion(player * players, int i)
{
    srand(time(NULL));
    printf("Player %d : Choose a card by its index :\n", players[i].id);  
    int cardChoice = rand() % 9;
    card chosenCard = players[i].cards[cardChoice];
    printf("Chosen Card : %d:%d\n", chosenCard.cardNumber, chosenCard.cardBoeuf);
} */

int main(int argc, char const *argv[])
{
    card * newDeck = createDeck();
    card * shuffledDeck = shuffleDeck(newDeck);
    card * beginningCards = partyCards(shuffledDeck);
    //cardRow * beginningCards = partyCards(shuffledDeck);
    int numPlayers = numberOfPlayers();
    player * players = createPlayers(numPlayers);
    forEachPlayer(numPlayers, players, shuffledDeck);
    displayCards(beginningCards);
    sendNbPlayers(numPlayers);
    /* for (int i = 0; i < numPlayers; i++) // for debugging
    {
        printf("Player %d:\n", i + 1);
        for (int j = 0; j < 10; j++)
        {
            printf("%d:%d\n", players[i].cards[j].cardNumber, players[i].cards[j].cardBoeuf);
        }
    } */
    /* for(int i = 0; i < numPlayers * 10; i++)
    {
        for(int i = 0; i < numPlayers; i++)
        {
            if(players[i].robot == 0)
            {
                choiceOfCardsHumanVersion(players, i);
                int row = choiceOfRow();
            }
            else
            {
                choiceOfCardsBotVersion(players, i);
                int row = choiceOfRow();
            }
        }
    } */
    //choiceOfCards(numPlayers, players);
    return 0;
}


    /* for (int i = 0; i < numPlayers; i++) // for debugging
    {
        printf("Player %d:\n", i + 1);
        for (int j = 0; j < 10; j++)
        {
            printf("%d:%d\n", players[i].cards[j].cardNumber, players[i].cards[j].cardBoeuf);
        }
    } */

    /* while (1)
    {
        card rcc = receiveCardChoices();
        printf("Received card : %d:%d\n", rcc.cardNumber, rcc.cardBoeuf); 
    } */
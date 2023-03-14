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
    card * followingCards;
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
    card * CardShuffle;
    CardShuffle = (card *)malloc(104 * sizeof(card));
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
    mkfifo("../FIFOS/nbPlayers", 0777);
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

//i was here working on card rows to make it look like the one on the test.c
cardRow * createRows(card * shuffleDeck)
{
    cardRow * rows;
    rows = (cardRow *)malloc(4 * sizeof(cardRow));
    for (int i = 0; i < 4; i++)
    {
        rows[i].followingCards = (card *)malloc(6 * sizeof(card));
    }
    return rows;
}

void fillRows(cardRow * cr, card * shuffleDeck)
{
    for (int i = 0; i < 4; i++)
    {
        cr[i].followingCards[0] = shuffleDeck[i];
        cr[i].followingCardsSize = 1;
    }
}

void displayCards(cardRow * begginningCards)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%d)", i + 1);
        int size = begginningCards[i].followingCardsSize;
        for (int j = 0; j < size; j++)
        {
            printf(" %d:%d ", begginningCards[i].followingCards[j].cardNumber, begginningCards[i].followingCards[j].cardBoeuf);
        }
        printf("\n");
    }
}

int bestRow(cardRow * begginningCards)
{
    int min = begginningCards[0].followingCards[0].cardNumber;
    int row = -1;
    for (int i = 0; i < 4; i++)
    {
        int size = begginningCards[i].followingCardsSize;
        if (begginningCards[i].followingCards[size - 1].cardNumber < min)
        {
            min = begginningCards[i].followingCards[size - 1].cardNumber;
            row = i;
        }
    }
    return row;
} 

int minimum(cardRow * begginningCards)
{
    int min = begginningCards[0].followingCards[0].cardNumber;
    for (int i = 0; i < 4; i++)
    {
        if (begginningCards[i].followingCards[0].cardNumber < min)
        {
            min = begginningCards[i].followingCards[0].cardNumber;
        }
    }
    return min;
}


void insertCardInRow(cardRow * begginningCards, int row, card card)
{
    begginningCards[row].followingCardsSize++;
    begginningCards[row].followingCards[begginningCards[row].followingCardsSize - 1] = card;
    system("clear");
    displayCards(begginningCards);
}

int replaceCardInRow(cardRow * begginningCards, int row, card card)
{
    int bull = begginningCards[row].followingCards[begginningCards[row].followingCardsSize - 1].cardBoeuf;
    begginningCards[row].followingCards[begginningCards[row].followingCardsSize - 1] = card;
    system("clear");
    displayCards(begginningCards);
    return bull;
}

int countBullHeads(cardRow cr)
{
    int sum = 0;
    for (int i = 0; i < 6; i++)
    {
        sum += cr.followingCards[i].cardNumber;
    }
    return sum;
}

void forEachPlayer(int nb, player * players, card * cards, int add) // for debugging
{
    int index;
    for (int i = 0; i < nb; i++)
    {
        char name_of_file[30] = "../CardFiles/";
        char temp[30];
        sprintf(temp, "Player_%d", i + 1);
        strcat(name_of_file, temp);
        //../CardFiles/Player_%d_File
        /* FILE * f1 = fopen(name_of_file, "ab"); //append
        fprintf(f1, "Player : %d\n", players[i].id);
        fclose(f1); */
        for (int j = 0; j < 10; j++)
        {
            FILE * f1 = fopen(name_of_file, "ab");
            if (f1 == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            index = (4 + i * 10) + j + add;
            players[i].cards[j] = cards[index];
            fprintf(f1, "%d) %d:%d\n", j + 1, players[i].cards[j].cardNumber, players[i].cards[j].cardBoeuf); 
            fclose(f1);
        }
    }
}

void forEachPlayerUpdate(int i, player player, card * cards, int idx) // for debugging
{
    int index;
    char name_of_file[30] = "../CardFiles/";
    char temp[30];
    sprintf(temp, "Player_%d", i + 1);
    strcat(name_of_file, temp);
    for (int j = 0; j < 10; j++)
    {
        FILE * f1 = fopen(name_of_file, "ab");
        if (f1 == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
        index = (4 + i * 10) + j;

        player.cards[idx].cardNumber = 0;
        player.cards[idx].cardBoeuf = 0;
        fprintf(f1, "%d) %d:%d\n", j + 1, player.cards[j].cardNumber, player.cards[j].cardBoeuf); 

        player.cards[j] = cards[index];
        fprintf(f1, "%d) %d:%d\n", j + 1, player.cards[j].cardNumber, player.cards[j].cardBoeuf); 
        fclose(f1);
    }
}

void clearFiles(int i)
{
    char name_of_file[30] = "../CardFiles/";
    char temp[30];
    sprintf(temp, "Player_%d", i);
    strcat(name_of_file, temp);
    remove(name_of_file);
}

int receiveCardChoice()
{
    int choice;
    int fd = open("../FIFOS/cardChoice", O_RDONLY);
    read(fd, &choice, sizeof(int));
    close(fd);
    return choice;
}

int main(int argc, char const *argv[])
{
    card * newDeck = createDeck();
    card * shuffledDeck = shuffleDeck(newDeck);
    cardRow * beginningCards = createRows(shuffledDeck);
    fillRows(beginningCards, shuffledDeck);
    int numPlayers = numberOfPlayers();
    player * players = createPlayers(numPlayers);
    forEachPlayer(numPlayers, players, shuffledDeck, 0);
    displayCards(beginningCards);
    sendNbPlayers(numPlayers);
    while (1)
    {
        for (int k = 0; k < numPlayers * 10; k++)
        {
            for (int i = 0; i < numPlayers; i++)
            {
                if (players[i].bullCount >= 66) //arret de jeu
                {
                    printf("Player %d lost!\n", players[i].id);
                    exit(0);
                }
                else 
                {
                    int cChoice = receiveCardChoice();
                    printf("Received Card Choice : %d\n", cChoice);
                    card chosenCard = players[i].cards[cChoice - 1];
                    int rChoice = bestRow(beginningCards);
                    int min = minimum(beginningCards);
                    if (chosenCard.cardNumber > min)
                    {
                        insertCardInRow(beginningCards, rChoice, chosenCard);
                    }
                    else
                    {
                        players[i].bullCount += replaceCardInRow(beginningCards, rChoice, chosenCard);
                    }
                    clearFiles(i + 1);
                    forEachPlayerUpdate(i, players[i], shuffledDeck, cChoice);
                }
            }
        }
        forEachPlayer(numPlayers, players, shuffledDeck, numPlayers * 10);
    }
    return 0;
}


//if followingCardsSize == 6 ==> players[i].bullCount = la somme
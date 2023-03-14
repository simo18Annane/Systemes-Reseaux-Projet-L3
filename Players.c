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
    int id;
    int robot;
    card cards[10];
    int bullHeadCollected;
} player;

void visualizeCards(int i)
{
    char name_of_file[30] = "../CardFiles/";
    char temp[30];
    sprintf(temp, "Player_%d", i + 1);
    strcat(name_of_file, temp);
    FILE *fp = fopen(name_of_file, "r");    
    if(fp == NULL)
    {
        perror("Unable to open file!");
        exit(1);
    }
    
    char * chunk = (char*)malloc(100 * sizeof(char));
    
    fputs(temp, stdout);
    fputs("\n", stdout);
    while(fgets(chunk, sizeof(chunk), fp) != NULL) {
        fputs(chunk, stdout);
    }
    fputs("\n", stdout);
    
    fclose(fp);
}

int humanOrBot()
{
    char c;
    printf("Are you a bot ? y/n\n");
    scanf("%c", &c);
    if (c == 121)
    {
        printf("You are a bot\n");
        return 1;
    }
    else if (c == 110)
    {
        printf("You are a human\n");
        return 0;   
    }
    system("clear");
}

void choiceOfCardAndRow()
{
    printf("Choose a card by it's index\n");
    int index0;
    do 
    {
        scanf("%d", &index0);
    } while(index0 > 10);
    mkfifo("../FIFOS/cardChoice", 0777);
    int fd0 = open("../FIFOS/cardChoice", O_WRONLY);
    write(fd0, &index0, sizeof(int));
    close(fd0);
}

void choiceOfCardAndRowBot()
{
    printf("Choose a card by it's index\n");
    int index0 = 1 + rand() % 10;
    printf("Bot chosen index : %d\n", index0);
    mkfifo("../FIFOS/cardChoice", 0777);
    int fd0 = open("../FIFOS/cardChoice", O_WRONLY);
    write(fd0, &index0, sizeof(int));
    close(fd0);
    getchar();
}

void updateCards(int i)
{
    system("clear");
    char name_of_file[30] = "../CardFiles/";
    char temp[30];
    sprintf(temp, "Player_%d", i + 1);
    strcat(name_of_file, temp);
    FILE *fp = fopen(name_of_file, "r");    
    if(fp == NULL)
    {
        perror("Unable to open file!");
        exit(1);
    }
    
    char * chunk = (char*)malloc(100 * sizeof(char));
    
    fputs(temp, stdout);
    fputs("\n", stdout);
    while(fgets(chunk, sizeof(chunk), fp) != NULL) {
        fputs(chunk, stdout);
    }
    fputs("\n", stdout);
    
    fclose(fp);
}

int main(int argc, char const * argv[])
{
    srand((unsigned)time(NULL));
    int nb = atoi(argv[1]);
    visualizeCards(nb);
    int bot = humanOrBot();
    if (bot == 0) 
    {
        for (int i = 0; i < 10; i++)
        {
            choiceOfCardAndRow();
            updateCards(nb);
        }
    }
    else if (bot == 1)
    {
        for (int i = 0; i < 10; i++)
        {
            choiceOfCardAndRowBot();
            updateCards(nb);
        }
    }
}
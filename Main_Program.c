#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>

/* int numberOfPlayers()
{
    int nbPlayers;
    do 
    {
        printf("Entrer le nombre de joueurs:\n");
        scanf("%d", &nbPlayers);
    } while (nbPlayers > 10);

    return nbPlayers;
}
 */

int receiveNbPlayers()
{
    int nb;
    int fd = open("../FIFOS/nbPlayers", O_RDONLY);
    read(fd, &nb, sizeof(int));
    close(fd);
    return nb;
}

int main(int argc, char const *argv[])
{
    int pid = fork();
    if (pid == 0)
    {
        int nbPlayers = receiveNbPlayers();
        for (int i = 0; i < nbPlayers; i++)
        {
            char temp1[100];
            sprintf(temp1, "gnome-terminal --command=\"./Players %d\" ", i);
            system(temp1);
        }
        wait(NULL);
    }
    else 
    {
        system("gnome-terminal --command=\"./Card_Shuffler\" "); 
    }
    return 0;
}
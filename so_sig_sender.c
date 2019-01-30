#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

//crear signal handler
void sig_handler(int signal){
    fprintf(stdout, "ID del proceso: %d\n", signal);
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Uso: <programa> <numero de sensores>\n");
        exit(EXIT_FAILURE);
    }

    int num_procesos = 0;
    num_procesos = (atoi)(argv[1]);

    /*Create as many processes as number of sensors there are*/
    pid_t cpid, w, pids[num_procesos];
    int i;
    char buf[32];
    char buf2[32];

    for(i = 0; i < num_procesos; i++){
        sprintf(buf, "gnome-terminal -- ./sensor %d %d", (i+1), (i+1)*100);
        sprintf(buf2, "gnome-terminal -- ./lector %d %d", (i+1), (i+1)*100);

        system(buf);
        system(buf2);
    }

    return 0;
}

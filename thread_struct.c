#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SHMSZ 27
#define PI 3.14159265

/*Creation of three structures to send necessary values to threads to work on respective functions
-distance reading, angle reading and real distance calculation*/

typedef struct val_struct_dist{
    char val_tmpd[SHMSZ];
    char *val_shmd;
}val_dist;

typedef struct val_struct_angle{
    char val_tmpt[SHMSZ];
    char *val_shmt;
    char val_oldt[SHMSZ];
}val_angle;

typedef struct val_struct_calculo{
    char *distance;
    char *angle;
    char val_oldd[SHMSZ];
    float resultado;
}val_calcular;

/*Declaration of global variables*/

int shmidd,shmidt;
key_t keyd,keyt;
char *shmd, *shmt;

clock_t init_time, end_time;
double elapsed_time;
double avg_time;
int counter = 0;

/*constantly reads a distance value on shared memory shmd, saves a copy of shmd on to tmpd*/

void* leerDist(void* args){
    val_dist *arg_struct = (val_dist*)args;
    char *tmpd = arg_struct->val_tmpd;
    char *shmd = arg_struct->val_shmd;
    if(strcmp(shmd,tmpd)!=0)
        fprintf(stderr,"distancia %s\n",shmd);
    strcpy(tmpd, shmd);
    //arg_struct->val_tmpd = arg_struct->val_shmd;
    pthread_exit(0);
}

/*constantly reads an angle value on shared memory shmt, saves a copy of shmt on tmpt
and stores old values of shmt on oldt*/

void* leerGiros(void* args){
    val_angle *arg_struct = (val_angle*)args;
    char *tmpt = arg_struct->val_tmpt;
    char *shmt = arg_struct->val_shmt;
    char *oldt = arg_struct->val_oldt;
    //arg_struct->val_tmpt = arg_struct->val_shmt;
    strcpy(tmpt, shmt);
    if ((strcmp(tmpt,"--")!=0)&&(strcmp(oldt,tmpt)!=0)){
			fprintf(stdout,"giroscopio: %s\n",tmpt);
			strcpy(oldt,tmpt);
            //arg_struct->val_oldt = arg_struct->val_tmpt;
        }
    pthread_exit(0);
}

void* calcDist(void* args){
    counter = counter + 1;
    //calcular la distancia cuando angulo sea dado, no calcular cuando sea "--"
    val_calcular *arg_struct = (val_calcular*)args;
    char *distance = arg_struct->distance;
    char *angle = arg_struct->angle;
    char *oldd = arg_struct->val_oldd;
    if((strcmp(angle, "--")!=0)&&(strcmp(oldd, distance)!=0)){
        strcpy(oldd, distance);
        float distancef = strtod(arg_struct->distance, NULL);
        float anglef = strtod(arg_struct->angle, NULL);
        arg_struct->resultado = distancef*cos(anglef/180*PI);
        end_time = clock();
        elapsed_time = ((double) (end_time - init_time))/CLOCKS_PER_SEC;
        fprintf(stdout, "Distancia Real: %f Elapsed time: %f\n", arg_struct->resultado, elapsed_time);
    }/*else if(strcmp(angle, "--")==0){
    }*/
    pthread_exit(0);
    
}

int main(int argc, char *argv[])
{
    char tmpd[SHMSZ];
    char oldt[SHMSZ];
    char tmpt[SHMSZ];
    //crear threads que usen estructuras que referencian a memoria compartidas
    pthread_t tids[3];

    //keyd = 1234;
    keyd = (atoi)(argv[1]);
    if ((shmidd = shmget(keyd, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return(1);
    }
    if ((shmd = shmat(shmidd, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }
    //keyt = 5678;
    keyt = (atoi)(argv[2]);
    if ((shmidt = shmget(keyt, SHMSZ,  0666)) < 0) {
        perror("shmget");
        return(1);
    }
    if ((shmt = shmat(shmidt, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(1);
    }
    
    val_dist distancia;
    distancia.val_shmd = shmd;
    //distancia.val_tmpd = "--";

    val_angle angulos;
    angulos.val_shmt = shmt;
    //angulos.val_tmpt = "";
    //angulos.val_oldt = "";

    val_calcular calcular;
    calcular.distance = shmd;
    calcular.angle = shmt;

    while(1){
        init_time = clock();
        pthread_create(&tids[0], NULL, leerDist, &distancia);
        pthread_create(&tids[1], NULL, leerGiros, &angulos);

        pthread_join(tids[0], NULL);
        pthread_join(tids[1], NULL);

        //printf("shmd: %s | tmpd: %s\n", distancia.val_shmd, distancia.val_tmpd);
        //printf("shmt: %s | tmpt: %s | oldt: %s\n", angulos.val_shmt, angulos.val_tmpt, angulos.val_oldt);

        pthread_create(&tids[2], NULL, calcDist, &calcular);
        pthread_join(tids[2], NULL);

        //printf("resultado: %f | tiempo de respuesta: %f\n", calcular.resultado, elapsed_time);
    }




    return 0;
}

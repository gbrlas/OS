#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int spremnik;
int *PRAVO;
int *ZASTAVICA;

void udji_u_KO (int i, int j);
void izadji_iz_KO (int i, int j);

void kraj (int sig) {
    //odvoji podatke
    shmdt((char *) PRAVO);
    shmdt((char *) ZASTAVICA);
    //obriši spremnik
    shmctl(spremnik, IPC_RMID, NULL);
    exit(0);
}

void posao_za_dijete(int i) {
    int j, k, m;
	
	sigset (SIGINT, kraj);
	
	//odredi koje je dijete u pitanju
    if (i == 0)
        j=1;
    else j=0;

    for (k=1; k<=5; k++) {
        udji_u_KO(i,j);
        for (m=1; m<=5; m++) {
            printf("Proces: %d, K.O. br: %d, (%d/5)\n", i+1, k, m);
            sleep(1);
        }
        izadji_iz_KO(i, j);
    }
}

//Dekkerov algoritam
void udji_u_KO (int i, int j) {
    ZASTAVICA[i] = 1;

    //radno čekanje dok je drugi proces u KO
    while (ZASTAVICA[j] != 0) {
        if (*PRAVO == j) {
            ZASTAVICA[i] = 0;
            while (*PRAVO == j);
            ZASTAVICA[i] = 1;
        }
    }
}

void izadji_iz_KO(int i, int j) {
    *PRAVO = j;
    ZASTAVICA[i] = 0;
    usleep(1);
}
//kraj Dekkerovog algoritma

int main(int argc, char *argv[]) {
    int i;

    //stvori zajednički spremnik
    if ((spremnik = shmget(IPC_PRIVATE, sizeof(int)*10, 0600)) == -1) {
    	printf("Nema dovoljno zajedničke memorije.");
        exit(1);
    }

    //dodaj varijable u spremnik i postavi vrijednosti
    PRAVO=(int *) shmat(spremnik, NULL, 0);
    *PRAVO=0;

    ZASTAVICA=(int *) shmat(spremnik, NULL, 0)+1;
    for (i=0;i<2;i++)
        ZASTAVICA[i]=0;

    //signal koji prekida sve procese
    sigset (SIGINT, kraj);

    //stvori procese koji će obavljati posao
    for (i=0; i<2; i++){
        switch(fork()){
            case -1:
                printf("Ne moze se stvoriti novi proces!\n");
                exit(0);
            case 0:
                posao_za_dijete(i);
                exit(0);
            default:
                printf("Stvoren proces: %d\n", i+1);
                break;
        }
    }

    for (i=0; i<2; i++)
        wait(NULL);
	
	//odvoji podatke
    shmdt(PRAVO);
    shmdt(ZASTAVICA);
    //obriši spremnik
    shmctl(spremnik, IPC_RMID, NULL);
    
    return 0;
}

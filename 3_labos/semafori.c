#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>

int ULAZ = 0, IZLAZ = 0;
char M[5];
char **argumenti;
int proizvodjaci;

sem_t PISI, PUN, PRAZAN;

//dretva proizvođača
void *proizvodjac (void *thread_id) {
    int id, i=0;
    
    char *dretva_argument;
	
    id = *(int *)thread_id;

    //uzmi svoj string
    dretva_argument = argumenti[id];
 	printf("%d. proizvođač: %s\n", id+1, dretva_argument);
 	sleep(1);
    
    while(1) {
        sem_wait(&PUN);
        sem_wait(&PISI);
        
        //upiši u spremnik
        M[ULAZ] = dretva_argument[i];
        ULAZ = (ULAZ+1) % 5;
        printf("PROIZVOĐAČ%d -> %c\n", id+1, dretva_argument[i]);
        
        sem_post(&PISI);
        sem_post(&PRAZAN);
        
        if (dretva_argument[i] == '\0')
        	break;
        i++;
        sleep(1);
    };

    printf("%d. proizvođač gotov.\n", id+1);

    return NULL;
}

//dretva potrošač
void *potrosac (void *thread_id) {
    int i = 0, brojac = 0, n = 0;
    char *polje;
	
    polje = (char *) malloc(sizeof(char)*1000);
	
	//dok ima podataka učitavaj ih u polje
    while(1) {
        sem_wait(&PRAZAN);
        
        if (M[IZLAZ] != '\0') {
        	polje[i] = M[IZLAZ];
        	i++;
        	n++;
        
        //jesu li svi proizvođači završili?	
        } else {
        	brojac++;
        	if (brojac == proizvodjaci)
        		break;
        }
        printf("POTROŠAČ <- %c\n", M[IZLAZ]);
        IZLAZ = (IZLAZ+1) % 5;
        
        sem_post(&PUN);
        
        sleep(1);
   	}
   	
   	//ispiši onaj zadnji '\0'
   	printf("POTROŠAČ <- %c\n", M[IZLAZ]);
   	
   	polje = (char *)realloc(polje, sizeof(char)*n);
	
    printf("Primljeno je: %s\n", polje);
    printf("Potrošač gotov.\n");
	
    free(polje);
    return 0;
}




int main (int argc, char *argv[]) {
    pthread_t *thread;
    int *thread_id;
    int i;
	
	//koliko ima proizvođača?
	proizvodjaci = argc-1;
    printf("Broj proizvođača: %d\n", proizvodjaci);
    
    sleep(1);
	
	//postavi semafore na početne vrijednosti
    sem_init(&PISI, 0, 1);
    sem_init(&PUN, 0, 5);
    sem_init(&PRAZAN, 0, 0);
   
    //alokacija memorije
    argumenti = (char **) malloc(sizeof(char *)*(argc-1));
    thread_id = (int *) malloc(sizeof(int)*argc);
    thread = (pthread_t *) malloc(sizeof(pthread_t)*(argc));
	
    for (i = 0; i<(argc-1); i++) {
        thread_id[i] = 0;
        argumenti[i] = argv[i+1];
    }
    //stvori dretve proizvođača
    for (i = 0; i<(argc-1); i++) {
    	thread_id[i] = i;
        if(pthread_create(&thread[i], NULL, proizvodjac, ((void *)&thread_id[i]))) {
            printf("Ne može se stvoriti nova dretva\n");
            exit(1);
        }
    }
    
    //stvori dretvu potrošača
    if(pthread_create(&thread[i], NULL, potrosac, ((void *)&thread_id[i]))) {
            printf("Ne može se stvoriti nova dretva\n");
            exit(1);
    }

    //čekaj da dretve završe
    for (i = 0; i<(argc-1); i++) 
        pthread_join(thread[i], NULL);
        
    pthread_join(thread[i], NULL);
	
	free(thread);
    free(argumenti);
    free(thread_id);

    return 0;
}



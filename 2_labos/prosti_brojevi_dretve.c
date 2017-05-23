#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

int *TRAZIM;
int *BROJ;
int BR_DRETVI;
int pauza = 0;
unsigned long broj = 1000000001;
unsigned long zadnji = 1000000001;

void periodicki_ispis (int sig) {
	printf("zadnji prosti broj: %lu\n", zadnji);
}

void postavi_pauzu (int sig) {
	int i;
	pauza = 1 - pauza;
	
	if (pauza) 
		for (i = 0; i < BR_DRETVI; i++) 
			printf("Dretva %d pauzira\n", i+1);
	if (!pauza)
		for (i = 0; i < BR_DRETVI; i++) 
			printf("Dretva %d nastavlja s radom\n", i+1);
}

void prekini (int sig) {
	printf("Kraj rada, zadnji prosti broj: %lu\n", zadnji);
	exit(0);
}

int prost (unsigned long n) {
	unsigned long i;
	unsigned long max;

	//nije prost
	if ((n&1) == 0)
		return 0;

	max = sqrt(n);

	for (i = 3; i <= max; i += 2) {
		if (n%i == 0)
			return 0;
	}

	//prost je
	return 1;
}

//Lamportov algoritam
void udji_u_KO (int i) {
    int max, j, x;
    max = 0;

    TRAZIM[i] = 1;
    for (x=0; x<BR_DRETVI; x++) {
        if (BROJ[x] > max)
            max = BROJ[x];
    }

    BROJ[i] = max+1;
    TRAZIM[i] = 0;

    for (j=0;j<BR_DRETVI;j++){
    	while (TRAZIM[j]!=0);

        while (BROJ[j]!=0 && (BROJ[j]<BROJ[i] || (BROJ[j]==BROJ[i] && j<i)));
    }
}

void izadji_iz_KO (int i) {
    BROJ[i] = 0;
}
//kraj Lamportovog algoritma


void *broji (void *thread_id) {
    int id, test;
    unsigned long moj;

    id = *(int *)thread_id;

    while (1) {
        while(pauza) 
        	sleep(1);
		
        udji_u_KO(id);
       	
       	//uzmi broj
        moj = broj;
        printf("Dretva %d: uzimam %lu\n", id+1, moj);
        broj++;
        izadji_iz_KO(id);

        //testiraj je li broj prost
        test = prost(moj);
        sleep(1);
	
        udji_u_KO(id);
        if (test == 1 && moj > zadnji) {
            zadnji = moj;
            printf("Dretva %d našla prosti broj %lu\n", id+1, moj);
        }
        izadji_iz_KO;
    }
}



int main (int argc, char** argv) {
	int i;
	pthread_t *thread;
	int *thread_id;
	
	BR_DRETVI = atoi(argv[1]);
	printf("Broj dretvi: %d\n", BR_DRETVI);
	sleep(2);

	//povezivanje signala sa funkcijama
	sigset(SIGINT, postavi_pauzu);
	sigset(SIGTERM, prekini);

	//postavljanje periodičkog alarma
	struct itimerval t;
	sigset (SIGALRM, periodicki_ispis);
	
	

	//slanje signala svakih 5 sekundi
	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;
	t.it_interval.tv_sec = 5;
	t.it_interval.tv_usec = 0;

	//pokretanje timera
	setitimer (ITIMER_REAL, &t, NULL);
	
	
	
    //alociraj memoriju
	thread = (pthread_t *) malloc(sizeof(pthread_t)*BR_DRETVI);
    thread_id = (int *) malloc(sizeof(int)*BR_DRETVI);
    TRAZIM = (int *) malloc(sizeof(int)*BR_DRETVI);
    BROJ = (int *) malloc(sizeof(int)*BR_DRETVI);
    
    for (i=0; i<BR_DRETVI; i++) {
    	TRAZIM[i] = 0;
    	BROJ[i] = 0;
    	thread_id[i] = 0;
    }

    //stvori dretve
	for (i=0; i<BR_DRETVI; i++) {
        thread_id[i] = i;
        if (pthread_create(&thread[i], NULL, broji, ((void *)&thread_id[i]))) {
            printf("Ne može se stvoriti nova dretva\n");
            exit(1);
        }
	}
	
    for (i=0; i<BR_DRETVI; i++)
        pthread_join(thread[i], NULL);

    //oslobodi alociranu memoriju
    free(thread);
    free(thread_id);
    free(TRAZIM);
    free(BROJ);

	return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <malloc.h>

int auti_na_mostu = 0;
int smjer_most = 2;
int *smjer_auta; // 0 = lijevo, 1 = desno

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t uvjet = PTHREAD_COND_INITIALIZER;

//monitorske funkcije
void popniSeNaMost (int smjer) {
    pthread_mutex_lock(&m);

    while ((auti_na_mostu == 3) || ((auti_na_mostu > 0) && (smjer_most != smjer)))
        pthread_cond_wait(&uvjet, &m);

    auti_na_mostu++;
    smjer_most = smjer;

    pthread_mutex_unlock(&m);

    return;
}

void sidjiSaMosta (int smjer) {
    pthread_mutex_lock(&m);

    auti_na_mostu--;

    pthread_cond_broadcast(&uvjet);
    pthread_mutex_unlock(&m);

    return;
}
//kraj monitorskih funkcija

void *Auto (void *thread_id) {
	int id, moj_smjer;
	
	id = *(int *)thread_id;
	moj_smjer = smjer_auta[id];
	
	if (moj_smjer == 0)
		printf("Auto %d čeka na prelazak preko mosta, smjer: lijevo\n", id+1);
	else
		printf("Auto %d čeka na prelazak preko mosta, smjer: desno\n", id+1);
		
	popniSeNaMost(moj_smjer);
	printf("Auto %d prelazi most\n", id+1);
	sleep(1);
	sidjiSaMosta(moj_smjer);
	printf("Auto %d je prešao most.\n", id+1);
	
	return NULL;
}

int main (int argc, char *argv[]) {
    pthread_t *thread;
    int *thread_id;
    int i, koliko, x;

    koliko = atoi(argv[1]);
    
    printf("Broj auta: %d\n", koliko);
    sleep(1);

    thread_id = (int *) malloc(sizeof(int)*koliko);
    thread = (pthread_t *) malloc(sizeof(pthread_t)*koliko);
    smjer_auta = (int *) malloc(sizeof(int)*koliko);

    srand(time(NULL));

    for (i = 0; i < koliko; i++) {
        thread_id[i] = 0;
        x = rand() % 2;
        smjer_auta[i] = x;
    }

    for (i = 0; i < koliko; i++) {
        thread_id[i] = i;
        if (pthread_create(&thread[i], NULL, Auto, ((void *)&thread_id[i]))) {
            printf("Ne može se stvoriti nova dretva\n");
            exit(1);
        }
        usleep(500000);
    }

    for (i = 0; i < koliko; i++) {
        pthread_join(thread[i], NULL);
    }
    
    printf("Svi su auti prešli preko mosta, kraj programa\n");

    free(thread_id);
    free(thread);
    free(smjer_auta);

    return 0;
}





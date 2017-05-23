#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define MAX 10000

struct at {
    int duljina;
    struct at *sljed;
    int slobodan;
};
typedef struct at zaglavlje;

int slobodni = 1;
int zauzeti = 0;
int total = 1;

zaglavlje *glava;


int oslobodi (int adresa) {
    int adresaPomocna = 0;
    zaglavlje *temp;
    zaglavlje *pomocni;

    temp = glava;

    //ako je adresa izvan granica veličine najvećeg bloka
    if (adresa > 10000 || adresa < 0)
        return 0;

    //nađi blok koji se nalazi na zadanoj adresi
    while (adresa != adresaPomocna) {
        adresaPomocna += temp->duljina;
        temp = temp->sljed;

        //ako je zadana kriva adresa
        if (adresaPomocna > adresa) {
            printf("Zadana je kriva adresa!\n");
            return 0;
        }
    }

    if (temp->slobodan == 1) {
        printf("Ne može se osloboditi već slobodan blok!\n");
        return 0;
    }


    temp->slobodan = 1;
    slobodni++;
    zauzeti--;

    //ako je samo jedan veliki program u spremniku
    if (temp == glava && temp->sljed == NULL) {
        temp->duljina = MAX;
        zauzeti--;
        slobodni++;
        return 1;
    }


    temp = glava;

    while (temp != NULL) {
        pomocni = temp->sljed;

        if (pomocni == NULL)
            break;


        //poveži susjedne slobodne
        while ((temp->slobodan == 1) && (pomocni->slobodan == 1)) {
            temp->duljina += pomocni->duljina;
            temp->sljed = pomocni->sljed;

            free(pomocni);

            slobodni--;
            total--;

            pomocni = temp->sljed;
            if (pomocni == NULL)
                break;
        }

        temp = temp->sljed;
    }

    return 1;
}

int dodijeli (int velicina) {
    int adresa = -1;
    int size;
    zaglavlje *temp;
    zaglavlje *pomocni;
    zaglavlje *novi;

    temp = (zaglavlje *) malloc(sizeof(zaglavlje));
    temp = glava;

    //nađi postoji li slobodan i dovoljno velik blok
    for (temp; temp != NULL; temp = temp->sljed) {
        if (temp->slobodan == 1 && (temp->duljina >= (velicina + sizeof(zaglavlje)/8))) {
            adresa = 1;
            break;
        }
    }

    //ako nema slobodnog bloka
    if (adresa == -1)
        return adresa;

    //ako taman stane u nađeni blok i ne može se stvoriti novi blok
    if (temp->duljina <= velicina + 2*(sizeof(zaglavlje)/8)) {
        temp->slobodan = 0;
        zauzeti++;
        slobodni--;
    } else {
    //inače postojeći blok razbijamo na dva bloka
        //inicijaliziraj novi blok i ubaci ga u postojeću listu
        novi = (zaglavlje *) malloc(sizeof(zaglavlje));
        novi->slobodan = 1;
        novi->sljed = temp->sljed;
        novi->duljina = temp->duljina - velicina - sizeof(zaglavlje)/8;

        //program ide u stari blok
        temp->slobodan = 0;
        temp->sljed = novi;
        temp->duljina = velicina + sizeof(zaglavlje)/8;

        zauzeti++;
        total++;
    }

    adresa = 0;
    //nađi konačnu adresu dodijeljenog prostora i vrati ju glavnom programu
    for (pomocni = glava; pomocni != temp; pomocni = pomocni->sljed)
        adresa += pomocni->duljina;

    return adresa;
}

int main () {
    int i, pocetak, velicina, adresa;
    int lokacija;
    char zahtjev, operacija;
    zaglavlje *temp;

    //inicijalizacija početnog bloka
    glava = (zaglavlje *) malloc(sizeof(zaglavlje));
    glava->duljina = MAX;
    glava->sljed = NULL;
    glava->slobodan = 1;
    
    printf("Velicina zaglavlja: %lu okteta\n", sizeof(zaglavlje)/8);

    while (1) {
        pocetak = 0;
        temp = glava;

        printf("\nRS: broj blokova = %d, slobodni = %d, zauzeti = %d\n", total, slobodni, zauzeti);

        for (i = 0; i < total; i++) {
            if (temp->slobodan == 1)
                printf("%d: pocetak = %d, velicina = %d, oznaka = slobodan\n", i+1, pocetak, temp->duljina);
            else
                printf("%d: pocetak = %d, velicina = %d, oznaka = zauzet\n", i+1, pocetak, temp->duljina);

            pocetak = pocetak + temp->duljina;
            temp = temp->sljed;
        }

        printf("\nUnesi zahtjev (d-dodijeli, o-oslobodi): ");
        scanf("%s", &operacija);

        switch (operacija) {
            case 'd':
                printf("Unesi velicinu programa (u oktetima): ");
                scanf("%d", &velicina);

                adresa = dodijeli(velicina);

                if (adresa != - 1)
                    printf("Dodijeljen je blok na adresi: %d\n", adresa);
                else
                    printf("Nema dovoljno memorije!\n");
                break;

            case 'o':
                printf("Unesi pocetnu adresu programa: ");
                scanf("%d", &adresa);

                if (oslobodi(adresa) == 1)
                    printf("Memorija oslobođena\n");
                else
                    printf("Greška prilikom oslobađanja memorije!\n");
                break;
            
            default:
            	printf("Niste upisali valjanu operaciju, pokušajte opet.\n");
        }
    }

    free(glava);

    return 0;
}

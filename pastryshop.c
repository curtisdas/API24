// header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#define RICETTARIO_SIZE 5000
#define MAGAZZINO_SIZE 5000

// tipi definiti

typedef struct Ordine {
    struct Ordine* next;
    struct Ricetta* ricetta;
    unsigned int istante_arrivo;
    unsigned int numero_elementi_ordinati;
    unsigned int peso_totale;
} Ordine;

typedef struct Lotto {
    struct Lotto* next;
    int quantita;
    int scadenza;
} Lotto;

typedef struct Contenitore {
    char* key;
    struct Contenitore* next;
    struct Lotto* list_lotti;
    unsigned int quantita_totale;
} Contenitore;

typedef struct Ingrediente {
    struct Ingrediente* next;
    struct Contenitore* contenitore;
    unsigned int quantita;
} Ingrediente;

typedef struct Ricetta {
    char* key;
    struct Ricetta* next;
    struct Ingrediente* list_ingredienti;
} Ricetta;

// funzioni principali

void func_spedizione();
void func_aggiungi_ricetta();
void func_rimuovi_ricetta();
void func_rifornimento();
void func_ordine();

// funzioni ausiliarie

unsigned int func_hash(char* word);
Contenitore* func_trova_crea_contenitore(char* nome);
void func_pulisci_contenitore(Contenitore* contenitore);
bool func_prepara_ordine(Ordine* ordine);
void func_merge(Ordine array[], int left, int middle, int right);
void func_merge_sort(Ordine array[], int left, int right);

// variabili globali

Ricetta* ricettario[RICETTARIO_SIZE];
Contenitore* magazzino[MAGAZZINO_SIZE];
Ordine* head_ordini = NULL;
Ordine* tail_ordini = NULL;
Ordine* head_ordini_pronti = NULL;
Ordine* tail_ordini_pronti = NULL;
unsigned int tempo = 0;
unsigned int corr_tempo, corr_peso;

// codice

int main() {

    // inizializzazione strutture dati
    
    int index;
    for(index=0; index<RICETTARIO_SIZE; index++)
        ricettario[index] = NULL;

    for(index=0; index<MAGAZZINO_SIZE; index++)
        magazzino[index] = NULL;

    // configurazione corriere

    if(scanf("%u %u", &corr_tempo, &corr_peso)<0) abort();

    // ciclo principale

    char istruzione[20];
    while(1) {

        if(tempo%corr_tempo==0 && tempo>0)
            func_spedizione();

        if(scanf("%s", istruzione) == EOF)
            break;
        if(strcmp(istruzione, "aggiungi_ricetta")==0)
            func_aggiungi_ricetta();
        else if(strcmp(istruzione, "rimuovi_ricetta")==0)
            func_rimuovi_ricetta();
        else if(strcmp(istruzione, "rifornimento")==0)
            func_rifornimento();
        else if(strcmp(istruzione, "ordine")==0)
            func_ordine();

        tempo++;
    }

    return 0;
}

void func_spedizione() {
    long int peso_disponibile = corr_peso;
    Ordine* tmp_ordine_pronto = head_ordini_pronti;
    Ordine* tmp_ordine_caricato = head_ordini_pronti;
    Ordine* free_ordine_caricato;
    int count=0, index=0;

    while(tmp_ordine_pronto != NULL && ( peso_disponibile - (long int) tmp_ordine_pronto->peso_totale)>0) {
        peso_disponibile = peso_disponibile - tmp_ordine_pronto->peso_totale;
        tmp_ordine_pronto = tmp_ordine_pronto->next;
        count++;
    }

    if(count>0) {

        // crea array ordini caricati

        head_ordini_pronti = tmp_ordine_pronto;
        Ordine* array_ordini_caricati = (Ordine*) calloc(count, sizeof(Ordine));
        while(index < count) {
            array_ordini_caricati[index] = *tmp_ordine_caricato;
            free_ordine_caricato = tmp_ordine_caricato;
            tmp_ordine_caricato = tmp_ordine_caricato->next;
            free(free_ordine_caricato);
            index++;
        }
        func_merge_sort(array_ordini_caricati, 0, count-1);
        index = 0;
        while(index < count) {
            printf("%d %s %u\n", array_ordini_caricati[index].istante_arrivo, (array_ordini_caricati[index].ricetta)->key, array_ordini_caricati[index].numero_elementi_ordinati);
            index++;
        }
        free(array_ordini_caricati);
    }
    else
        printf("camioncino vuoto\n");
}

void func_aggiungi_ricetta() {

    char nome_ricetta[256];
    if(scanf("%255s", nome_ricetta)<0) abort();

    // trova ricetta

    unsigned int index = func_hash(nome_ricetta) % RICETTARIO_SIZE;
    Ricetta* new_ricetta = ricettario[index];
    while (new_ricetta != NULL) {
        if (strcmp(new_ricetta->key, nome_ricetta)==0) {
            printf("ignorato\n");
            while(1) {
                char fine = getchar();
                if(fine=='\n') return;
            }
        }
        new_ricetta = new_ricetta->next;
    }
    
    // crea ricetta

    new_ricetta = (Ricetta*) malloc(sizeof(Ricetta));
    char* tmp_nome_ricetta = (char*) malloc(sizeof(char)*(strlen(nome_ricetta)+1));
    strcpy(tmp_nome_ricetta, nome_ricetta);
    new_ricetta->key = tmp_nome_ricetta;
    new_ricetta->next = ricettario[index];
    new_ricetta->list_ingredienti = NULL;
    ricettario[index] = new_ricetta;
    printf("aggiunta\n");

    // aggiungi ingredienti

    char nome_ingrediente[256];
    unsigned int quantita;
    Contenitore* contenitore;
    Ingrediente* new_ingrediente;

    while(1) {

        char fine = getchar();
        if(fine=='\n') return;

        if(scanf("%255s %u", nome_ingrediente, &quantita)<0) abort();

        contenitore = func_trova_crea_contenitore(nome_ingrediente);
        new_ingrediente = (Ingrediente*) malloc(sizeof(Ingrediente));
        new_ingrediente->next = new_ricetta->list_ingredienti;
        new_ingrediente->contenitore = contenitore;
        new_ingrediente->quantita = quantita;
        new_ricetta->list_ingredienti = new_ingrediente;
    }
}

void func_rimuovi_ricetta() {
    char nome_ricetta[256];
    if(scanf("%255s", nome_ricetta)<0) abort();

    // trova ricetta

    unsigned int index = func_hash(nome_ricetta) % RICETTARIO_SIZE;
    Ricetta* tmp_ricetta = ricettario[index];
    Ricetta* prev_ricetta = NULL;
    while (tmp_ricetta != NULL && strcmp(tmp_ricetta->key, nome_ricetta)!=0) {
        prev_ricetta = tmp_ricetta;
        tmp_ricetta = tmp_ricetta->next;
    }
    if(tmp_ricetta == NULL) {
        printf("non presente\n");
        return;
    }

    // trova ordini

    Ordine* tmp_ordine = head_ordini;
    while(tmp_ordine!=NULL) {
        if(strcmp(nome_ricetta, tmp_ordine->ricetta->key)==0) {
            printf("ordini in sospeso\n");
            return;
        }
        tmp_ordine = tmp_ordine->next;
    }
    tmp_ordine = head_ordini_pronti;
    while(tmp_ordine!=NULL) {
        if(strcmp(nome_ricetta, tmp_ordine->ricetta->key)==0) {
            printf("ordini in sospeso\n");
            return;
        }
        tmp_ordine = tmp_ordine->next;
    }

    // elimina ricetta

    if(prev_ricetta == NULL) 
        ricettario[index] = tmp_ricetta->next;
    else
        prev_ricetta->next = tmp_ricetta->next;

    Ingrediente* tmp_ingrediente = tmp_ricetta->list_ingredienti;
    Ingrediente* free_ingrediente;
    while(tmp_ingrediente!=NULL) {
        free_ingrediente = tmp_ingrediente;
        tmp_ingrediente = tmp_ingrediente->next;
        free(free_ingrediente);
    }
    free(tmp_ricetta->key);
    free(tmp_ricetta);
    printf("rimossa\n");
}

void func_rifornimento() {

    char nome_ingrediente[256];
    unsigned int quantita;
    unsigned int scadenza;
    Contenitore* contenitore;
    Lotto* new_lotto;
    Lotto* tmp_lotto;
    Lotto* prev_lotto;

    while(1) {

        char fine = getchar();
        if(fine=='\n') {
            printf("rifornito\n");
            break;
        }

        // leggi e crea lotto

        if(scanf("%255s %u %u", nome_ingrediente, &quantita, &scadenza)<0) abort();
        contenitore = func_trova_crea_contenitore(nome_ingrediente);
        new_lotto = (Lotto*) malloc(sizeof(Lotto));
        new_lotto->quantita = quantita;
        new_lotto->scadenza = scadenza;
        
        // aggiungi lotto

        func_pulisci_contenitore(contenitore);
        tmp_lotto = contenitore->list_lotti;
        if(tmp_lotto == NULL || tmp_lotto->scadenza > scadenza) {
            contenitore->list_lotti = new_lotto;
            new_lotto->next = tmp_lotto;
        }
        else {
            prev_lotto = tmp_lotto;
            tmp_lotto = tmp_lotto->next;
            while(tmp_lotto != NULL && tmp_lotto->scadenza <= scadenza) {
                prev_lotto = tmp_lotto;
                tmp_lotto = tmp_lotto->next;
            }
            prev_lotto->next = new_lotto;
            new_lotto->next = tmp_lotto;
        }
        contenitore->quantita_totale = contenitore->quantita_totale + quantita;
    }

    // controlla coda ordini

    Ordine* tmp_ordine = head_ordini;
    Ordine* prev_ordine = NULL;
    Ordine* new_ordine_pronto;
    Ordine* tmp_ordine_pronto = head_ordini_pronti;
    Ordine* prev_ordine_pronto = NULL;
    while(tmp_ordine!=NULL) {
        if(func_prepara_ordine(tmp_ordine)) {
            new_ordine_pronto = tmp_ordine;
            if(prev_ordine==NULL) {
                tmp_ordine = tmp_ordine->next;
                head_ordini = tmp_ordine;
            }
            else {
                tmp_ordine = tmp_ordine->next;
                prev_ordine->next = tmp_ordine;
            }
            while(tmp_ordine_pronto!=NULL && new_ordine_pronto->istante_arrivo > tmp_ordine_pronto->istante_arrivo) {
                prev_ordine_pronto = tmp_ordine_pronto;
                tmp_ordine_pronto = tmp_ordine_pronto->next;
            }
            if(prev_ordine_pronto==NULL) {
                new_ordine_pronto->next = head_ordini_pronti;
                head_ordini_pronti = new_ordine_pronto;
                prev_ordine_pronto = new_ordine_pronto;
                tmp_ordine_pronto = new_ordine_pronto->next;
            }
            else {
                new_ordine_pronto->next = tmp_ordine_pronto;
                prev_ordine_pronto->next = new_ordine_pronto;
                prev_ordine_pronto = new_ordine_pronto;
            }
        } else {
            prev_ordine = tmp_ordine;
            tmp_ordine = tmp_ordine->next;
        }
    }
    if(tmp_ordine==NULL) tail_ordini = prev_ordine;
    if(tmp_ordine_pronto==NULL) tail_ordini_pronti = prev_ordine_pronto;
}

void func_ordine() {

    // leggi ordine

    char nome_ricetta[256];
    unsigned int numero_elementi_ordinati;  
    if(scanf("%255s %u", nome_ricetta, &numero_elementi_ordinati)<0) abort();

    // trova ricetta

    unsigned int index = func_hash(nome_ricetta) % RICETTARIO_SIZE;
    Ricetta* tmp_ricetta = ricettario[index];
    while (tmp_ricetta != NULL && strcmp(tmp_ricetta->key, nome_ricetta)!=0)
        tmp_ricetta = tmp_ricetta->next;
    if(tmp_ricetta == NULL) {
        printf("rifiutato\n");
        return;
    }

    // crea ordine

    Ordine* new_ordine = (Ordine*) malloc(sizeof(Ordine));
    new_ordine->next = NULL;
    new_ordine->ricetta = tmp_ricetta;
    new_ordine->istante_arrivo = tempo;
    new_ordine->numero_elementi_ordinati = numero_elementi_ordinati;
    new_ordine->peso_totale = 0;

    printf("accettato\n");

    // prepara o accoda ordine

    if(func_prepara_ordine(new_ordine)) {
        if(head_ordini_pronti == NULL)
            head_ordini_pronti = new_ordine;
        else
            tail_ordini_pronti->next = new_ordine;
        tail_ordini_pronti = new_ordine;
    }
    else {
        if(head_ordini == NULL)
            head_ordini = new_ordine;
        else
            tail_ordini->next = new_ordine;
        tail_ordini = new_ordine;
    }
}

unsigned int func_hash(char* string) {
    unsigned int hash = 0;
    int i;
    int len = strlen(string);
    for(i=0; i<len; i++) {
        hash += string[i];
        hash *= 7;
    }
    return hash;
}

Contenitore* func_trova_crea_contenitore(char* nome_contenitore) {

    // trova contenitore

    unsigned int index = func_hash(nome_contenitore) % MAGAZZINO_SIZE;
    Contenitore* tmp_contenitore = magazzino[index];
    while (tmp_contenitore != NULL) {
        if (strcmp(tmp_contenitore->key, nome_contenitore)==0) {
            return tmp_contenitore;
        }
        tmp_contenitore = tmp_contenitore->next;
    }

    // crea contenitore

    tmp_contenitore = (Contenitore*) malloc(sizeof(Contenitore));
    char* tmp_nome_contenitore = (char*) malloc(sizeof(char)*(strlen(nome_contenitore)+1));
    strcpy(tmp_nome_contenitore, nome_contenitore);
    tmp_contenitore->key = tmp_nome_contenitore;
    tmp_contenitore->next = magazzino[index];
    tmp_contenitore->list_lotti = NULL;
    tmp_contenitore->quantita_totale = 0;
    magazzino[index] = tmp_contenitore;
    return tmp_contenitore;
}

void func_rimuovi_ingredienti(Ordine* ordine) {
    Lotto* tmp_lotto;
    Lotto* free_lotto;
    Ingrediente* tmp_ingrediente;
    Contenitore* tmp_contenitore;
    int tmp_quantita_totale;
    tmp_ingrediente = ordine->ricetta->list_ingredienti;
    while(tmp_ingrediente!=NULL) {

        tmp_contenitore = tmp_ingrediente->contenitore;
        tmp_quantita_totale = ordine->numero_elementi_ordinati * tmp_ingrediente->quantita;
        ordine->peso_totale += tmp_quantita_totale;
        tmp_lotto = tmp_contenitore->list_lotti;
        tmp_contenitore->quantita_totale = tmp_contenitore->quantita_totale - tmp_quantita_totale;

        while(tmp_lotto != NULL && tmp_quantita_totale - tmp_lotto->quantita > 0) {
            tmp_quantita_totale = tmp_quantita_totale - tmp_lotto->quantita;
            free_lotto = tmp_lotto;
            tmp_lotto = tmp_lotto->next;
            free(free_lotto);
        }
        if(tmp_lotto != NULL && tmp_quantita_totale - tmp_lotto->quantita == 0) {
            free_lotto = tmp_lotto;
            tmp_lotto = tmp_lotto->next;
            free(free_lotto);
        }
        else if(tmp_lotto != NULL)
            tmp_lotto->quantita = tmp_lotto->quantita - tmp_quantita_totale;
        tmp_contenitore->list_lotti = tmp_lotto;
        tmp_ingrediente = tmp_ingrediente->next;
    }
}

bool func_prepara_ordine(Ordine* ordine) {

    // controlla se si puo preparare
    
    Ingrediente* tmp_ingrediente=ordine->ricetta->list_ingredienti;
    Contenitore* tmp_contenitore;
    int tmp_quantita_totale;
    while(tmp_ingrediente!=NULL) {
        tmp_contenitore = tmp_ingrediente->contenitore;
        tmp_quantita_totale = ordine->numero_elementi_ordinati * tmp_ingrediente->quantita;
        if(tmp_contenitore->quantita_totale < tmp_quantita_totale)
            return false;
        func_pulisci_contenitore(tmp_contenitore);
        if(tmp_contenitore->quantita_totale < tmp_quantita_totale)
            return false;
        tmp_ingrediente = tmp_ingrediente->next;
    }

    // rimuovi dal magazzino

    func_rimuovi_ingredienti(ordine);
    
    return true;
}

void func_merge(Ordine array[], int left, int middle, int right) {
    int i, j, k;
    int n1 = middle - left + 1;
    int n2 = right - middle;
    Ordine* L = (Ordine*) malloc(n1*sizeof(Ordine));
    Ordine* R = (Ordine*) malloc(n2*sizeof(Ordine));
    for (i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (j = 0; j < n2; j++)
        R[j] = array[middle + 1 + j];
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (L[i].peso_totale >= R[j].peso_totale) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }
    free(L);
    free(R);
}

void func_merge_sort(Ordine array[], int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        func_merge_sort(array, left, middle);
        func_merge_sort(array, middle + 1, right);
        func_merge(array, left, middle, right);
    }
}

void func_pulisci_contenitore(Contenitore* contenitore) {
    Lotto* tmp_lotto = contenitore->list_lotti;
    int t = tempo;
    while(tmp_lotto != NULL) {
        if(tmp_lotto->scadenza > t)
            break;
        contenitore->quantita_totale -= tmp_lotto->quantita;
        tmp_lotto=tmp_lotto->next;
    }
    contenitore->list_lotti = tmp_lotto;
}
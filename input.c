#include "config.h"
#include "input.h"
#include "display.h"

int lire_choix_menu(void){
    char buffer[100];  // buffer temporaire pour la saisie
    int choix;

    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            // EOF ou erreur
            printf("Erreur de lecture. Reessayez : ");
            continue;
        }

        // Vérifie si l'entrée est un entier valide
        if (sscanf(buffer, "%d", &choix) == 1) {
            return choix;
        } else {
            printf("Entrée invalide, veuillez entrer un nombre : ");
        }
    }
}

char lire_lettre(void){
    char buffer[100]; // buffer temporaire pour la saisie
    char lettre;

    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            // EOF ou erreur
            printf("Erreur de lecture. Reessayez : ");
            continue;
        }

        int i = 0;
        while (buffer[i] != '\0' && isspace(buffer[i])) i++; // ignorer les espaces

        lettre = buffer[i];

        // Vérifie si c'est une lettre
        if (isalpha(lettre)) {
            return toupper(lettre); // renvoie toujours en majuscule
        } else {
            printf("Entrée invalide, veuillez entrer une lettre : ");
        }
    }
}

Difficulte lire_niveau_difficulte(void){
    int choix;

    while (1) {
        printf("Choisissez la difficulte : ");
        if (scanf("%d", &choix) != 1) {
            // Nettoyage du buffer si mauvaise saisie
            while (getchar() != '\n');
            printf("Entrée invalide.\n");
            continue;
        }

        // Nettoyage du '\n'
        while (getchar() != '\n');

        if (choix >= 1 && choix <= 3) {
            return (Difficulte)choix;
        } else {
            printf("Choix invalide. Entrez 1, 2 ou 3.\n");
        }
    }
}

int lire_choix_action(void){
    char buffer[100];

    while (1) {
        printf("Entrez une lettre, '?' pour l'indice, '!' pour quitter : ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return EOF;
        }

        // Prend le premier caractère non espace
        char c = buffer[0];

        // Cas : indice ou quitter
        if (c == '?' || c == '!') {
            return c;
        }

        // Cas : lettre alphabetique
        if (isalpha(c)) {
            return toupper(c);
        }

        printf("Entrée invalide. Veuillez entrer une lettre.\n");
    }
}

int confirmer_action(const char *message){

    char buffer[10];

    while (1) {
        printf("%s (O/N) : ", message);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0; // sécurité
        }

        // Cherche le premier caractère utile
        char c = toupper(buffer[0]);

        if (c == 'O') {
            return 1;
        }
        if (c == 'N') {
            return 0;
        }

        printf("Veuillez repondre par O (oui) ou N (non).\n");
    }
}

void vider_buffer_entree(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void attendre_touche(void){
    char buffer[2];
    printf("\nAppuyez sur Entree pour continuer...");
    fgets(buffer, sizeof(buffer), stdin);
}





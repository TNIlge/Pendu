#include "config.h"
#include "game.h"
#include <stdio.h>
Partie* initialiser_partie(EntreeDictionnaire *entree, Difficulte niveau, int id_user) {

    Partie *partie = malloc(sizeof(Partie));
    if(partie == NULL){
        printf("Erreur d'allocation memoire pour la partie.\n");
        return NULL;
    }

    partie->mot_secret = malloc(strlen(entree->mot) + 1);
    strcpy(partie->mot_secret, entree->mot);

    partie->indice = malloc(strlen(entree->indice) + 1);
    strcpy(partie->indice, entree->indice);

   partie->mot_affiche = malloc(strlen(entree->mot) + 1);
    for(int i = 0; i < strlen(entree->mot); i++) {
        partie->mot_affiche[i] = '_';
    }

    partie->lettres_utilisees[0] = '\0';
    partie->nb_lettres_utilisees = 0;
    partie->essais_max = obtenir_essais_max(niveau);
    partie->essais_restants = partie->essais_max;
    partie->nb_erreurs = 0;
    partie->indice_utilise = 0;
    partie->niveau = niveau;
    partie->heure_debut = time(NULL);
    partie->partie_gagnee = -1; // Partie en cours
    partie->id_utilisateur = id_user;

    return partie;

}

int verifier_lettre(Partie *partie, char lettre){
    int compteur = 0;
    for(int i = 0; i < strlen(partie->mot_secret); i++) {
        if(partie->mot_secret[i] == lettre) {
            compteur++;
        }
    }
    return compteur;
}
void mettre_a_jour_affichage(Partie *partie, char lettre){
    for(int i = 0; i < strlen(partie->mot_secret); i++) {
        if(partie->mot_secret[i] == lettre) {
            partie->mot_affiche[i] = lettre;
        }
    }
}
int est_lettre_utilisee(Partie *partie, char lettre){
    for(int i = 0; i < partie->nb_lettres_utilisees; i++) {
        if(partie->lettres_utilisees[i] == lettre) {
            return 1;
        }
    }
    return 0;
}
void ajouter_lettre_utilisee(Partie *partie, char lettre){
    partie->lettres_utilisees[partie->nb_lettres_utilisees] = lettre;
    partie->nb_lettres_utilisees++;
}
void consulter_indice(Partie *partie){
    if(!partie->indice_utilise) {
        partie->indice_utilise = 1;
        partie->essais_restants--;
    }
}
int indice_deja_consulte(Partie *partie){
    return partie->indice_utilise;
}
int est_victoire(Partie *partie) {

    for (int i = 0; partie->mot_affiche[i] != '\0'; i++) {
        if (partie->mot_affiche[i] == '_') {
            return 0;
    }
    }
        return 1;
}

int est_defaite(Partie *partie){
    return partie->essais_restants <= 0;
}
int obtenir_duree_partie(Partie *partie){
    time_t maintenant = time(NULL);
    double duree = difftime(maintenant, partie->heure_debut);
    return (int)duree;
}
void liberer_partie(Partie *partie){
    if(partie != NULL) {
        free(partie->mot_secret);
        free(partie->mot_affiche);
        free(partie->indice);
        free(partie);
    }
}
int obtenir_essais_max(Difficulte niveau){
    switch(niveau) {
        case FACILE:
            return 10;
        case MOYEN:
            return 8;
        case DIFFICILE:
            return 6;
        default:
            return 8;
    }
}

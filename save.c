#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "save.h"

#define DOSSIER_SAUVEGARDES "data/saves"

/**
 * Construit le nom du fichier de sauvegarde pour un utilisateur
 */
static void construire_nom_fichier_save(int id_utilisateur, char *buffer, size_t taille) {
    snprintf(buffer, taille, "%s/save_%d.txt", DOSSIER_SAUVEGARDES, id_utilisateur);
}

/**
 * Vérifie si une sauvegarde existe pour un utilisateur
 */
int partie_sauvegardee_existe(int id_utilisateur) {
    char nom_fichier[256];
    construire_nom_fichier_save(id_utilisateur, nom_fichier, sizeof(nom_fichier));

    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        return 0;
    }

    fclose(fichier);
    return 1;
}

/**
 * Sauvegarde une partie en cours
 * Format du fichier:
 * Ligne 1: id_utilisateur
 * Ligne 2: mot_secret
 * Ligne 3: mot_affiche
 * Ligne 4: indice
 * Ligne 5: lettres_utilisees
 * Ligne 6: nb_lettres_utilisees|essais_restants|essais_max|nb_erreurs|indice_utilise|niveau|heure_debut
 */
int sauvegarder_partie(Partie *partie) {
    if (partie == NULL) {
        return 0;
    }

    char nom_fichier[256];
    construire_nom_fichier_save(partie->id_utilisateur, nom_fichier, sizeof(nom_fichier));

    FILE *fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        return 0;
    }

    // Ligne 1: ID utilisateur
    fprintf(fichier, "%d\n", partie->id_utilisateur);

    // Ligne 2: Mot secret
    fprintf(fichier, "%s\n", partie->mot_secret);

    // Ligne 3: Mot affiché
    fprintf(fichier, "%s\n", partie->mot_affiche);

    // Ligne 4: Indice
    fprintf(fichier, "%s\n", partie->indice);

    // Ligne 5: Lettres utilisées
    fprintf(fichier, "%s\n", partie->lettres_utilisees);

    // Ligne 6: Données numériques
    fprintf(fichier, "%d|%d|%d|%d|%d|%d|%ld\n",
            partie->nb_lettres_utilisees,
            partie->essais_restants,
            partie->essais_max,
            partie->nb_erreurs,
            partie->indice_utilise,
            (int)partie->niveau,
            partie->heure_debut);

    fclose(fichier);
    return 1;
}

/**
 * Charge une partie sauvegardée
 * Retourne NULL si aucune sauvegarde n'existe
 */
Partie* charger_partie(int id_utilisateur) {
    char nom_fichier[256];
    construire_nom_fichier_save(id_utilisateur, nom_fichier, sizeof(nom_fichier));

    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        return NULL;
    }

    Partie *partie = malloc(sizeof(Partie));
    if (partie == NULL) {
        fclose(fichier);
        return NULL;
    }

    char ligne[512];

    // Ligne 1: ID utilisateur
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        sscanf(ligne, "%d", &partie->id_utilisateur);
    }

    // Ligne 2: Mot secret
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strcspn(ligne, "\n")] = '\0'; // Enlever le \n
        partie->mot_secret = malloc(strlen(ligne) + 1);
        if (partie->mot_secret != NULL) {
            strcpy(partie->mot_secret, ligne);
        }
    }

    // Ligne 3: Mot affiché
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strcspn(ligne, "\n")] = '\0';
        partie->mot_affiche = malloc(strlen(ligne) + 1);
        if (partie->mot_affiche != NULL) {
            strcpy(partie->mot_affiche, ligne);
        }
    }

    // Ligne 4: Indice
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strcspn(ligne, "\n")] = '\0';
        partie->indice = malloc(strlen(ligne) + 1);
        if (partie->indice != NULL) {
            strcpy(partie->indice, ligne);
        }
    }

    // Ligne 5: Lettres utilisées
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne[strcspn(ligne, "\n")] = '\0';
        strncpy(partie->lettres_utilisees, ligne, MAX_LETTRES_ALPHABET - 1);
        partie->lettres_utilisees[MAX_LETTRES_ALPHABET - 1] = '\0';
    }

    // Ligne 6: Données numériques
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        int niveau_temp;
        sscanf(ligne, "%d|%d|%d|%d|%d|%d|%ld",
               &partie->nb_lettres_utilisees,
               &partie->essais_restants,
               &partie->essais_max,
               &partie->nb_erreurs,
               &partie->indice_utilise,
               &niveau_temp,
               &partie->heure_debut);
        partie->niveau = (Difficulte)niveau_temp;
    }

    // Initialiser les champs non sauvegardés
    partie->partie_gagnee = -1; // En cours

    fclose(fichier);
    return partie;
}

/**
 * Supprime la sauvegarde d'un utilisateur
 */
void supprimer_sauvegarde(int id_utilisateur) {
    char nom_fichier[256];
    construire_nom_fichier_save(id_utilisateur, nom_fichier, sizeof(nom_fichier));

    remove(nom_fichier);
}

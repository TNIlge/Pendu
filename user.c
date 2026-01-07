#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "user.h"

#define FICHIER_UTILISATEURS "data/users.txt"

// Variables globales pour le cache des utilisateurs
static Utilisateur *utilisateurs_cache = NULL;
static int nb_utilisateurs_cache = 0;

/**
 * Charge tous les utilisateurs depuis le fichier data/users.txt
 * Format: id|pseudo|mot_de_passe|date_creation|actif
 */
int charger_utilisateurs(Utilisateur **utilisateurs, int *nb_utilisateurs) {
    FILE *fichier = fopen(FICHIER_UTILISATEURS, "r");

    if (fichier == NULL) {
        // Le fichier n'existe pas encore, on initialise vide
        *utilisateurs = malloc(sizeof(Utilisateur) * MAX_UTILISATEURS);
        if (*utilisateurs == NULL) {
            return 0;
        }
        *nb_utilisateurs = 0;
        return 1;
    }

    *utilisateurs = malloc(sizeof(Utilisateur) * MAX_UTILISATEURS);
    if (*utilisateurs == NULL) {
        fclose(fichier);
        return 0;
    }

    *nb_utilisateurs = 0;
    char ligne[256];

    while (fgets(ligne, sizeof(ligne), fichier) != NULL && *nb_utilisateurs < MAX_UTILISATEURS) {
        Utilisateur *user = &(*utilisateurs)[*nb_utilisateurs];

        // Parse la ligne: id|pseudo|mot_de_passe|date_creation|actif
        if (sscanf(ligne, "%d|%[^|]|%[^|]|%ld|%d",
                   &user->id,
                   user->pseudo,
                   user->mot_de_passe,
                   &user->date_creation,
                   &user->actif) == 5) {
            (*nb_utilisateurs)++;
        }
    }

    fclose(fichier);

    // Mise à jour du cache
    utilisateurs_cache = *utilisateurs;
    nb_utilisateurs_cache = *nb_utilisateurs;

    return 1;
}

/**
 * Sauvegarde tous les utilisateurs dans le fichier data/users.txt
 */
int sauvegarder_utilisateurs(Utilisateur *utilisateurs, int nb_utilisateurs) {
    FILE *fichier = fopen(FICHIER_UTILISATEURS, "w");

    if (fichier == NULL) {
        return 0;
    }

    for (int i = 0; i < nb_utilisateurs; i++) {
        fprintf(fichier, "%d|%s|%s|%ld|%d\n",
                utilisateurs[i].id,
                utilisateurs[i].pseudo,
                utilisateurs[i].mot_de_passe,
                utilisateurs[i].date_creation,
                utilisateurs[i].actif);
    }

    fclose(fichier);

    // Mise à jour du cache
    utilisateurs_cache = utilisateurs;
    nb_utilisateurs_cache = nb_utilisateurs;

    return 1;
}

/**
 * Vérifie si un pseudo existe déjà
 */
int pseudo_existe(const char *pseudo) {
    Utilisateur *users = NULL;
    int nb_users = 0;

    if (!charger_utilisateurs(&users, &nb_users)) {
        return 0;
    }

    for (int i = 0; i < nb_users; i++) {
        if (users[i].actif && strcmp(users[i].pseudo, pseudo) == 0) {
            free(users);
            return 1;
        }
    }

    free(users);
    return 0;
}

/**
 * Génère un nouvel ID unique pour un utilisateur
 */
static int generer_nouvel_id(Utilisateur *utilisateurs, int nb_utilisateurs) {
    int max_id = 0;

    for (int i = 0; i < nb_utilisateurs; i++) {
        if (utilisateurs[i].id > max_id) {
            max_id = utilisateurs[i].id;
        }
    }

    return max_id + 1;
}

/**
 * Crée un nouvel utilisateur
 * Retourne un pointeur vers l'utilisateur créé ou NULL en cas d'erreur
 */
Utilisateur* creer_utilisateur(const char *pseudo, const char *mot_de_passe) {
    // Vérifier que le pseudo n'existe pas déjà
    if (pseudo_existe(pseudo)) {
        return NULL;
    }

    Utilisateur *users = NULL;
    int nb_users = 0;

    if (!charger_utilisateurs(&users, &nb_users)) {
        return NULL;
    }

    if (nb_users >= MAX_UTILISATEURS) {
        free(users);
        return NULL;
    }

    // Créer le nouvel utilisateur
    Utilisateur *nouveau = &users[nb_users];
    nouveau->id = generer_nouvel_id(users, nb_users);
    strncpy(nouveau->pseudo, pseudo, MAX_PSEUDO - 1);
    nouveau->pseudo[MAX_PSEUDO - 1] = '\0';
    strncpy(nouveau->mot_de_passe, mot_de_passe, MAX_PASSWORD - 1);
    nouveau->mot_de_passe[MAX_PASSWORD - 1] = '\0';
    nouveau->date_creation = time(NULL);
    nouveau->actif = 1;

    nb_users++;

    // Sauvegarder
    if (!sauvegarder_utilisateurs(users, nb_users)) {
        free(users);
        return NULL;
    }

    // Allouer et retourner une copie de l'utilisateur
    Utilisateur *copie = malloc(sizeof(Utilisateur));
    if (copie != NULL) {
        memcpy(copie, nouveau, sizeof(Utilisateur));
    }

    free(users);
    return copie;
}

/**
 * Connecte un utilisateur (vérifie pseudo et mot de passe)
 * Retourne un pointeur vers l'utilisateur ou NULL si échec
 */
Utilisateur* connecter_utilisateur(const char *pseudo, const char *mot_de_passe) {
    Utilisateur *users = NULL;
    int nb_users = 0;

    if (!charger_utilisateurs(&users, &nb_users)) {
        return NULL;
    }

    for (int i = 0; i < nb_users; i++) {
        if (users[i].actif &&
            strcmp(users[i].pseudo, pseudo) == 0 &&
            strcmp(users[i].mot_de_passe, mot_de_passe) == 0) {

            // Utilisateur trouvé, créer une copie
            Utilisateur *copie = malloc(sizeof(Utilisateur));
            if (copie != NULL) {
                memcpy(copie, &users[i], sizeof(Utilisateur));
            }

            free(users);
            return copie;
        }
    }

    free(users);
    return NULL;
}

/**
 * Trouve un utilisateur par son ID
 */
Utilisateur* trouver_utilisateur_par_id(int id) {
    Utilisateur *users = NULL;
    int nb_users = 0;

    if (!charger_utilisateurs(&users, &nb_users)) {
        return NULL;
    }

    for (int i = 0; i < nb_users; i++) {
        if (users[i].actif && users[i].id == id) {
            Utilisateur *copie = malloc(sizeof(Utilisateur));
            if (copie != NULL) {
                memcpy(copie, &users[i], sizeof(Utilisateur));
            }
            free(users);
            return copie;
        }
    }

    free(users);
    return NULL;
}

/**
 * Trouve un utilisateur par son pseudo
 */
Utilisateur* trouver_utilisateur_par_pseudo(const char *pseudo) {
    Utilisateur *users = NULL;
    int nb_users = 0;

    if (!charger_utilisateurs(&users, &nb_users)) {
        return NULL;
    }

    for (int i = 0; i < nb_users; i++) {
        if (users[i].actif && strcmp(users[i].pseudo, pseudo) == 0) {
            Utilisateur *copie = malloc(sizeof(Utilisateur));
            if (copie != NULL) {
                memcpy(copie, &users[i], sizeof(Utilisateur));
            }
            free(users);
            return copie;
        }
    }

    free(users);
    return NULL;
}

/**
 * Libère la mémoire allouée pour les utilisateurs
 */
void liberer_utilisateurs(Utilisateur *utilisateurs) {
    if (utilisateurs != NULL) {
        free(utilisateurs);
    }
}

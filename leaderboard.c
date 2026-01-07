#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leaderboard.h"
#include "stats.h"
#include "user.h"
#include "display.h"

#define FICHIER_CLASSEMENT "data/leaderboard.txt"

/**
 * Génère le classement complet en lisant tous les utilisateurs et leurs stats
 */
Leaderboard* generer_classement(void) {
    Leaderboard *leaderboard = malloc(sizeof(Leaderboard));
    if (leaderboard == NULL) {
        return NULL;
    }

    leaderboard->nb_joueurs = 0;
    leaderboard->derniere_maj = time(NULL);

    // Charger tous les utilisateurs
    Utilisateur *users = NULL;
    int nb_users = 0;

    if (!charger_utilisateurs(&users, &nb_users)) {
        free(leaderboard);
        return NULL;
    }

    // Pour chaque utilisateur actif, charger ses stats et l'ajouter au classement
    for (int i = 0; i < nb_users && leaderboard->nb_joueurs < MAX_CLASSEMENT; i++) {
        if (!users[i].actif) {
            continue;
        }

        Statistiques *stats = charger_statistiques_utilisateur(users[i].id);
        if (stats == NULL) {
            continue;
        }

        // Ajouter seulement les joueurs qui ont joué au moins une partie
        if (stats->total_parties > 0) {
            EntreeClassement *entree = &leaderboard->classement[leaderboard->nb_joueurs];

            entree->id_utilisateur = users[i].id;
            strncpy(entree->pseudo, users[i].pseudo, MAX_PSEUDO - 1);
            entree->pseudo[MAX_PSEUDO - 1] = '\0';
            entree->total_parties = stats->total_parties;
            entree->total_victoires = stats->total_victoires;
            entree->taux_victoire = stats->taux_victoire;
            entree->moyenne_essais = stats->moyenne_essais;
            entree->rang = 0; // Sera calculé après le tri

            leaderboard->nb_joueurs++;
        }

        free(stats);
    }

    free(users);

    // Trier par défaut selon le taux de victoire (critère 0)
    trier_classement(leaderboard, 0);

    // Assigner les rangs
    for (int i = 0; i < leaderboard->nb_joueurs; i++) {
        leaderboard->classement[i].rang = i + 1;
    }

    return leaderboard;
}

/**
 * Fonction de comparaison pour qsort - Taux de victoire (décroissant)
 */
static int comparer_taux_victoire(const void *a, const void *b) {
    EntreeClassement *e1 = (EntreeClassement *)a;
    EntreeClassement *e2 = (EntreeClassement *)b;

    // Trier par taux de victoire décroissant
    if (e2->taux_victoire > e1->taux_victoire) return 1;
    if (e2->taux_victoire < e1->taux_victoire) return -1;

    // En cas d'égalité, trier par nombre de victoires
    return e2->total_victoires - e1->total_victoires;
}

/**
 * Fonction de comparaison pour qsort - Nombre de victoires (décroissant)
 */
static int comparer_victoires(const void *a, const void *b) {
    EntreeClassement *e1 = (EntreeClassement *)a;
    EntreeClassement *e2 = (EntreeClassement *)b;

    return e2->total_victoires - e1->total_victoires;
}

/**
 * Fonction de comparaison pour qsort - Moyenne d'essais (croissant = meilleur)
 */
static int comparer_moyenne_essais(const void *a, const void *b) {
    EntreeClassement *e1 = (EntreeClassement *)a;
    EntreeClassement *e2 = (EntreeClassement *)b;

    if (e1->moyenne_essais > e2->moyenne_essais) return 1;
    if (e1->moyenne_essais < e2->moyenne_essais) return -1;
    return 0;
}

/**
 * Trie le classement selon un critère
 * critere: 0 = Taux de victoire, 1 = Nombre de victoires, 2 = Moyenne essais
 */
void trier_classement(Leaderboard *leaderboard, int critere) {
    if (leaderboard == NULL || leaderboard->nb_joueurs == 0) {
        return;
    }

    switch (critere) {
        case 0:
            qsort(leaderboard->classement, leaderboard->nb_joueurs,
                  sizeof(EntreeClassement), comparer_taux_victoire);
            break;
        case 1:
            qsort(leaderboard->classement, leaderboard->nb_joueurs,
                  sizeof(EntreeClassement), comparer_victoires);
            break;
        case 2:
            qsort(leaderboard->classement, leaderboard->nb_joueurs,
                  sizeof(EntreeClassement), comparer_moyenne_essais);
            break;
        default:
            qsort(leaderboard->classement, leaderboard->nb_joueurs,
                  sizeof(EntreeClassement), comparer_taux_victoire);
    }

    // Réassigner les rangs après le tri
    for (int i = 0; i < leaderboard->nb_joueurs; i++) {
        leaderboard->classement[i].rang = i + 1;
    }
}

/**
 * Sauvegarde le classement dans un fichier
 */
int sauvegarder_classement(Leaderboard *leaderboard) {
    if (leaderboard == NULL) {
        return 0;
    }

    FILE *fichier = fopen(FICHIER_CLASSEMENT, "w");
    if (fichier == NULL) {
        return 0;
    }

    // Ligne 1: Nombre de joueurs et date de MAJ
    fprintf(fichier, "%d|%ld\n", leaderboard->nb_joueurs, leaderboard->derniere_maj);

    // Lignes suivantes: Entrées du classement
    for (int i = 0; i < leaderboard->nb_joueurs; i++) {
        EntreeClassement *e = &leaderboard->classement[i];
        fprintf(fichier, "%d|%s|%d|%d|%.2f|%.2f|%d\n",
                e->id_utilisateur,
                e->pseudo,
                e->total_parties,
                e->total_victoires,
                e->taux_victoire,
                e->moyenne_essais,
                e->rang);
    }

    fclose(fichier);
    return 1;
}

/**
 * Charge le classement depuis un fichier
 */
Leaderboard* charger_classement(void) {
    FILE *fichier = fopen(FICHIER_CLASSEMENT, "r");

    // Si le fichier n'existe pas, générer un nouveau classement
    if (fichier == NULL) {
        return generer_classement();
    }

    Leaderboard *leaderboard = malloc(sizeof(Leaderboard));
    if (leaderboard == NULL) {
        fclose(fichier);
        return NULL;
    }

    char ligne[256];

    // Ligne 1: Nombre de joueurs et date
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        sscanf(ligne, "%d|%ld", &leaderboard->nb_joueurs, &leaderboard->derniere_maj);
    }

    // Lignes suivantes: Entrées
    int index = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL && index < leaderboard->nb_joueurs) {
        EntreeClassement *e = &leaderboard->classement[index];

        sscanf(ligne, "%d|%[^|]|%d|%d|%f|%f|%d",
               &e->id_utilisateur,
               e->pseudo,
               &e->total_parties,
               &e->total_victoires,
               &e->taux_victoire,
               &e->moyenne_essais,
               &e->rang);

        index++;
    }

    fclose(fichier);
    return leaderboard;
}

/**
 * Obtient le rang d'un utilisateur dans le classement
 * Retourne 0 si l'utilisateur n'est pas dans le classement
 */
int obtenir_rang_utilisateur(Leaderboard *leaderboard, int id_utilisateur) {
    if (leaderboard == NULL) {
        return 0;
    }

    for (int i = 0; i < leaderboard->nb_joueurs; i++) {
        if (leaderboard->classement[i].id_utilisateur == id_utilisateur) {
            return leaderboard->classement[i].rang;
        }
    }

    return 0;
}

/**
 * Compare deux joueurs et affiche les résultats
 */
void comparer_deux_joueurs(int id_joueur1, int id_joueur2) {
    // Charger les statistiques des deux joueurs
    Statistiques *stats1 = charger_statistiques_utilisateur(id_joueur1);
    Statistiques *stats2 = charger_statistiques_utilisateur(id_joueur2);

    if (stats1 == NULL || stats2 == NULL) {
        afficher_message("Erreur lors du chargement des statistiques.");
        if (stats1) free(stats1);
        if (stats2) free(stats2);
        return;
    }

    // Charger les utilisateurs pour obtenir les pseudos
    Utilisateur *user1 = trouver_utilisateur_par_id(id_joueur1);
    Utilisateur *user2 = trouver_utilisateur_par_id(id_joueur2);

    if (user1 == NULL || user2 == NULL) {
        afficher_message("Erreur lors du chargement des utilisateurs.");
        free(stats1);
        free(stats2);
        if (user1) free(user1);
        if (user2) free(user2);
        return;
    }

    // Charger le classement pour obtenir les rangs
    Leaderboard *leaderboard = charger_classement();
    int rang1 = obtenir_rang_utilisateur(leaderboard, id_joueur1);
    int rang2 = obtenir_rang_utilisateur(leaderboard, id_joueur2);

    // Afficher la comparaison (utilise la fonction du développeur 2)
    afficher_comparaison(stats1, stats2, user1->pseudo, user2->pseudo, rang1, rang2);

    // Libérer la mémoire
    free(stats1);
    free(stats2);
    free(user1);
    free(user2);
    liberer_classement(leaderboard);
}

/**
 * Libère la mémoire allouée pour le classement
 */
void liberer_classement(Leaderboard *leaderboard) {
    if (leaderboard != NULL) {
        free(leaderboard);
    }
}

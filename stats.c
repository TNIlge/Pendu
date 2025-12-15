#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stats.h"

#define DOSSIER_STATS "data/stats"

/**
 * Construit le nom du fichier de statistiques pour un utilisateur
 */
static void construire_nom_fichier_stats(int id_utilisateur, char *buffer, size_t taille) {
    snprintf(buffer, taille, "%s/user_%d.txt", DOSSIER_STATS, id_utilisateur);
}

/**
 * Initialise une structure Statistiques vide
 */
void initialiser_statistiques(Statistiques *stats, int id_utilisateur) {
    memset(stats, 0, sizeof(Statistiques));
    stats->id_utilisateur = id_utilisateur;
    stats->taux_victoire = 0.0f;
    stats->moyenne_essais = 0.0f;
    stats->moyenne_duree = 0.0f;
}

/**
 * Charge les statistiques d'un utilisateur depuis le fichier
 * Format du fichier:
 * Ligne 1: id_utilisateur
 * Ligne 2: total_parties|total_victoires|total_defaites|total_essais|total_secondes|total_indices
 * Ligne 3: parties_facile|victoires_facile|parties_moyen|victoires_moyen|parties_difficile|victoires_difficile
 * Ligne 4+: Historique des parties (mot|essais|duree|niveau|victoire|indice|date)
 */
Statistiques* charger_statistiques_utilisateur(int id_utilisateur) {
    char nom_fichier[256];
    construire_nom_fichier_stats(id_utilisateur, nom_fichier, sizeof(nom_fichier));

    FILE *fichier = fopen(nom_fichier, "r");

    Statistiques *stats = malloc(sizeof(Statistiques));
    if (stats == NULL) {
        return NULL;
    }

    // Si le fichier n'existe pas, initialiser avec des valeurs par défaut
    if (fichier == NULL) {
        initialiser_statistiques(stats, id_utilisateur);
        return stats;
    }

    // Lecture des données
    char ligne[512];

    // Ligne 1: ID utilisateur
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        sscanf(ligne, "%d", &stats->id_utilisateur);
    }

    // Ligne 2: Totaux
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        sscanf(ligne, "%d|%d|%d|%d|%d|%d",
               &stats->total_parties,
               &stats->total_victoires,
               &stats->total_defaites,
               &stats->total_essais,
               &stats->total_secondes,
               &stats->total_indices_utilises);
    }

    // Ligne 3: Stats par niveau
    if (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        sscanf(ligne, "%d|%d|%d|%d|%d|%d",
               &stats->parties_facile,
               &stats->victoires_facile,
               &stats->parties_moyen,
               &stats->victoires_moyen,
               &stats->parties_difficile,
               &stats->victoires_difficile);
    }

    // Lignes suivantes: Historique des parties
    stats->nb_parties_recentes = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL &&
           stats->nb_parties_recentes < 10) {
        StatPartie *sp = &stats->parties_recentes[stats->nb_parties_recentes];

        int niveau_temp;
        if (sscanf(ligne, "%[^|]|%d|%d|%d|%d|%d|%ld",
                   sp->mot,
                   &sp->essais_utilises,
                   &sp->duree_secondes,
                   &niveau_temp,
                   &sp->victoire,
                   &sp->indice_utilise,
                   &sp->date) == 7) {
            sp->niveau = (Difficulte)niveau_temp;
            stats->nb_parties_recentes++;
        }
    }

    fclose(fichier);

    // Recalculer les moyennes
    calculer_moyennes(stats);

    return stats;
}

/**
 * Sauvegarde les statistiques d'un utilisateur dans le fichier
 */
int sauvegarder_statistiques_utilisateur(Statistiques *stats) {
    if (stats == NULL) {
        return 0;
    }

    char nom_fichier[256];
    construire_nom_fichier_stats(stats->id_utilisateur, nom_fichier, sizeof(nom_fichier));

    FILE *fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        return 0;
    }

    // Ligne 1: ID utilisateur
    fprintf(fichier, "%d\n", stats->id_utilisateur);

    // Ligne 2: Totaux
    fprintf(fichier, "%d|%d|%d|%d|%d|%d\n",
            stats->total_parties,
            stats->total_victoires,
            stats->total_defaites,
            stats->total_essais,
            stats->total_secondes,
            stats->total_indices_utilises);

    // Ligne 3: Stats par niveau
    fprintf(fichier, "%d|%d|%d|%d|%d|%d\n",
            stats->parties_facile,
            stats->victoires_facile,
            stats->parties_moyen,
            stats->victoires_moyen,
            stats->parties_difficile,
            stats->victoires_difficile);

    // Lignes suivantes: Historique des parties
    for (int i = 0; i < stats->nb_parties_recentes; i++) {
        StatPartie *sp = &stats->parties_recentes[i];
        fprintf(fichier, "%s|%d|%d|%d|%d|%d|%ld\n",
                sp->mot,
                sp->essais_utilises,
                sp->duree_secondes,
                (int)sp->niveau,
                sp->victoire,
                sp->indice_utilise,
                sp->date);
    }

    fclose(fichier);
    return 1;
}

/**
 * Ajoute une partie terminée aux statistiques
 */
void ajouter_partie_aux_stats(Statistiques *stats, Partie *partie) {
    if (stats == NULL || partie == NULL) {
        return;
    }

    // Mettre à jour les totaux
    stats->total_parties++;

    if (partie->partie_gagnee == 1) {
        stats->total_victoires++;
    } else {
        stats->total_defaites++;
    }

    int essais_utilises = partie->essais_max - partie->essais_restants;
    stats->total_essais += essais_utilises;

    int duree = (int)difftime(time(NULL), partie->heure_debut);
    stats->total_secondes += duree;

    if (partie->indice_utilise) {
        stats->total_indices_utilises++;
    }

    // Mettre à jour les stats par niveau
    switch (partie->niveau) {
        case FACILE:
            stats->parties_facile++;
            if (partie->partie_gagnee == 1) {
                stats->victoires_facile++;
            }
            break;
        case MOYEN:
            stats->parties_moyen++;
            if (partie->partie_gagnee == 1) {
                stats->victoires_moyen++;
            }
            break;
        case DIFFICILE:
            stats->parties_difficile++;
            if (partie->partie_gagnee == 1) {
                stats->victoires_difficile++;
            }
            break;
    }

    // Ajouter à l'historique (FIFO - garder les 10 dernières)
    if (stats->nb_parties_recentes < 10) {
        // Il y a de la place
        StatPartie *sp = &stats->parties_recentes[stats->nb_parties_recentes];
        strncpy(sp->mot, partie->mot_secret, MAX_LONGUEUR_MOT - 1);
        sp->mot[MAX_LONGUEUR_MOT - 1] = '\0';
        sp->essais_utilises = essais_utilises;
        sp->duree_secondes = duree;
        sp->niveau = partie->niveau;
        sp->victoire = partie->partie_gagnee;
        sp->indice_utilise = partie->indice_utilise;
        sp->date = time(NULL);
        stats->nb_parties_recentes++;
    } else {
        // Décaler toutes les parties vers la gauche et ajouter à la fin
        for (int i = 0; i < 9; i++) {
            stats->parties_recentes[i] = stats->parties_recentes[i + 1];
        }

        StatPartie *sp = &stats->parties_recentes[9];
        strncpy(sp->mot, partie->mot_secret, MAX_LONGUEUR_MOT - 1);
        sp->mot[MAX_LONGUEUR_MOT - 1] = '\0';
        sp->essais_utilises = essais_utilises;
        sp->duree_secondes = duree;
        sp->niveau = partie->niveau;
        sp->victoire = partie->partie_gagnee;
        sp->indice_utilise = partie->indice_utilise;
        sp->date = time(NULL);
    }

    // Recalculer les moyennes
    calculer_moyennes(stats);

    // Sauvegarder
    sauvegarder_statistiques_utilisateur(stats);
}

/**
 * Calcule les moyennes (taux de victoire, moyenne essais, moyenne durée)
 */
void calculer_moyennes(Statistiques *stats) {
    if (stats == NULL) {
        return;
    }

    if (stats->total_parties > 0) {
        stats->taux_victoire = (float)stats->total_victoires / stats->total_parties * 100.0f;
        stats->moyenne_essais = (float)stats->total_essais / stats->total_parties;
        stats->moyenne_duree = (float)stats->total_secondes / stats->total_parties;
    } else {
        stats->taux_victoire = 0.0f;
        stats->moyenne_essais = 0.0f;
        stats->moyenne_duree = 0.0f;
    }
}

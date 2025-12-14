#ifndef STATS_H
#define STATS_H

#include "config.h"

// Prototypes des fonctions de stats.c
Statistiques* charger_statistiques_utilisateur(int id_utilisateur);
int sauvegarder_statistiques_utilisateur(Statistiques *stats);
void ajouter_partie_aux_stats(Statistiques *stats, Partie *partie);
void calculer_moyennes(Statistiques *stats);
void initialiser_statistiques(Statistiques *stats, int id_utilisateur);
void afficher_statistiques_utilisateur(Statistiques *stats, const char *pseudo);

#endif // STATS_H

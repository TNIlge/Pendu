#ifndef SAVE_H
#define SAVE_H

#include "config.h"

// Prototypes des fonctions de save.c
int sauvegarder_partie(Partie *partie);
Partie* charger_partie(void);
int partie_sauvegardee_existe(void);
void supprimer_sauvegarde(void);
Statistiques* charger_statistiques(void);
int sauvegarder_statistiques(Statistiques *stats);
void ajouter_partie_aux_stats(Statistiques *stats, Partie *partie);
void calculer_moyennes(Statistiques *stats);
void initialiser_statistiques(Statistiques *stats);

#endif

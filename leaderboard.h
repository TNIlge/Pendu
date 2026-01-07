#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "config.h"

// Prototypes des fonctions de leaderboard.c
Leaderboard* generer_classement(void);
void trier_classement(Leaderboard *leaderboard, int critere);
int sauvegarder_classement(Leaderboard *leaderboard);
Leaderboard* charger_classement(void);
void afficher_classement(Leaderboard *leaderboard, int id_utilisateur);
int obtenir_rang_utilisateur(Leaderboard *leaderboard, int id_utilisateur);
void comparer_deux_joueurs(int id_joueur1, int id_joueur2);
void liberer_classement(Leaderboard *leaderboard);

#endif // LEADERBOARD_H

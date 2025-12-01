
#ifndef GAME_H
#define GAME_H

#include "config.h"

// Prototypes des fonctions de game.c
Partie* initialiser_partie(char *mot, Difficulte niveau);
int verifier_lettre(Partie *partie, char lettre);
void mettre_a_jour_affichage(Partie *partie, char lettre);
int est_lettre_utilisee(Partie *partie, char lettre);
void ajouter_lettre_utilisee(Partie *partie, char lettre);
int est_victoire(Partie *partie);
int est_defaite(Partie *partie);
int obtenir_duree_partie(Partie *partie);
void liberer_partie(Partie *partie);
int obtenir_essais_max(Difficulte niveau);

#endif

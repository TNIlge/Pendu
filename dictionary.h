#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "config.h"

// Prototypes des fonctions de dictionary.c
Dictionnaire* charger_dictionnaire(Difficulte niveau);
char* selectionner_mot_aleatoire(Dictionnaire *dico);
void liberer_dictionnaire(Dictionnaire *dico);
int valider_longueur_mot(const char *mot, Difficulte niveau);

#endif

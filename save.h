#ifndef SAVE_H
#define SAVE_H

#include "config.h"

// Prototypes des fonctions de save.c
int sauvegarder_partie(Partie *partie);
Partie* charger_partie(int id_utilisateur);
int partie_sauvegardee_existe(int id_utilisateur);
void supprimer_sauvegarde(int id_utilisateur);

#endif


#ifndef INPUT_H
#define INPUT_H

#include "config.h"

// Prototypes des fonctions de input.c
int lire_choix_menu(void);
char lire_lettre(void);
Difficulte lire_niveau_difficulte(void);
int lire_choix_action(void);
int confirmer_action(const char *message);
void vider_buffer_entree(void);
void attendre_touche(void);

#endif

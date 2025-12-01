
#ifndef USER_H
#define USER_H

#include "config.h"

// Prototypes des fonctions de user.c
int charger_utilisateurs(Utilisateur **utilisateurs, int *nb_utilisateurs);
int sauvegarder_utilisateurs(Utilisateur *utilisateurs, int nb_utilisateurs);
Utilisateur* creer_utilisateur(const char *pseudo, const char *mot_de_passe);
Utilisateur* connecter_utilisateur(const char *pseudo, const char *mot_de_passe);
int pseudo_existe(const char *pseudo);
Utilisateur* trouver_utilisateur_par_id(int id);
Utilisateur* trouver_utilisateur_par_pseudo(const char *pseudo);
void liberer_utilisateurs(Utilisateur *utilisateurs);

#endif

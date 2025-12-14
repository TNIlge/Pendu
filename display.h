#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

// Prototypes des fonctions de display.c
void nettoyer_ecran(void);
void afficher_message(const char *message);
void afficher_menu(const char* titre, char options[][MAX_OPTION_LONGUEUR], int nb_options);
void afficher_pendu(int nb_erreurs);
void afficher_etat_jeu(Partie *partie);
void afficher_mot(const char *mot, const int *lettres_trouvees);
void afficher_lettres_utilisees(Partie *partie);
void formater_duree(int secondes, char *buffer);
void afficher_aide(void);
void afficher_statistiques(Statistiques *stats);
void afficher_victoire(Partie *partie);
void afficher_defaite(Partie *partie);

void afficher_indice(const char *indice);
void afficher_classement_complet(Leaderboard *leaderboard, int id_utilisateur);
void afficher_comparaison(Statistiques *stats1, Statistiques *stats2,
                          const char *pseudo1, const char *pseudo2,
                          int rang1, int rang2);


#endif

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Niveaux de difficulté
typedef enum {
    FACILE = 1,      // 3-5 lettres, 10 essais
    MOYEN = 2,       // 6-8 lettres, 8 essais
    DIFFICILE = 3    // 9+ lettres, 6 essais
} Difficulte;

// Type de dictionnaire (correspond au niveau)
typedef enum {
    DICO_FACILE = 1,
    DICO_MOYEN = 2,
    DICO_DIFFICILE = 3
} TypeDictionnaire;


#define MAX_LONGUEUR_MOT 50        // Longueur maximale d'un mot
#define MAX_LETTRES_ALPHABET 26    // Nombre de lettres dans l'alphabet
#define MAX_PSEUDO 30              // Longueur maximale d'un pseudo
#define MAX_PASSWORD 50            // Longueur maximale d'un mot de passe
#define MAX_INDICE 200             // Longueur maximale d'un indice
#define MAX_UTILISATEURS 100       // Nombre maximum d'utilisateurs
#define MAX_CLASSEMENT 50          // Nombre de joueurs affichés au classement

// Limites de longueur de mots par niveau
#define MIN_LONGUEUR_FACILE 3
#define MAX_LONGUEUR_FACILE 5
#define MIN_LONGUEUR_MOYEN 6
#define MAX_LONGUEUR_MOYEN 8
#define MIN_LONGUEUR_DIFFICILE 9

// Nombre d'essais par niveau
#define ESSAIS_FACILE 10
#define ESSAIS_MOYEN 8
#define ESSAIS_DIFFICILE 6

typedef struct {
    int id;                        // ID unique de l'utilisateur
    char pseudo[MAX_PSEUDO];       // Pseudo de l'utilisateur
    char mot_de_passe[MAX_PASSWORD]; // Mot de passe (stockage simple)
    time_t date_creation;          // Date de création du compte
    int actif;                     // 1 si compte actif, 0 si supprimé
} Utilisateur;

typedef struct {
    int est_connecte;              // 1 si un utilisateur est connecté
    Utilisateur utilisateur;       // L'utilisateur connecté
} Session;

typedef struct {
    char mot[MAX_LONGUEUR_MOT];    // Le mot
    char indice[MAX_INDICE];       // L'indice pour ce mot
} EntreeDictionnaire;

typedef struct {
    char *mot_secret;              // Le mot à deviner (alloué dynamiquement)
    char *mot_affiche;             // Mot avec _ pour lettres non trouvées
    char *indice;                  // L'indice pour ce mot (alloué dynamiquement)
    char lettres_utilisees[MAX_LETTRES_ALPHABET];  // Lettres déjà proposées
    int nb_lettres_utilisees;      // Nombre de lettres utilisées
    int essais_restants;           // Nombre d'essais restants
    int essais_max;                // Maximum d'essais selon difficulté
    int nb_erreurs;                // Nombre d'erreurs commises
    int indice_utilise;            // 1 si l'utilisateur a consulté l'indice
    Difficulte niveau;             // Niveau de difficulté
    time_t heure_debut;            // Heure de début (pour chronomètre)
    int partie_gagnee;             // 1 si victoire, 0 sinon, -1 en cours
    int id_utilisateur;            // ID de l'utilisateur jouant cette partie
} Partie;


/* ============================================================
   STRUCTURE : DICTIONNAIRE
   ============================================================ */

typedef struct {
    EntreeDictionnaire *entrees;   // Tableau d'entrées (mots + indices)
    int nb_entrees;                // Nombre d'entrées dans le dictionnaire
    TypeDictionnaire type;         // Type de dictionnaire
    char nom_fichier[100];         // Nom du fichier source
} Dictionnaire;


typedef struct {
    char mot[MAX_LONGUEUR_MOT];    // Le mot de la partie
    int essais_utilises;           // Nombre d'essais utilisés
    int duree_secondes;            // Durée en secondes
    Difficulte niveau;             // Niveau joué
    int victoire;                  // 1 si gagné, 0 si perdu
    int indice_utilise;            // 1 si indice consulté
    time_t date;                   // Date de la partie
} StatPartie;


typedef struct {
    int id_utilisateur;            // ID de l'utilisateur
    int total_parties;             // Nombre total de parties jouées
    int total_victoires;           // Nombre de victoires
    int total_defaites;            // Nombre de défaites
    int total_essais;              // Total des essais sur toutes les parties
    int total_secondes;            // Temps total de jeu en secondes
    int total_indices_utilises;    // Nombre total d'indices utilisés

    // Statistiques par niveau
    int parties_facile;            // Parties en mode facile
    int victoires_facile;          // Victoires en mode facile
    int parties_moyen;             // Parties en mode moyen
    int victoires_moyen;           // Victoires en mode moyen
    int parties_difficile;         // Parties en mode difficile
    int victoires_difficile;       // Victoires en mode difficile

    // Moyennes calculées
    float taux_victoire;           // Pourcentage de victoires
    float moyenne_essais;          // Moyenne d'essais par partie
    float moyenne_duree;           // Durée moyenne par partie

    // Historique des parties récentes (10 dernières parties)
    StatPartie parties_recentes[10];
    int nb_parties_recentes;       // Nombre de parties dans l'historique
} Statistiques;


typedef struct {
    int id_utilisateur;            // ID de l'utilisateur
    char pseudo[MAX_PSEUDO];       // Pseudo de l'utilisateur
    int total_parties;             // Nombre total de parties
    int total_victoires;           // Nombre de victoires
    float taux_victoire;           // Pourcentage de victoires
    float moyenne_essais;          // Moyenne d'essais par partie
    int rang;                      // Position dans le classement
} EntreeClassement;


typedef struct {
    EntreeClassement classement[MAX_CLASSEMENT];  // Tableau des entrées
    int nb_joueurs;                // Nombre de joueurs dans le classement
    time_t derniere_maj;           // Date de dernière mise à jour
} Leaderboard;

#endif

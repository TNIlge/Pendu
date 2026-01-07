#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "config.h"
#include "user.h"
#include "game.h"
#include "dictionary.h"
#include "display.h"
#include "stats.h"
#include "leaderboard.h"
#include "save.h"

Session session_globale = {0, {0}};

// Prototypes des fonctions du menu
void menu_principal(void);
void menu_authentification(void);
void menu_jouer(void);
void jouer_partie(Difficulte niveau);
void reprendre_partie_sauvegardee(void);
void menu_statistiques(void);
void menu_classement(void);
void inscription(void);
void connexion(void);
char lire_lettre(void);
void vider_buffer(void);

int main(void) {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));

    printf("=============================================================\n");
    printf("        Bienvenue dans le JEU DU PENDU - Projet C\n");
    printf("=============================================================\n");
    printf("\nChargement du jeu...\n");

    // Afficher le menu d'authentification
    menu_authentification();

    // Si l'utilisateur est connecté, afficher le menu principal
    if (session_globale.est_connecte) {
        menu_principal();
    }

    printf("\n=============================================================\n");
    printf("        Merci d'avoir joué ! À bientôt !\n");
    printf("=============================================================\n");

    return 0;
}

/**
 * Vide le buffer d'entrée pour éviter les problèmes de saisie
 */
void vider_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Menu d'authentification (Inscription / Connexion / Quitter)
 */
void menu_authentification(void) {
    int choix = 0;

    while (!session_globale.est_connecte && choix != 3) {
        char options[3][MAX_OPTION_LONGUEUR] = {
            "Inscription",
            "Connexion",
            "Quitter"
        };

        afficher_menu("AUTHENTIFICATION", options, 3);

        if (scanf("%d", &choix) != 1) {
            vider_buffer();
            afficher_message("Erreur : Veuillez entrer un nombre valide.");
            continue;
        }
        vider_buffer();

        switch (choix) {
            case 1:
                inscription();
                break;
            case 2:
                connexion();
                break;
            case 3:
                afficher_message("Au revoir !");
                break;
            default:
                afficher_message("Choix invalide. Veuillez réessayer.");
        }
    }
}

/**
 * Gère l'inscription d'un nouvel utilisateur
 */
void inscription(void) {
    char pseudo[MAX_PSEUDO];
    char mot_de_passe[MAX_PASSWORD];

    nettoyer_ecran();
    printf("=============================================================\n");
    printf("                      INSCRIPTION\n");
    printf("=============================================================\n");

    printf("Pseudo : ");
    if (fgets(pseudo, MAX_PSEUDO, stdin) == NULL) {
        afficher_message("Erreur de saisie.");
        return;
    }
    pseudo[strcspn(pseudo, "\n")] = '\0';

    // Vérifier si le pseudo existe déjà
    if (pseudo_existe(pseudo)) {
        afficher_message("Ce pseudo existe déjà. Veuillez en choisir un autre.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }

    printf("Mot de passe : ");
    if (fgets(mot_de_passe, MAX_PASSWORD, stdin) == NULL) {
        afficher_message("Erreur de saisie.");
        return;
    }
    mot_de_passe[strcspn(mot_de_passe, "\n")] = '\0';

    // Créer l'utilisateur
    Utilisateur *nouveau = creer_utilisateur(pseudo, mot_de_passe);

    if (nouveau != NULL) {
        afficher_message("\nInscription réussie !");
        printf("Vous pouvez maintenant vous connecter.\n");
        free(nouveau);
    } else {
        afficher_message("Erreur lors de l'inscription.");
    }

    printf("\nAppuyez sur Entrée pour continuer...");
    getchar();
}

/**
 * Gère la connexion d'un utilisateur existant
 */
void connexion(void) {
    char pseudo[MAX_PSEUDO];
    char mot_de_passe[MAX_PASSWORD];

    nettoyer_ecran();
    printf("=============================================================\n");
    printf("                       CONNEXION\n");
    printf("=============================================================\n");

    printf("Pseudo : ");
    if (fgets(pseudo, MAX_PSEUDO, stdin) == NULL) {
        afficher_message("Erreur de saisie.");
        return;
    }
    pseudo[strcspn(pseudo, "\n")] = '\0';

    printf("Mot de passe : ");
    if (fgets(mot_de_passe, MAX_PASSWORD, stdin) == NULL) {
        afficher_message("Erreur de saisie.");
        return;
    }
    mot_de_passe[strcspn(mot_de_passe, "\n")] = '\0';

    // Tenter la connexion
    Utilisateur *user = connecter_utilisateur(pseudo, mot_de_passe);

    if (user != NULL) {
        session_globale.est_connecte = 1;
        session_globale.utilisateur = *user;
        free(user);

        afficher_message("\nConnexion réussie !");
        printf("Bienvenue, %s !\n", session_globale.utilisateur.pseudo);
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
    } else {
        afficher_message("Pseudo ou mot de passe incorrect.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
    }
}

/**
 * Menu principal du jeu (après connexion)
 */
void menu_principal(void) {
    int choix = 0;

    while (choix != 6) {
        char options[6][MAX_OPTION_LONGUEUR] = {
            "Jouer une partie",
            "Reprendre une partie",
            "Voir mes statistiques",
            "Voir le classement",
            "Aide",
            "Déconnexion"
        };

        afficher_menu("MENU PRINCIPAL", options, 6);

        if (scanf("%d", &choix) != 1) {
            vider_buffer();
            afficher_message("Erreur : Veuillez entrer un nombre valide.");
            continue;
        }
        vider_buffer();

        switch (choix) {
            case 1:
                menu_jouer();
                break;
            case 2:
                reprendre_partie_sauvegardee();
                break;
            case 3:
                menu_statistiques();
                break;
            case 4:
                menu_classement();
                break;
            case 5:
                afficher_aide();
                break;
            case 6:
                afficher_message("Déconnexion...");
                session_globale.est_connecte = 0;
                break;
            default:
                afficher_message("Choix invalide. Veuillez réessayer.");
        }
    }
}

/**
 * Menu de sélection de la difficulté
 */
void menu_jouer(void) {
    int choix = 0;

    char options[4][MAX_OPTION_LONGUEUR] = {
        "Mode Facile (3-5 lettres, 10 essais)",
        "Mode Moyen (6-8 lettres, 8 essais)",
        "Mode Difficile (9+ lettres, 6 essais)",
        "Retour"
    };

    afficher_menu("CHOISIR LA DIFFICULTÉ", options, 4);

    if (scanf("%d", &choix) != 1) {
        vider_buffer();
        afficher_message("Erreur : Veuillez entrer un nombre valide.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }
    vider_buffer();

    switch (choix) {
        case 1:
            jouer_partie(FACILE);
            break;
        case 2:
            jouer_partie(MOYEN);
            break;
        case 3:
            jouer_partie(DIFFICILE);
            break;
        case 4:
            return;
        default:
            afficher_message("Choix invalide.");
    }
}

/**
 * Boucle de jeu principale
 */
void jouer_partie(Difficulte niveau) {
    // Charger le dictionnaire correspondant
    Dictionnaire *dico = charger_dictionnaire(niveau);

    if (dico == NULL || dico->nb_entrees == 0) {
        afficher_message("Erreur : Impossible de charger le dictionnaire.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }

    // Sélectionner un mot aléatoire
    int index_aleatoire = rand() % dico->nb_entrees;
    EntreeDictionnaire *entree_selectionnee = &dico->entrees[index_aleatoire];

    // Initialiser la partie
    Partie *partie = initialiser_partie(entree_selectionnee, niveau, session_globale.utilisateur.id);

    if (partie == NULL) {
        afficher_message("Erreur lors de l'initialisation de la partie.");
        liberer_dictionnaire(dico);
        return;
    }

    // Boucle de jeu
    while (partie->partie_gagnee == -1) {
        // Afficher l'état du jeu
        afficher_etat_jeu(partie);

        // Demander une lettre
        printf("\nProposez une lettre (ou '?' pour l'indice, '!' pour quitter et sauvegarder) : ");
        char lettre = lire_lettre();

        // Vérifier les commandes spéciales
        if (lettre == '?') {
            if (indice_deja_consulte(partie)) {
                afficher_message("Vous avez déjà utilisé l'indice !");
            } else {
                consulter_indice(partie);
                afficher_indice(partie->indice);
            }
            continue;
        }

        if (lettre == '!') {
            if (sauvegarder_partie(partie)) {
                afficher_message("Partie sauvegardée avec succès !");
                printf("Vous pourrez la reprendre plus tard.\n");
            } else {
                afficher_message("Erreur lors de la sauvegarde de la partie.");
            }

            // Libérer la mémoire et sortir SANS mettre à jour les statistiques
            liberer_partie(partie);
            liberer_dictionnaire(dico);

            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            return;
        }

        // Vérifier si c'est une lettre valide
        if (lettre == '\0' || !isalpha(lettre)) {
            afficher_message("Veuillez entrer une lettre valide (A-Z).");
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            continue;
        }

        // Vérifier si la lettre a déjà été utilisée
        if (est_lettre_utilisee(partie, lettre)) {
            afficher_message("Vous avez déjà proposé cette lettre !");
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            continue;
        }

        // Ajouter la lettre à la liste des lettres utilisées
        ajouter_lettre_utilisee(partie, lettre);

        // Vérifier si la lettre est dans le mot
        int nb_occurrences = verifier_lettre(partie, lettre);

        if (nb_occurrences > 0) {
            // Bonne lettre !
            mettre_a_jour_affichage(partie, lettre);
            printf("\nBien joué ! La lettre '%c' apparaît %d fois.\n",
                   lettre, nb_occurrences);
        } else {
            // Mauvaise lettre
            partie->nb_erreurs++;
            partie->essais_restants--;
            printf("\nDommage ! La lettre '%c' n'est pas dans le mot.\n", lettre);
        }

        // Vérifier si la partie est gagnée
        if (est_victoire(partie)) {
            partie->partie_gagnee = 1;
        }

        // Vérifier si la partie est perdue
        if (est_defaite(partie)) {
            partie->partie_gagnee = 0;
        }

        if (partie->partie_gagnee == -1) {
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
        }
    }

    if (partie->partie_gagnee == 1) {
        afficher_victoire(partie);
        supprimer_sauvegarde(session_globale.utilisateur.id);
    } else if (partie->partie_gagnee == 0) {
        afficher_defaite(partie);
        supprimer_sauvegarde(session_globale.utilisateur.id);
    }

    if (partie->partie_gagnee == 1 || partie->partie_gagnee == 0) {
        Statistiques *stats = charger_statistiques_utilisateur(session_globale.utilisateur.id);
        if (stats != NULL) {
            ajouter_partie_aux_stats(stats, partie);
            free(stats);
        }
    }

    // Libérer la mémoire
    liberer_partie(partie);
    liberer_dictionnaire(dico);
}

/**
 * Reprend une partie précédemment sauvegardée
 */
void reprendre_partie_sauvegardee(void) {
    if (!partie_sauvegardee_existe(session_globale.utilisateur.id)) {
        afficher_message("Aucune partie sauvegardée trouvée.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }

    Partie *partie = charger_partie(session_globale.utilisateur.id);

    if (partie == NULL) {
        afficher_message("Erreur lors du chargement de la partie.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }

    afficher_message("Partie chargée avec succès ! Reprenons...");
    printf("\nAppuyez sur Entrée pour continuer...");
    getchar();

    // Boucle de jeu (identique à jouer_partie)
    while (partie->partie_gagnee == -1) {
        // Afficher l'état du jeu
        afficher_etat_jeu(partie);

        // Demander une lettre
        printf("\nProposez une lettre (ou '?' pour l'indice, '!' pour quitter et sauvegarder) : ");
        char lettre = lire_lettre();

        // Vérifier les commandes spéciales
        if (lettre == '?') {
            if (indice_deja_consulte(partie)) {
                afficher_message("Vous avez déjà utilisé l'indice !");
            } else {
                consulter_indice(partie);
                afficher_indice(partie->indice);
            }
            continue;
        }

        if (lettre == '!') {
            if (sauvegarder_partie(partie)) {
                afficher_message("Partie sauvegardée avec succès !");
                printf("Vous pourrez la reprendre plus tard.\n");
            } else {
                afficher_message("Erreur lors de la sauvegarde de la partie.");
            }

            // Libérer la mémoire et sortir SANS mettre à jour les statistiques
            liberer_partie(partie);

            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            return;
        }

        // Vérifier si c'est une lettre valide
        if (lettre == '\0' || !isalpha(lettre)) {
            afficher_message("Veuillez entrer une lettre valide (A-Z).");
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            continue;
        }

        // Vérifier si la lettre a déjà été utilisée
        if (est_lettre_utilisee(partie, lettre)) {
            afficher_message("Vous avez déjà proposé cette lettre !");
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            continue;
        }

        // Ajouter la lettre à la liste des lettres utilisées
        ajouter_lettre_utilisee(partie, lettre);

        // Vérifier si la lettre est dans le mot
        int nb_occurrences = verifier_lettre(partie, lettre);

        if (nb_occurrences > 0) {
            // Bonne lettre !
            mettre_a_jour_affichage(partie, lettre);
            printf("\nBien joué ! La lettre '%c' apparaît %d fois.\n",
                   lettre, nb_occurrences);
        } else {
            // Mauvaise lettre
            partie->nb_erreurs++;
            partie->essais_restants--;
            printf("\nDommage ! La lettre '%c' n'est pas dans le mot.\n", lettre);
        }

        // Vérifier si la partie est gagnée
        if (est_victoire(partie)) {
            partie->partie_gagnee = 1;
        }

        // Vérifier si la partie est perdue
        if (est_defaite(partie)) {
            partie->partie_gagnee = 0;
        }

        if (partie->partie_gagnee == -1) {
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
        }
    }

    // Afficher le résultat final
    if (partie->partie_gagnee == 1) {
        afficher_victoire(partie);
        supprimer_sauvegarde(session_globale.utilisateur.id);
    } else if (partie->partie_gagnee == 0) {
        afficher_defaite(partie);
        supprimer_sauvegarde(session_globale.utilisateur.id);
    }

    if (partie->partie_gagnee == 1 || partie->partie_gagnee == 0) {
        Statistiques *stats = charger_statistiques_utilisateur(session_globale.utilisateur.id);
        if (stats != NULL) {
            ajouter_partie_aux_stats(stats, partie);
            free(stats);
        }
    }

    // Libérer la mémoire
    liberer_partie(partie);
}

/**
 * Menu des statistiques
 */
void menu_statistiques(void) {
    // Charger les statistiques de l'utilisateur connecté
    Statistiques *stats = charger_statistiques_utilisateur(session_globale.utilisateur.id);

    if (stats == NULL) {
        afficher_message("Erreur lors du chargement des statistiques.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }

    nettoyer_ecran();
    afficher_statistiques(stats);

    printf("\nAppuyez sur Entrée pour continuer...");
    getchar();

    free(stats);
}

/**
 * Menu du classement
 */
void menu_classement(void) {
    int choix = 0;

    char options[3][MAX_OPTION_LONGUEUR] = {
        "Voir le classement complet",
        "Comparer avec un autre joueur",
        "Retour"
    };

    afficher_menu("CLASSEMENT", options, 3);

    if (scanf("%d", &choix) != 1) {
        vider_buffer();
        afficher_message("Erreur : Veuillez entrer un nombre valide.");
        printf("\nAppuyez sur Entrée pour continuer...");
        getchar();
        return;
    }
    vider_buffer();

    switch (choix) {
        case 1: {
            // Générer et afficher le classement complet
            Leaderboard *leaderboard = generer_classement();

            if (leaderboard != NULL) {
                afficher_classement_complet(leaderboard, session_globale.utilisateur.id);
                liberer_classement(leaderboard);
            } else {
                afficher_message("Erreur lors de la génération du classement.");
                printf("\nAppuyez sur Entrée pour continuer...");
                getchar();
            }
            break;
        }
        case 2: {
            // Comparer avec un autre joueur
            char pseudo_adversaire[MAX_PSEUDO];

            printf("\nPseudo du joueur à comparer : ");
            if (fgets(pseudo_adversaire, MAX_PSEUDO, stdin) == NULL) {
                afficher_message("Erreur de saisie.");
                return;
            }
            pseudo_adversaire[strcspn(pseudo_adversaire, "\n")] = '\0';

            Utilisateur *adversaire = trouver_utilisateur_par_pseudo(pseudo_adversaire);

            if (adversaire == NULL) {
                afficher_message("Joueur introuvable.");
                printf("\nAppuyez sur Entrée pour continuer...");
                getchar();
            } else {
                comparer_deux_joueurs(session_globale.utilisateur.id, adversaire->id);
                free(adversaire);
            }
            break;
        }
        case 3:
            return;
        default:
            afficher_message("Choix invalide.");
    }
}

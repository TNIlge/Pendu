#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "user.h"
#include "game.h"
#include "dictionary.h"
#include "display.h"
#include "input.h"
#include "stats.h"
#include "leaderboard.h"
#include "save.h"

// Session globale
static Session session_actuelle = {0};

/**
 * Menu d'authentification
 * Retourne 1 si connexion réussie, 0 pour quitter
 */
int menu_authentification(void) {
    while (1) {
        nettoyer_ecran();

        char options[][MAX_OPTION_LONGUEUR] = {
            "1. Creer un compte",
            "2. Se connecter",
            "3. Quitter"
        };

        afficher_menu("=== JEU DU PENDU - AUTHENTIFICATION ===", options, 3);

        int choix = lire_choix_menu();

        switch (choix) {
            case 1: {
                // Créer un compte
                nettoyer_ecran();
                afficher_message("=== CREATION DE COMPTE ===\n");

                char pseudo[MAX_PSEUDO];
                char mot_de_passe[MAX_PASSWORD];

                printf("Pseudo (max %d caracteres): ", MAX_PSEUDO - 1);
                fgets(pseudo, MAX_PSEUDO, stdin);
                pseudo[strcspn(pseudo, "\n")] = '\0';

                if (strlen(pseudo) == 0) {
                    afficher_message("Pseudo invalide.");
                    attendre_touche();
                    continue;
                }

                if (pseudo_existe(pseudo)) {
                    afficher_message("Ce pseudo existe deja.");
                    attendre_touche();
                    continue;
                }

                printf("Mot de passe (max %d caracteres): ", MAX_PASSWORD - 1);
                fgets(mot_de_passe, MAX_PASSWORD, stdin);
                mot_de_passe[strcspn(mot_de_passe, "\n")] = '\0';

                if (strlen(mot_de_passe) == 0) {
                    afficher_message("Mot de passe invalide.");
                    attendre_touche();
                    continue;
                }

                Utilisateur *nouveau = creer_utilisateur(pseudo, mot_de_passe);

                if (nouveau != NULL) {
                    afficher_message("Compte cree avec succes!");
                    session_actuelle.utilisateur = *nouveau;
                    session_actuelle.est_connecte = 1;
                    free(nouveau);

                    // Initialiser les statistiques pour ce nouvel utilisateur
                    Statistiques *stats = malloc(sizeof(Statistiques));
                    initialiser_statistiques(stats, session_actuelle.utilisateur.id);
                    sauvegarder_statistiques_utilisateur(stats);
                    free(stats);

                    return 1;
                } else {
                    afficher_message("Erreur lors de la creation du compte.");
                    attendre_touche();
                }
                break;
            }

            case 2: {
                // Se connecter
                nettoyer_ecran();
                afficher_message("=== CONNEXION ===\n");

                char pseudo[MAX_PSEUDO];
                char mot_de_passe[MAX_PASSWORD];

                printf("Pseudo: ");
                fgets(pseudo, MAX_PSEUDO, stdin);
                pseudo[strcspn(pseudo, "\n")] = '\0';

                printf("Mot de passe: ");
                fgets(mot_de_passe, MAX_PASSWORD, stdin);
                mot_de_passe[strcspn(mot_de_passe, "\n")] = '\0';

                Utilisateur *user = connecter_utilisateur(pseudo, mot_de_passe);

                if (user != NULL) {
                    afficher_message("Connexion reussie!");
                    session_actuelle.utilisateur = *user;
                    session_actuelle.est_connecte = 1;
                    free(user);
                    attendre_touche();
                    return 1;
                } else {
                    afficher_message("Pseudo ou mot de passe incorrect.");
                    attendre_touche();
                }
                break;
            }

            case 3:
                return 0;

            default:
                afficher_message("Choix invalide.");
                attendre_touche();
        }
    }
}

/**
 * Jouer une partie complète
 */
void jouer_partie(Difficulte niveau) {
    // Charger le dictionnaire
    Dictionnaire *dico = charger_dictionnaire(niveau);
    if (dico == NULL || dico->nb_entrees == 0) {
        afficher_message("Erreur: Impossible de charger le dictionnaire.");
        attendre_touche();
        return;
    }

    // Sélectionner un mot aléatoire
    int index = rand() % dico->nb_entrees;
    EntreeDictionnaire *entree = &dico->entrees[index];

    // Initialiser la partie
    Partie *partie = initialiser_partie(entree, niveau, session_actuelle.utilisateur.id);
    if (partie == NULL) {
        afficher_message("Erreur: Impossible d'initialiser la partie.");
        liberer_dictionnaire(dico);
        attendre_touche();
        return;
    }

    // Boucle de jeu
    while (1) {
        nettoyer_ecran();
        afficher_etat_jeu(partie);

        // Vérifier victoire/défaite
        if (est_victoire(partie)) {
            partie->partie_gagnee = 1;
            afficher_victoire(partie);
            break;
        }

        if (est_defaite(partie)) {
            partie->partie_gagnee = 0;
            afficher_defaite(partie);
            break;
        }

        // Menu d'action
        char options_jeu[][MAX_OPTION_LONGUEUR] = {
            "1. Proposer une lettre",
            "2. Consulter l'indice",
            "3. Sauvegarder et quitter",
            "4. Abandonner"
        };

        afficher_menu("=== QUE VOULEZ-VOUS FAIRE ? ===/n", options_jeu, 4);

        int action = lire_choix_menu();

        switch (action) {
            case 1: {
                // Proposer une lettre
                char lettre = lire_lettre();

                if (est_lettre_utilisee(partie, lettre)) {
                    afficher_message("Vous avez deja propose cette lettre!");
                    attendre_touche();
                    continue;
                }

                ajouter_lettre_utilisee(partie, lettre);

                if (verifier_lettre(partie, lettre)) {
                    mettre_a_jour_affichage(partie, lettre);
                    afficher_message("Bonne lettre!");
                } else {
                    partie->nb_erreurs++;
                    partie->essais_restants--;
                    afficher_message("Mauvaise lettre!");
                }

                attendre_touche();
                break;
            }

            case 2:
                // Consulter l'indice
                if (!indice_deja_consulte(partie)) {
                    consulter_indice(partie);
                    afficher_indice(partie->indice);
                } else {
                    afficher_indice(partie->indice);
                }
                attendre_touche();
                break;

            case 3:
                // Sauvegarder et quitter
                if (sauvegarder_partie(partie)) {
                    afficher_message("Partie sauvegardee avec succes!");
                } else {
                    afficher_message("Erreur lors de la sauvegarde.");
                }
                liberer_partie(partie);
                liberer_dictionnaire(dico);
                attendre_touche();
                return;

            case 4:
                // Abandonner
                if (confirmer_action("Voulez-vous vraiment abandonner cette partie? (o/n): ")) {
                    partie->partie_gagnee = 0;
                    afficher_defaite(partie);
                    goto fin_partie;
                }
                break;

            default:
                afficher_message("Action invalide.");
                attendre_touche();
        }
    }

fin_partie:
    // Mettre à jour les statistiques
    Statistiques *stats = charger_statistiques_utilisateur(session_actuelle.utilisateur.id);
    if (stats != NULL) {
        ajouter_partie_aux_stats(stats, partie);
        sauvegarder_statistiques_utilisateur(stats);
        free(stats);
    }

    // Supprimer la sauvegarde si elle existe
    supprimer_sauvegarde(session_actuelle.utilisateur.id);

    // Libérer la mémoire
    liberer_partie(partie);
    liberer_dictionnaire(dico);

    attendre_touche();
}

/**
 * Menu principal du jeu
 */
void menu_principal(void) {
    while (1) {
        nettoyer_ecran();

        printf("\n=== BIENVENUE %s ===\n\n", session_actuelle.utilisateur.pseudo);

        char options[][MAX_OPTION_LONGUEUR] = {
            "1. Nouvelle partie",
            "2. Reprendre une partie",
            "3. Mes statistiques",
            "4. Classement general",
            "5. Comparer avec un autre joueur",
            "6. Aide",
            "7. Se deconnecter"
        };

        afficher_menu("=== MENU PRINCIPAL ===", options, 7);

        int choix = lire_choix_menu();

        switch (choix) {
            case 1: {
                // Nouvelle partie
                nettoyer_ecran();
                afficher_message("=== CHOISIR LA DIFFICULTE ===\n");
                Difficulte niveau = lire_niveau_difficulte();
                jouer_partie(niveau);
                break;
            }

            case 2: {
                // Reprendre une partie
                if (partie_sauvegardee_existe(session_actuelle.utilisateur.id)) {
                    Partie *partie = charger_partie(session_actuelle.utilisateur.id);
                    if (partie != NULL) {
                        afficher_message("Partie chargee avec succes!");
                        attendre_touche();

                        // Reprendre le jeu avec la partie chargée
                        Dictionnaire *dico = charger_dictionnaire(partie->niveau);

                        while (1) {
                            nettoyer_ecran();
                            afficher_etat_jeu(partie);

                            /*if (est_victoire(partie)) {
                                partie->partie_gagnee = 1;
                                afficher_victoire(partie);
                                break;
                            }

                            if (est_defaite(partie)) {
                                partie->partie_gagnee = 0;
                                afficher_defaite(partie);
                                break;
                            }*/

                            char options_jeu[][MAX_OPTION_LONGUEUR] = {
                                "1. Proposer une lettre",
                                "2. Consulter l'indice",
                                "3. Sauvegarder et quitter",
                                "4. Abandonner"
                            };

                            afficher_menu("=== QUE VOULEZ-VOUS FAIRE ? ===\n", options_jeu, 4);

                            int action = lire_choix_menu();

                            switch (action) {
                                case 1: {
                                    char lettre = lire_lettre();

                                    if (est_lettre_utilisee(partie, lettre)) {
                                        afficher_message("Vous avez deja propose cette lettre!");
                                        attendre_touche();
                                        continue;
                                    }

                                    ajouter_lettre_utilisee(partie, lettre);

                                    if (verifier_lettre(partie, lettre)) {
                                        mettre_a_jour_affichage(partie, lettre);
                                        afficher_message("Bonne lettre!");
                                    } else {
                                        partie->nb_erreurs++;
                                        partie->essais_restants--;
                                        afficher_message("Mauvaise lettre!");
                                    }

                                    attendre_touche();
                                    break;
                                }

                                case 2:
                                    if (!indice_deja_consulte(partie)) {
                                        consulter_indice(partie);
                                        afficher_indice(partie->indice);
                                    } else {
                                        afficher_indice(partie->indice);
                                    }

                                    attendre_touche();
                                    break;

                                case 3:
                                    if (sauvegarder_partie(partie)) {
                                        afficher_message("Partie sauvegardee avec succes!");
                                    } else {
                                        afficher_message("Erreur lors de la sauvegarde.");
                                    }
                                    liberer_partie(partie);
                                    liberer_dictionnaire(dico);
                                    attendre_touche();
                                    goto fin_reprise;

                                case 4:
                                    if (confirmer_action("Voulez-vous vraiment abandonner? (o/n): ")) {
                                        partie->partie_gagnee = 0;
                                        afficher_defaite(partie);
                                        goto fin_partie_reprise;
                                    }
                                    break;
                            }
                        }

fin_partie_reprise:
                        // Mettre à jour les statistiques
                        Statistiques *stats = charger_statistiques_utilisateur(session_actuelle.utilisateur.id);
                        if (stats != NULL) {
                            ajouter_partie_aux_stats(stats, partie);
                            sauvegarder_statistiques_utilisateur(stats);
                            free(stats);
                        }

                        supprimer_sauvegarde(session_actuelle.utilisateur.id);
                        liberer_partie(partie);
                        liberer_dictionnaire(dico);
                        attendre_touche();

fin_reprise:;
                    } else {
                        afficher_message("Erreur lors du chargement de la partie.");
                        attendre_touche();
                    }
                } else {
                    afficher_message("Aucune partie sauvegardee.");
                    attendre_touche();
                }
                break;
            }

            case 3: {
                // Mes statistiques
                nettoyer_ecran();
                Statistiques *stats = charger_statistiques_utilisateur(session_actuelle.utilisateur.id);
                if (stats != NULL) {
                    afficher_statistiques(stats);
                    free(stats);
                } else {
                    afficher_message("Aucune statistique disponible.");
                }
                attendre_touche();
                break;
            }

            case 4: {
                // Classement général
                nettoyer_ecran();
                Leaderboard *leaderboard = generer_classement();
                if (leaderboard != NULL) {
                    afficher_classement_complet(leaderboard, session_actuelle.utilisateur.id);
                    sauvegarder_classement(leaderboard);
                    liberer_classement(leaderboard);
                } else {
                    afficher_message("Erreur lors de la generation du classement.");
                }
                attendre_touche();
                break;
            }

            case 5: {
                // Comparer avec un autre joueur
                nettoyer_ecran();
                afficher_message("=== COMPARER AVEC UN AUTRE JOUEUR ===\n");

                char pseudo_adversaire[MAX_PSEUDO];
                printf("Pseudo du joueur a comparer: ");
                fgets(pseudo_adversaire, MAX_PSEUDO, stdin);
                pseudo_adversaire[strcspn(pseudo_adversaire, "\n")] = '\0';

                Utilisateur *adversaire = trouver_utilisateur_par_pseudo(pseudo_adversaire);

                if (adversaire == NULL) {
                    afficher_message("Joueur introuvable.");
                    attendre_touche();
                } else if (adversaire->id == session_actuelle.utilisateur.id) {
                    afficher_message("Vous ne pouvez pas vous comparer a vous-meme!");
                    free(adversaire);
                    attendre_touche();
                } else {
                    comparer_deux_joueurs(session_actuelle.utilisateur.id, adversaire->id);
                    free(adversaire);
                    attendre_touche();
                }
                break;
            }

            case 6:
                // Aide
                nettoyer_ecran();
                afficher_aide();
                attendre_touche();
                break;

            case 7:
                // Se déconnecter
                if (confirmer_action("Voulez-vous vraiment vous deconnecter? (o/n): ")) {
                    session_actuelle.est_connecte = 0;
                    return;
                }
                break;

            default:
                afficher_message("Choix invalide.");
                attendre_touche();
        }
    }
}

/**
 * Point d'entrée principal
 */
int main(void) {
    // Initialiser le générateur aléatoire
    srand(time(NULL));

    // Boucle principale du programme
    while (1) {
        if (!session_actuelle.est_connecte) {
            if (!menu_authentification()) {
                // L'utilisateur veut quitter
                nettoyer_ecran();
                afficher_message("Merci d'avoir joue au Pendu! A bientot!");
                break;
            }
        }

        menu_principal();
    }

    return 0;
}

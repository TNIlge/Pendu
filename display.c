#include "config.h"
#include "display.h"

void nettoyer_ecran(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void afficher_message(const char *message) {
    printf("%s\n", message);
}

void afficher_menu(const char* titre, char options[][MAX_OPTION_LONGUEUR], int nb_options) {
    nettoyer_ecran(); //nettoyer l'ecran pour un affichage clair
    const char *jeu = "JEU DU PENDU - BIENVENUE";
    printf("============================================================\n");
    printf("| %-56.58s |\n", " ");
    printf("|              %-30.30s              |\n", jeu);
    printf("| %-56.58s |\n", " ");
    printf("+----------------------------------------------------------+\n");
    printf("|              %-30.30s              |\n", titre);
    printf("+----------------------------------------------------------+\n");
    for (int i = 0; i < nb_options; i++) {
        printf("|  [%d] %-51.50s |\n", i + 1, options[i]);
    }

    printf("============================================================\n");
    printf("\nVotre choix : "); //Saisie utilisateur
}

static const char* obtenir_nom_difficulte(Difficulte niveau) { //Redéfinir les dificultés
    switch(niveau) {
        case FACILE: return "FACILE";
        case MOYEN: return "MOYEN";
        case DIFFICILE: return "DIFFICILE";
        default: return "INCONNU";
    }
}

void afficher_pendu(int nb_erreurs) {
    printf("\n");

    switch(nb_erreurs) {
        case 0:  // Aucune erreur
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |\n");
            printf("   |\n");
            printf("   |\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 1:  // 1 erreur
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |\n");
            printf("   |\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 2:  // 2 erreurs
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |    |\n");
            printf("   |\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 3:  // 3 erreurs
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |   /|\n");
            printf("   |\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 4:  // 4 erreurs
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |   /|\\\n");
            printf("   |\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 5:  // 5 erreurs
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |   /|\\\n");
            printf("   |   /\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 6:  // 6 erreurs - fin du niveau difficile
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |   /|\\\n");
            printf("   |   / \\\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 7:  // 7 erreurs
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |  _/|\\\n");
            printf("   |   / \\\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 8:  // 8 erreurs - fin du niveau moyen
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |  _/|\\_\n");
            printf("   |   / \\\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 9:  // 9 erreurs
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |    O\n");
            printf("   |  _/|\\_\n");
            printf("   |  _/ \\\n");
            printf("   |\n");
            printf("  _|_\n");
            break;

        case 10:  // 10 erreurs - fin du niveau facile
            printf("   ______\n");
            printf("   |    |\n");
            printf("   |   X_X\n");
            printf("   |  _/|\\_\n");
            printf("   |  _/ \\_\n");
            printf("   |  \n");
            printf("  _|_\n");
            break;

        default:  // Par sécurité
            printf("Erreur : nombre d'erreurs invalide!\n");
    }

    printf("\n");
}

void afficher_mot(const char *mot, const int *lettres_trouvees) {
    if (mot == NULL) return;

    printf("\nMot à deviner : ");

    for (int i = 0; mot[i] != '\0'; i++) {
        if (lettres_trouvees != NULL && lettres_trouvees[i]) {
            printf("%c ", mot[i]);  // Affiche la lettre devinée
        }else {
            printf("_ "); //lettre.s non devinée.s
        }
    }

    printf("\n");
}

void afficher_lettres_utilisees(Partie *partie){
    if (partie == NULL) {
        afficher_message("Erreur: La structure Partie est NULL.");
        return;
    }

    printf(" Lettres deja jouees : ");

    // Si aucune lettre n'a été jouée
    if (partie->nb_lettres_utilisees == 0) {
        printf("[ Aucune ]\n");
        return;
    }

    //
    printf("[ ");
    for (int i = 0; i < partie->nb_lettres_utilisees; i++) {
        // Les lettres sont affichées en majuscules pour une meilleure cohérence visuelle
        printf("%c ", toupper(partie->lettres_utilisees[i]));
    }

    printf("]\n");
}

void formater_duree(int secondes, char *buffer){
    if (buffer == NULL) {
        // En cas d'erreur ou de buffer non alloué, on quitte.
        return;
    }

    if (secondes < 0) {
        secondes = 0;
    }

    // Calcul des minutes et des secondes restantes
    int minutes = secondes / 60;
    int secondes_restantes = secondes % 60;

    if (minutes > 99) {
        minutes = 99;
    }

    snprintf(buffer, 9, "%02d:%02d", minutes, secondes_restantes);
}

void afficher_etat_jeu(Partie *partie) {
    if (partie == NULL) return;

    /* Si la partie est terminée, on délègue */
    if (partie->partie_gagnee == 1) {
        afficher_victoire(partie);
        return;
    }

    if (partie->partie_gagnee == 0 && partie->essais_restants == 0) {
        afficher_defaite(partie);
        return;
    }

    time_t maintenant = time(NULL);
    int duree_secondes = (int)difftime(maintenant, partie->heure_debut);

    char temps_buffer[9];
    formater_duree(duree_secondes, temps_buffer);

    nettoyer_ecran();

    printf("==============================================================\n");
    printf(" Niveau : %-10s | Essais : %d/%d  | Temps : %s\n",
           obtenir_nom_difficulte(partie->niveau),
           partie->nb_erreurs,
           partie->essais_max,
           temps_buffer);
    printf("==============================================================\n");

    afficher_pendu(partie->nb_erreurs); //afficher le dessin du pendu

    printf("\n\tMot a trouver :  ");
    if (partie->mot_affiche != NULL){
        for (int i = 0; partie->mot_affiche[i] != '\0'; i++) {
            printf("%c ", partie->mot_affiche[i]);
        }
    }

    printf("\n\n");

    // Affiche les lettres déjà utilisées
    printf("--------------------------------------------------------------\n");
    afficher_lettres_utilisees(partie);

    // Affiche si l’indice a été utilisé
    if (partie->indice_utilise) {
        printf("Indice : %s\n", partie->indice);
    } else {
        printf(" Indice : Disponible (Tapez '?' pour l'utiliser)\n");
    }

    printf("==============================================================\n");

}

void afficher_aide(void){
    nettoyer_ecran();

    // Titre
    printf("============================================================\n");
    printf("| %-56.58s |\n", " ");
    printf("|                      AIDE ET REGLES                      |\n");
    printf("| %-56.58s |\n", " ");
    printf("============================================================\n");

    // Section 1 : Règles du jeu
    printf("\n--- Regles du Jeu ---\n");
    printf("Le but est de deviner le mot secret lettre par lettre.\n");
    printf("Chaque mauvaise proposition ajoute un element au pendu.\n");
    printf("Le nombre d'erreurs maximales depend de la difficulte :\n");
    printf("  - FACILE : %d erreurs\n", ESSAIS_FACILE);
    printf("  - MOYEN  : %d erreurs\n", ESSAIS_MOYEN);
    printf("  - DIFFICILE : %d erreurs\n", ESSAIS_DIFFICILE);
    printf("Si le pendu est complet avant de trouver le mot, vous perdez.\n");

    // Section 2 : Commandes en Jeu
    printf("\n--- Commandes en Jeu ---\n");
    printf("* Proposer une lettre : Tapez une seule lettre (A-Z).\n");
    printf("* Utiliser l'indice   : Tapez '?' (Une seule fois par partie).\n");
    printf("* Sauvegarder et Quitter: Tapez '!' (Disponible uniquement en mode partie).\n");

    // Section 3 : Informations générales
    printf("\n--- Informations ---\n");
    printf("Le jeu ne fait pas la difference entre majuscules et minuscules.\n");
    printf("Les lettres deja proposees sont ignorees.\n");
    printf("Votre temps est chronometre et influence votre classement.\n");

    printf("\n============================================================\n");
    printf("Appuyez sur Entree pour revenir au menu.\n");
    printf("============================================================\n");

    // Pause pour que l'utilisateur puisse lire l'aide
    getchar();
}

void afficher_statistiques(Statistiques *stats){
    if (stats == NULL) {
        printf("Aucune statistique disponible.\n");
        return;
    }

    printf("\n======================= STATISTIQUES ======================\n");

    /* -------- Résumé global -------- */
    printf("Parties jouees      : %d\n", stats->total_parties);
    printf("Victoires           : %d\n", stats->total_victoires);
    printf("Defaites            : %d\n", stats->total_defaites);
    printf("Indices utilises    : %d\n", stats->total_indices_utilises);

    printf("\n--- Moyennes ---\n");
    printf("Taux de victoire    : %.2f %%\n", stats->taux_victoire);
    printf("Essais moyens       : %.2f\n", stats->moyenne_essais);
    printf("Duree moyenne       : %.2f secondes\n", stats->moyenne_duree);

     /* -------- Stats par niveau -------- */
    printf("\n--- Par niveau ---\n");

    printf("FACILE     : %d parties | %d victoires\n",
           stats->parties_facile, stats->victoires_facile);

    printf("MOYEN      : %d parties | %d victoires\n",
           stats->parties_moyen, stats->victoires_moyen);

    printf("DIFFICILE  : %d parties | %d victoires\n",
           stats->parties_difficile, stats->victoires_difficile);

    /* -------- Historique récent -------- */
    if (stats->nb_parties_recentes > 0) {
        printf("\n--- Parties recentes ---\n");

        for (int i = 0; i < stats->nb_parties_recentes; i++) {
            StatPartie p = stats->parties_recentes[i];

            printf("%d. Mot: %-10s | %s | Essais: %d | Indice: %s\n",
                   i + 1,
                   p.mot,
                   p.victoire ? "Victoire" : "Défaite",
                   p.essais_utilises,
                   p.indice_utilise ? "Oui" : "Non");
        }
    } else {
        printf("\nAucune partie récente.\n");
    }

    printf("============================================================\n");
}

void afficher_victoire(Partie *partie){
    if (partie == NULL) return;

    nettoyer_ecran();

    /* Calcul du temps de jeu */
    time_t maintenant = time(NULL);
    int duree_secondes = (int)difftime(maintenant, partie->heure_debut);

    char temps_buffer[9];
    formater_duree(duree_secondes, temps_buffer);

    printf("============================================================\n");
    printf("| %-56s |\n", " ");
    printf("|                      VICTOIRE !!!                       |\n");
    printf("| %-56s |\n", " ");
    printf("============================================================\n\n");

    printf("Bravo ! Vous avez trouve le mot secret.\n\n");

    printf("Mot trouve       : %s\n", partie->mot_secret);
    printf("Difficulte       : %s\n", obtenir_nom_difficulte(partie->niveau));
    printf("Erreurs commises : %d / %d\n",
           partie->nb_erreurs, partie->essais_max);
    printf("Temps de jeu     : %s\n", temps_buffer);

    if (partie->indice_utilise) {
        printf("Indice utilise   : Oui\n");
    } else {
        printf("Indice utilise   : Non\n");
    }

    printf("\n============================================================\n");
    printf("Appuyez sur Entree pour continuer...\n");
    printf("============================================================\n");

    getchar();
}


void afficher_defaite(Partie *partie){
    if (partie == NULL) return;

    nettoyer_ecran();

    /* Calcul du temps de jeu */
    time_t maintenant = time(NULL);
    int duree_secondes = (int)difftime(maintenant, partie->heure_debut);

    char temps_buffer[9];
    formater_duree(duree_secondes, temps_buffer);

    printf("============================================================\n");
    printf("| %-56s |\n", " ");
    printf("|                       DEFAITE  T_T                       |\n");
    printf("| %-56s |\n", " ");
    printf("============================================================\n\n");

    /* Affichage du pendu complet */
    afficher_pendu(partie->nb_erreurs);

    printf("Dommage... Vous avez perdu.\n\n");

    printf("Mot secret        : %s\n", partie->mot_secret);
    printf("Difficulte        : %s\n", obtenir_nom_difficulte(partie->niveau));
    printf("Erreurs commises  : %d / %d\n",
           partie->nb_erreurs, partie->essais_max);
    printf("Temps de jeu      : %s\n", temps_buffer);

    if (partie->indice_utilise) {
        printf("Indice utilise    : Oui\n");
    } else {
        printf("Indice utilise    : Non\n");
    }

    printf("\n============================================================\n");
    printf("Appuyez sur Entree pour continuer...\n");
    printf("============================================================\n");

    getchar();
}


void afficher_indice(const char *indice){
    nettoyer_ecran();

    printf("============================================================\n");
    printf("| %-56s |\n", " ");
    printf("|                        INDICE                             |\n");
    printf("| %-56s |\n", " ");
    printf("============================================================\n\n");

    if (indice == NULL || strlen(indice) == 0) {
        printf("Aucun indice disponible pour ce mot.\n");
    } else {
        printf("Indice :\n");
        printf("  -> %s\n", indice);
    }

    printf("\n============================================================\n");
    printf("L'indice ne peut etre utilise qu'une seule fois.\n");
    printf("Appuyez sur Entree pour revenir au jeu...\n");
    printf("============================================================\n");

    getchar();
}

void afficher_classement_complet(Leaderboard *leaderboard, int id_utilisateur){

    if (leaderboard == NULL || leaderboard->nb_joueurs == 0) {
        printf("Aucun classement disponible.\n");
        return;
    }

    nettoyer_ecran();

    printf("============================================================\n");
    printf("| %-56s |\n", " ");
    printf("|                      CLASSEMENT COMPLET                 |\n");
    printf("| %-56s |\n", " ");
    printf("============================================================\n\n");

    printf("%-5s %-20s %-10s %-10s %-10s\n",
           "Rang", "Pseudo", "Parties", "Victoires", "Taux %");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < leaderboard->nb_joueurs; i++) {
        EntreeClassement e = leaderboard->classement[i];

        // Mettre le joueur courant en valeur
        if (e.id_utilisateur == id_utilisateur) {
            printf("-> "); // flèche pour l'utilisateur
        } else {
            printf("   ");
        }

        printf("%-3d %-20s %-10d %-10d %-9.2f\n",
               e.rang,
               e.pseudo,
               e.total_parties,
               e.total_victoires,
               e.taux_victoire);
    }

    printf("\n============================================================\n");
    printf("Appuyez sur Entree pour revenir au menu...\n");
    printf("============================================================\n");

    getchar(); // Pause
}

void afficher_comparaison(Statistiques *stats1, Statistiques *stats2, const char *pseudo1, const char *pseudo2, int rang1, int rang2){
    if (stats1 == NULL || stats2 == NULL) {
        afficher_message("Erreur: Donnees de statistiques manquantes pour la comparaison.");
        return;
    }

    nettoyer_ecran();
    char buffer_duree1[9]; // Pour J1
    char buffer_duree2[9]; // Pour J2

    // --- 1. Titre ---
    printf("========================================================================================\n");
    printf("| %-86.86s |\n", " ");
    printf("|                              COMPARATEUR DE JOUEURS                                    |\n");
    printf("| %-86.86s |\n", " ");
    printf("========================================================================================\n");

    // --- 2. En-tête des joueurs ---
    printf("| J1: %-30s (Rang: %-3d) | J2: %-30s (Rang: %-3d) |\n",
           pseudo1, rang1, pseudo2, rang2);
    printf("----------------------------------------------------------------------------------------\n");

    // --- 3. Statistiques Générales ---
    printf("| %-42s | %-42s |\n", "Parties Totales", "Parties Totales");
    printf("| %-42d | %-42d |\n", stats1->total_parties, stats2->total_parties);
    printf("----------------------------------------------------------------------------------------\n");

    printf("| %-42s | %-42s |\n", "Total Victoires", "Total Victoires");
    printf("| %-42d | %-42d |\n", stats1->total_victoires, stats2->total_victoires);
    printf("----------------------------------------------------------------------------------------\n");

    printf("| %-42s | %-42s |\n", "Taux de Victoire", "Taux de Victoire");
    printf("| %-41.2f%% | %-41.2f%% |\n", stats1->taux_victoire, stats2->taux_victoire);
    printf("----------------------------------------------------------------------------------------\n");

    printf("| %-42s | %-42s |\n", "Moy. Erreurs/Partie", "Moy. Erreurs/Partie");
    printf("| %-42.2f | %-42.2f |\n", stats1->moyenne_essais, stats2->moyenne_essais);
    printf("----------------------------------------------------------------------------------------\n");

    // Formatage et affichage de la durée moyenne
    formater_duree((int)stats1->moyenne_duree, buffer_duree1);
    formater_duree((int)stats2->moyenne_duree, buffer_duree2);
    printf("| %-42s | %-42s |\n", "Duree Moy. / Partie", "Duree Moy. / Partie");
    printf("| %-42s | %-42s |\n", buffer_duree1, buffer_duree2);
    printf("========================================================================================\n");

    // --- 4. Conclusion / Indice ---
    if (stats1->taux_victoire > stats2->taux_victoire) {
        printf("\n Avantage : %s (Meilleur taux de victoire)\n", pseudo1);
    } else if (stats2->taux_victoire > stats1->taux_victoire) {
        printf("\n Avantage : %s (Meilleur taux de victoire)\n", pseudo2);
    } else {
        printf("\n Les deux joueurs sont a egalite en taux de victoire.\n");
    }

    printf("\nAppuyez sur Entree pour revenir au menu.\n");
    printf("========================================================================================\n");

    // Pause obligatoire
    getchar();
}










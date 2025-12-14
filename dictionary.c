#include "dictionary.h"
#include "config.h"
#include <stdio.h>
Dictionnaire* charger_dictionnaire(Difficulte niveau)
{
    const char* nom_fichier = NULL;
// Déterminer le nom du fichier selon le niveau
    switch (niveau)
    {
    case FACILE:
        nom_fichier = "data/facile.txt";
        break;
    case MOYEN:
        nom_fichier = "data/moyen.txt";
        break;
    case DIFFICILE:
        nom_fichier = "data/difficile.txt";
        break;
    default:
        printf("Erreur: Niveau de difficulté inconnu (%d)\n", niveau);
        return NULL;
    }

    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier)
    {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }

    int nombre_lignes = 0;
    char ligne[250];

    while (fgets(ligne, sizeof(ligne), fichier))
    {
        nombre_lignes++;
    }

    rewind(fichier);

    Dictionnaire* dictionnaire = malloc(sizeof(Dictionnaire));
    dictionnaire->entrees = malloc(sizeof(EntreeDictionnaire) * nombre_lignes);
    dictionnaire->nb_entrees = 0;
    dictionnaire->type = (TypeDictionnaire)niveau;
    strcpy(dictionnaire->nom_fichier, nom_fichier);

    int i = 0;
    while(fgets(ligne, sizeof(ligne), fichier) && i < nombre_lignes)
    {
        ligne[strcspn(ligne, "\n")] = '\0';

        char *mot = strtok(ligne, "|");
        char *indice = strtok(NULL, "|");

        if (mot == NULL || indice == NULL)
        {
            printf("Format incorrect: %s\n", ligne);
            continue;  // Passer à la ligne suivante
        }

        char *ptr = mot;
        while (*ptr)
        {
            *ptr = toupper(*ptr);
            ptr++;
        }
        strncpy(dictionnaire->entrees[i].mot, mot, MAX_LONGUEUR_MOT - 1);
        dictionnaire->entrees[i].mot[MAX_LONGUEUR_MOT - 1] = '\0';

        strncpy(dictionnaire->entrees[i].indice, indice, MAX_INDICE - 1);
        dictionnaire->entrees[i].indice[MAX_INDICE - 1] = '\0';

        i++;
    }

    dictionnaire->nb_entrees = i;
    fclose(fichier);
    return dictionnaire;

}


char* selectionner_mot_aleatoire(Dictionnaire *dico)
{
    if (dico == NULL || dico->nb_entrees == 0)
    {
        return NULL;
    }
    int index_aleatoire = rand() % dico->nb_entrees;
    return dico->entrees[index_aleatoire].mot;
}


void liberer_dictionnaire(Dictionnaire *dico)
{
    if (dico)
    {
        if (dico->entrees)
        {
            free(dico->entrees);
        }
        free(dico);
    }
}
int valider_longueur_mot(const char *mot, Difficulte niveau)
{
    int longueur = strlen(mot);
    switch (niveau)
    {
    case FACILE:
        return (longueur >= 3 && longueur <= 5);
    case MOYEN:
        return (longueur >= 6 && longueur <= 8);
    case DIFFICILE:
        return (longueur >= 9);
    default:
        return 0;
    }
}





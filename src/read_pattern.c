#include <stdio.h>
#include <stdlib.h>

char *lire_fichier_txt(const char *chemin)
{
    FILE *fichier = fopen(chemin, "r");

    if (fichier == NULL)
    {
        perror("Erreur ouverture fichier");
        return NULL;
    }

    // Déterminer la taille du fichier
    fseek(fichier, 0, SEEK_END);
    long taille = ftell(fichier);
    rewind(fichier);

    if (taille < 0)
    {
        fclose(fichier);
        return NULL;
    }

    // Allouer la mémoire (+1 pour le caractère '\0')
    char *contenu = malloc((size_t)taille + 1);

    if (contenu == NULL)
    {
        fclose(fichier);
        return NULL;
    }

    // Lire le contenu
    size_t lus = fread(contenu, 1, (size_t)taille, fichier);
    contenu[lus] = '\0';

    fclose(fichier);

    return contenu;
}
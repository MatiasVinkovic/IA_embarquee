#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Transforme le texte lu en 48 pixels (0/1) + le label. Car le perceptron ne sait pas lire le texte, il ne sait que lire des 0 et des 1.
int texte_vers_motif(const char *contenu, int x[48], int *label)
{
    int n = 0;                      /* nombre de pixels lus */
    const char *p = contenu;        /* on avance caractère par caractère */

    while (*p != '\0' && n < 48) {
        if (*p == '.'){
            x[n] = 0;
            n++;                    /* on ne compte que les vrais pixels */
        }
        else if (*p == '*') {
            x[n] = 1;
            n++;
        }
        p++;                        /* les '\n' sont sautés sans être comptés */
    }

    if (n != 48) return -1;         // motif incomplet 

    while (isspace((unsigned char)*p)) p++;
    char *fin;
    long valeur = strtol(p, &fin, 10);
    if (fin == p || valeur < 0 || valeur > 9) return -1;
    while (isspace((unsigned char)*fin)) fin++;
    if (*fin != '\0') return -1;

    *label = (int)valeur;           // ce qui reste : la 9e ligne
    return 0;
}

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
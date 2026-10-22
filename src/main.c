#include <stdio.h>
#include <stdlib.h>
#include "read_pattern.h"

int main(void)
{
    char *contenu = lire_fichier_txt("../patterns/zero.txt");

    if (contenu != NULL)
    {
        printf("%s\n", contenu);
        free(contenu);
    }

    return 0;
}
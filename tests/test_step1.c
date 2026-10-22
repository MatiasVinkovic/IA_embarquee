#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "read_pattern.h"
#include "perceptron.h"

int main(void)
{
    /* --- Test lecture --- */
    int x[48], label;
    char *contenu = lire_fichier_txt("patterns/zero.txt");
    if (contenu == NULL) return 1;
    if (texte_vers_motif(contenu, x, &label) != 0) { free(contenu); return 1; }
    free(contenu);

    int noirs = 0;
    for (int j = 0; j < 48; j++) noirs += x[j];
    printf("zero : label = %d, pixels noirs = %d (attendu 0 et 16)\n", label, noirs);

    /* --- Test calcul papier : x=[1,0], w=[0.2,-0.1], b=-0.5, d=1 --- */
    Perceptron p = {0};          /* tout à 0 */
    p.w[0] = 0.2;
    p.w[1] = -0.1;
    p.b = -0.5;

    int xt[48] = {0};
    xt[0] = 1;

    double a = score(&p, xt);
    printf("a = %.2f (attendu -0.30)\n", a);
    printf("y = %d (attendu 0)\n", predict(a));

    int e = update(&p, xt, 1, 0.01);
    printf("e = %d (attendu 1)\n", e);
    printf("w0 = %.2f (attendu 0.21)\n", p.w[0]);
    printf("b  = %.2f (attendu -0.49)\n", p.b);

    return 0;
}
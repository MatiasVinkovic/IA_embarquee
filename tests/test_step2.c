#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "read_pattern.h"
#include "perceptron.h"


int main(void)
{
    int x[48], label;

    char *contenu = lire_fichier_txt("patterns/zero.txt");

    if (contenu == NULL) return 1;

    if (texte_vers_motif(contenu, x, &label) != 0) {
        free(contenu);
        return 1;
    }

    free(contenu);

    int noirs = 0;

    for (int j = 0; j < 48; j++) {
        noirs += x[j];
    }

    printf("zero : label = %d, pixels noirs = %d (attendu 0 et 16)\n",
           label, noirs);

    Perceptron p = {0};

    p.w[0] = 0.2;
    p.w[1] = -0.1;
    p.b = -0.5;

    int xt[48] = {0};
    xt[0] = 1;

    double a = score(&p, xt);

    printf("a = %.2f (attendu -0.30)\n", a);
    printf("y = %d (attendu 0)\n", predict_step2(a));

    double delta = update_step2(&p, xt, 1, 0.01);

    printf("delta = %.2f (attendu -1.30)\n", delta);
    printf("w0 = %.3f (attendu 0.213)\n", p.w[0]);
    printf("b  = %.3f (attendu -0.487)\n", p.b);

    Rng r1, r2;

    rng_seed(&r1, 42);
    rng_seed(&r2, 42);

    int identiques = 1;

    for (int i = 0; i < 100; i++) {
        if (rng_next(&r1) != rng_next(&r2)) {
            identiques = 0;
        }
    }

    printf("meme graine -> meme suite : %s (attendu OUI)\n",
           identiques ? "OUI" : "NON");

    Rng r;
    Perceptron q;

    rng_seed(&r, 42);
    perceptron_init(&q, &r);

    int dans_bornes = 1;

    for (int j = 0; j < 48; j++) {
        if (q.w[j] < -0.1 || q.w[j] > 0.1) {
            dans_bornes = 0;
        }
    }

    printf("poids dans [-0.1;0.1] : %s (attendu OUI)\n",
           dans_bornes ? "OUI" : "NON");

    printf("b = %.2f (attendu -0.50)\n", q.b);
    printf("w[0..2] = %.4f %.4f %.4f\n",
           q.w[0], q.w[1], q.w[2]);


    return 0;
}
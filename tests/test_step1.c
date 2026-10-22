#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "read_pattern.h"
#include "perceptron.h"

static int question2(void)
{
    int x[48];
    int d;
    char *contenu = lire_fichier_txt("patterns/un.txt");
    if (contenu == NULL) return 1;
    int statut = texte_vers_motif(contenu, x, &d);
    free(contenu);
    if (statut != 0) return 1;

    Rng rng;
    Perceptron modele;
    rng_seed(&rng, 42);
    perceptron_init(&modele, &rng);

    double a = score(&modele, x);
    int y = predict(a);
    int erreur = d - y;
    printf("Etape 1, un seul exemple (un.txt) : a = %.6f, y = %d, e = %d\n",
           a, y, erreur);

    update(&modele, x, d, 0.01);
    printf("Poids modifiés après cette mise à jour :\n");
    for (int j = 0; j < 48; j++) {
        if (x[j] != 0 && erreur != 0)
            printf("w[%d] = %.6f\n", j, modele.w[j]);
    }
    printf("Biais après cette mise à jour : %.6f\n", modele.b);
    return 0;
}

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

        /* --- Test RNG : même graine = même suite --- */
    Rng r1, r2;
    rng_seed(&r1, 42);
    rng_seed(&r2, 42);
    int identiques = 1;
    for (int i = 0; i < 100; i++) {
        if (rng_next(&r1) != rng_next(&r2)) identiques = 0;
    }
    printf("meme graine -> meme suite : %s (attendu OUI)\n", identiques ? "OUI" : "NON");

    /* --- Test init : poids dans [-0.1 ; 0.1], b = -0.5 --- */
    Rng r;
    rng_seed(&r, 42);
    Perceptron q;
    perceptron_init(&q, &r);
    int dans_bornes = 1;
    for (int j = 0; j < 48; j++) {
        if (q.w[j] < -0.1 || q.w[j] > 0.1) dans_bornes = 0;
    }
    printf("poids dans [-0.1;0.1] : %s (attendu OUI)\n", dans_bornes ? "OUI" : "NON");
    printf("b = %.2f (attendu -0.50)\n", q.b);
    printf("w[0..2] = %.4f %.4f %.4f\n", q.w[0], q.w[1], q.w[2]);

    if (question2() != 0) return 1;

    return 0;
}
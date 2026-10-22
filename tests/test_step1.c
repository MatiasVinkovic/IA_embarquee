#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "read_pattern.h"
#include "perceptron.h"

static int question2(void)
{
    int exemples[2][48];
    int labels[2];
    const char *chemins[] = {"patterns/zero.txt", "patterns/un.txt"};
    Rng rng;
    Perceptron modele;
    rng_seed(&rng, 42);
    perceptron_init(&modele, &rng);

    for (int i = 0; i < 2; i++) {
        char *contenu = lire_fichier_txt(chemins[i]);
        if (contenu == NULL) return 1;
        int statut = texte_vers_motif(contenu, exemples[i], &labels[i]);
        free(contenu);
        if (statut != 0) return 1;
    }

    for (int epoque = 0; epoque < 1000; epoque++) {
        for (int i = 0; i < 2; i++) update(&modele, exemples[i], labels[i], 0.01);
        if (predict(score(&modele, exemples[0])) == labels[0] &&
            predict(score(&modele, exemples[1])) == labels[1]) break;
    }

    int propres_corrects = 0;
    for (int i = 0; i < 2; i++)
        propres_corrects += predict(score(&modele, exemples[i])) == labels[i];
    if (propres_corrects != 2) {
        fprintf(stderr, "Impossible d'apprendre les deux motifs propres.\n");
        return 1;
    }

    FILE *fichier = fopen("noise.csv", "r");
    if (fichier == NULL) {
        perror("Ouverture de noise.csv");
        return 1;
    }

    char ligne[256];
    char pixels[49];
    int seed, chiffre, pourcentage, copie, inverses;
    int bruit_total = 0;
    int bruit_corrects = 0;
    if (fgets(ligne, sizeof ligne, fichier) == NULL) {
        fclose(fichier);
        return 1;
    }

    while (fgets(ligne, sizeof ligne, fichier) != NULL) {
        if (sscanf(ligne, "%d,%d,%d,%d,%d,%48s", &seed, &chiffre,
                   &pourcentage, &copie, &inverses, pixels) != 6) continue;
        if (chiffre > 1 || pourcentage != 10) continue;
        if (strlen(pixels) != 48) {
            fclose(fichier);
            fprintf(stderr, "Motif bruité invalide dans noise.csv.\n");
            return 1;
        }

        int x[48];
        for (int j = 0; j < 48; j++) {
            if (pixels[j] != '0' && pixels[j] != '1') {
                fclose(fichier);
                fprintf(stderr, "Pixel invalide dans noise.csv.\n");
                return 1;
            }
            x[j] = pixels[j] - '0';
        }
        bruit_total++;
        bruit_corrects += predict(score(&modele, x)) == chiffre;
    }
    fclose(fichier);

    if (bruit_total == 0) {
        fprintf(stderr, "Aucune copie à 10 %% trouvée dans noise.csv.\n");
        return 1;
    }
    printf("Question 2 : motifs propres reconnus = %d/2 ; copies à 10 %% de bruit reconnues = %d/%d.\n",
           propres_corrects, bruit_corrects, bruit_total);
    printf("Non : réussir sur les deux motifs originaux ne garantit pas la réussite sur leurs versions bruitées, car les pixels modifiés peuvent changer la décision.\n");
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
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

static int question3(void){

    int exemples[2][48];
    int labels[2];
    const char *chemins[] = {"patterns/zero.txt", "patterns/un.txt"};
    Rng rng;
    Perceptron modele;
    rng_seed(&rng, 42);
    perceptron_init(&modele, &rng);

    for (int i = 0; i < 2; i++) {
        char *contenu = lire_fichier_txt(chemins[i]);

        if (contenu == NULL) {
            return 1;
        }

        int statut = texte_vers_motif(contenu, exemples[i], &labels[i]);
        free(contenu);

        if (statut != 0) {
            fprintf(stderr, "Erreur de lecture du motif : %s\n", chemins[i]);
            return 1;
        }
    }

    int ordre[2] = {0, 1};
    int erreurs = 0;
    int epoque;
    int compte_error_null = 0; // Compteur qui verifie combien d'epoque d'affiler ont 0 erreurs , permet au perceptron de finir son apprentissage apres 50 epoque à 0 erreur. 

    for (epoque = 1; epoque <= 1000; epoque++) {

        rng_shuffle(&rng, ordre, 2);
        for (int i = 0; i < 2; i++) {
            int indice = ordre[i];

            update(&modele, exemples[indice], labels[indice], 0.01);
        }

        erreurs = 0;

        for (int i = 0; i < 2; i++) {
            int prediction = predict(score(&modele, exemples[i]));

            if (prediction != labels[i]) {
                erreurs++;
            }
        }

        printf("Epoque %d : %d erreur(s)\n", epoque, erreurs);

        if (erreurs == 0) {
            compte_error_null++;
        }
        else{
            compte_error_null = 0;
        }
        if (compte_error_null >= 50) {
            break;
        }
    }
    if (erreurs == 0) {
        printf("Question 3 : apprentissage reussi en %d epoque(s).\n",
               epoque);
    } else {
        printf("Question 3 : limite de 1000 epoques atteinte, "
               "%d erreur(s) restante(s).\n", erreurs);
    }

    return 0;
}

static int question4(void)
{
    int exemples[2][48];
    int labels[2];

    const char *chemins[] = {
        "patterns/zero.txt",
        "patterns/un.txt"
    };

    Rng rng;
    Perceptron modele;

    /* Initialisation */
    const unsigned int graine = 42;

    rng_seed(&rng, graine);
    perceptron_init(&modele, &rng);

    /* Chargement des deux motifs */
    for (int i = 0; i < 2; i++) {
        char *contenu = lire_fichier_txt(chemins[i]);

        if (contenu == NULL) {
            return 1;
        }

        int statut = texte_vers_motif(
            contenu,
            exemples[i],
            &labels[i]
        );

        free(contenu);

        if (statut != 0) {
            fprintf(stderr, "Erreur de lecture : %s\n", chemins[i]);
            return 1;
        }
    }


    int ordre[2] = {0, 1};
    int erreurs = 0;
    int epoque;

    FILE *f_erreurs = fopen("data/erreurs_epoques.csv", "w");

    if (f_erreurs == NULL) {
        perror("data/erreurs_epoques.csv");
        return 1;
    }

    fprintf(f_erreurs, "epoque,erreurs\n");

    for (epoque = 1; epoque <= 1000; epoque++) {

        /* Mélanger l'ordre des deux motifs */
        rng_shuffle(&rng, ordre, 2);

        /* Présenter chaque motif une fois */
        for (int i = 0; i < 2; i++) {
            int idx = ordre[i];

            update(
                &modele,
                exemples[idx],
                labels[idx],
                0.01
            );
        }

        /* Compter les erreurs sans modifier les poids */
        erreurs = 0;

        for (int i = 0; i < 2; i++) {
            int prediction = predict(score(&modele, exemples[i]));

            if (prediction != labels[i]) {
                erreurs++;
            }
        }

        fprintf(f_erreurs, "%d,%d\n", epoque, erreurs);

        printf("Epoque %d : %d erreur(s)\n", epoque, erreurs);

        /* Arrêt si les deux motifs sont reconnus */
        if (erreurs == 0) {
            break;
        }
    }

    fclose(f_erreurs);

    if (erreurs == 0) {
        printf("Apprentissage reussi en %d epoque(s).\n", epoque);
    } else {
        printf("Limite de 1000 epoques atteinte : "
               "%d erreur(s) restante(s).\n", erreurs);
    }

    FILE *f_poids = fopen("data/poids_appris.csv", "w");

    if (f_poids == NULL) {
        perror("data/poids_appris.csv");
        return 1;
    }

    fprintf(f_poids, "parametre,valeur\n");
    fprintf(f_poids, "graine,%u\n", graine);
    fprintf(f_poids, "biais,%.10f\n", modele.b);

    for (int i = 0; i < 48; i++) {
        fprintf(f_poids, "w%d,%.10f\n", i, modele.w[i]);
    }

    fclose(f_poids);

    /* Les copies bruitées sont lues dans noise.csv (graine 2026) :
     * ce sont les mêmes copies qui serviront à l'étape 2 */
    FILE *f_noise = fopen("noise.csv", "r");

    if (f_noise == NULL) {
        perror("noise.csv");
        return 1;
    }

    /* Une case par niveau de bruit (p = 0, 10, ..., 100) et par chiffre */
    int essais[11][2] = {{0}};
    int erreurs_bruit[11][2] = {{0}};

    char ligne[256];
    char pixels[49];
    int graine_bruit, chiffre, p, copie, inverses;

    /* On saute l'en-tête : noise_seed,digit,p,copy,flipped,pixels */
    if (fgets(ligne, sizeof ligne, f_noise) == NULL) {
        fclose(f_noise);
        return 1;
    }

    while (fgets(ligne, sizeof ligne, f_noise) != NULL) {
        if (sscanf(ligne, "%d,%d,%d,%d,%d,%48s", &graine_bruit, &chiffre,
                   &p, &copie, &inverses, pixels) != 6) {
            continue;
        }

        /* Étape 1 : seulement les chiffres 0 et 1 */
        if (chiffre < 0 || chiffre > 1) {
            continue;
        }

        if (strlen(pixels) != 48 || p < 0 || p > 100 || p % 10 != 0) {
            fclose(f_noise);
            fprintf(stderr, "Ligne invalide dans noise.csv : %s", ligne);
            return 1;
        }

        int x_bruite[48];

        for (int j = 0; j < 48; j++) {
            x_bruite[j] = pixels[j] - '0';
        }

        /* Prédire sans modifier les poids */
        int prediction = predict(score(&modele, x_bruite));
        int k = p / 10;

        essais[k][chiffre]++;

        if (prediction != chiffre) {
            erreurs_bruit[k][chiffre]++;
        }
    }

    fclose(f_noise);

    printf("\n--- Question 4 : robustesse au bruit (noise.csv) ---\n");
    printf("Bruit | Chiffre | Erreurs / essais | Taux d'erreur\n");
    printf("----------------------------------------------------\n");

    FILE *f_bruit = fopen("data/evaluation_bruit.csv", "w");

    if (f_bruit == NULL) {
        perror("data/evaluation_bruit.csv");
        return 1;
    }

    fprintf(
        f_bruit,
        "pourcentage,chiffre,essais,erreurs,taux_erreur\n"
    );

    for (int k = 0; k <= 10; k++) {
        for (int c = 0; c < 2; c++) {

            if (essais[k][c] == 0) {
                fprintf(stderr, "Aucune copie du chiffre %d a %d %% "
                        "dans noise.csv.\n", c, k * 10);
                fclose(f_bruit);
                return 1;
            }

            /* Calcul du taux d'erreur en pourcentage */
            double taux_erreur =
                100.0 * erreurs_bruit[k][c] / essais[k][c];

            printf(
                "%3d %% |    %d    |     %2d / %d      | %5.1f %%\n",
                k * 10,
                c,
                erreurs_bruit[k][c],
                essais[k][c],
                taux_erreur
            );

            fprintf(
                f_bruit,
                "%d,%d,%d,%d,%.2f\n",
                k * 10,
                c,
                essais[k][c],
                erreurs_bruit[k][c],
                taux_erreur
            );
        }
    }

    fclose(f_bruit);

    printf("\nFichiers generes :\n");
    printf("- data/erreurs_epoques.csv\n");
    printf("- data/poids_appris.csv\n");
    printf("- data/evaluation_bruit.csv\n");

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
    if (question3() != 0) return 1;
    if (question4() != 0) return 1;

    return 0;
}
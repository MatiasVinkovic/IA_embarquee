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
    const char *chemins[] = {"patterns/zero.txt", "patterns/un.txt"};
    Rng rng;
    Perceptron modele;
    
    rng_seed(&rng, 42);
    perceptron_init(&modele, &rng);
    
    // on charge les 2 motifs
    for (int i = 0; i < 2; i++) {
        char *contenu = lire_fichier_txt(chemins[i]);
        if (contenu == NULL) return 1;
        int statut = texte_vers_motif(contenu, exemples[i], &labels[i]);
        free(contenu);
        if (statut != 0) return 1;
    }

    //entrainer le perceptron sur ces 2 motifs, 1000 epochs
    int ordre[2] = {0, 1};
    for (int epoque = 1; epoque <= 1000; epoque++) {
        rng_shuffle(&rng, ordre, 2);
        for (int i = 0; i < 2; i++) {
            int idx = ordre[i];
            update(&modele, exemples[idx], labels[idx], 0.01);
        }
        if (predict(score(&modele, exemples[0])) == labels[0] &&
            predict(score(&modele, exemples[1])) == labels[1]) {
            break;
        }
    }

    printf("\n--- Question 4 : evaluation de la robustesse au bruit ---\n");
    printf("Bruit (%%) | Pixels inverses | Reconnus (/100) | Taux succes\n");
    printf("-----------------------------------------------------------\n");

    //on test pour p allant de 0 à 100%
    for (int p = 0; p <= 100; p += 10) {
        // n = arrondi(48 * p / 100) 
        int n_inverses = (int)round((48.0 * (double)p) / 100.0);
        int total_reconnus = 0;

        for (int c = 0; c < 2; c++) {              //pour chaque chiffre (0 et 1)
            for (int copie = 0; copie < 50; copie++) { // 50 copie
                int x_bruite[48];
                memcpy(x_bruite, exemples[c], sizeof(exemples[c]));

                // Préparer les 48 indices pour tirer n pixels distincts 
                int indices[48];
                for (int j = 0; j < 48; j++) indices[j] = j;
                rng_shuffle(&rng, indices, 48);

                //on inverse les N pixels dinstints
                for (int j = 0; j < n_inverses; j++) {
                    int idx = indices[j];
                    x_bruite[idx] = 1 - x_bruite[idx];
                }

                // Test de prédiction uniquement (sans apprentissage) 
                int pred = predict(score(&modele, x_bruite));
                if (pred == labels[c]) {
                    total_reconnus++;
                }
            }
        }

        double taux = (double)total_reconnus / 100.0 * 100.0;
        printf("  %3d %%    |       %2d        |    %3d / 100    |   %5.1f %%\n",
               p, n_inverses, total_reconnus, taux);
    }

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

    if (question2() != 0) return 1;
    if (question3() != 0) return 1;
    if (question4() != 0) return 1;

    return 0;
}
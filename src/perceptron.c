#include "perceptron.h"

/* On calcule le score : a = w0*x0 + w1*x1 + ... + w47*x47 + b */
double score(const Perceptron *p, const int x[N_IN])
{
    double a = p->b;                    /* on part du biais */
    for (int j = 0; j < N_IN; j++) {
        a += p->w[j] * x[j];            /* on ajoute chaque poids x pixel */
    }
    return a;
}

/* Fonction de prédiction : 1 si le score est positif ou nul, sinon 0 */
int predict(double a)
{
    if (a >= 0.0) return 1;
    return 0;
}

/* MISE A JOUR : on corrige les poids seulement si le neurone s'est trompé
 * d = réponse attendue, y = réponse du neurone, e = d - y */
int update(Perceptron *p, const int x[N_IN], int d, double eta)
{
    int y = predict(score(p, x));       /* ce que le neurone répond */
    int e = d - y;                      /* l'erreur : -1, 0 ou 1 */

    for (int j = 0; j < N_IN; j++) {
        p->w[j] = p->w[j] + eta * e * x[j];
    }
    p->b = p->b + eta * e;

    return e;
}
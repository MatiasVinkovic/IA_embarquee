#ifndef PERCEPTRON_H
#define PERCEPTRON_H
/*
Perceptron est une boîte qui contient tout ce que le neurone sait : ses 48 poids et son biais. 
C’est cette boîte qui change pendant l’apprentissage.
*/

#define N_IN 48

/* Le neurone : 48 poids + 1 biais */
typedef struct {
    double w[N_IN];
    double b;
} Perceptron;

double score(const Perceptron *p, const int x[N_IN]);
int    predict(double a);
int    update(Perceptron *p, const int x[N_IN], int d, double eta);

#endif
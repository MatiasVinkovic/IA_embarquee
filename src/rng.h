#ifndef RNG_H
#define RNG_H

#include <stdint.h>   /* pour uint32_t : entier de 32 bits sans signe */

/* L'état du générateur : un seul nombre qui change à chaque tirage */
typedef struct {
    uint32_t state;
} Rng;

int      rng_seed(Rng *r, uint32_t seed);
uint32_t rng_next(Rng *r);
double   rng_uniform(Rng *r, double lo, double hi);
void     rng_shuffle(Rng *r, int *tab, int n);

#endif
#include "rng.h"

/* GRAINE : le point de départ de la suite.
 * 0 est interdit : avec cet algorithme, 0 donnerait 0 pour toujours. */
int rng_seed(Rng *r, uint32_t seed)
{
    if (seed == 0) return -1;
    r->state = seed;
    return 0;
}

/* NOMBRE SUIVANT : algorithme xorshift32.
 * On mélange les bits de l'état (décalages << >> et XOR ^).
 * Résultat : une suite qui a l'air aléatoire, mais toujours la même
 * pour une même graine. C'est le même calcul que next_u32 dans tools.py. */
uint32_t rng_next(Rng *r)
{
    uint32_t s = r->state;
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    r->state = s;      /* on garde le nouvel état pour le prochain tirage */
    return s;
}

/* NOMBRE ENTRE lo ET hi :
 * rng_next donne un entier entre 0 et 4 294 967 295 (le max sur 32 bits).
 * On le divise par ce max pour obtenir u entre 0 et 1,
 * puis on l'étire dans l'intervalle [lo, hi]. */
double rng_uniform(Rng *r, double lo, double hi)
{
    double u = (double)rng_next(r) / 4294967295.0;
    return lo + (hi - lo) * u;
}

/* MELANGE (Fisher-Yates) : on part de la fin du tableau,
 * et on échange chaque case avec une case au hasard avant elle. */
void rng_shuffle(Rng *r, int *tab, int n)
{
    for (int i = n - 1; i > 0; i--) {
        int j = (int)(rng_next(r) % (uint32_t)(i + 1));  /* j entre 0 et i */
        int tmp = tab[i];
        tab[i] = tab[j];
        tab[j] = tmp;
    }
}
# `src/` — code du modèle

Ce dossier contient nos fonctions et programmes, écrits en **C**. Chaque fonction est commentée dans le code et testée dans `tests/`.

## Contenu

| Fichier | Rôle |
|---|---|
| `read_pattern.h` / `read_pattern.c` | Lecture des motifs 8×6 du dossier `patterns/`. |
| `perceptron.h` / `perceptron.c` | Le perceptron à une sortie : score, prédiction, mise à jour des poids. |

## Fonctions

### Lecture des motifs (`read_pattern.c`)

- **`char *lire_fichier_txt(const char *chemin)`**
  Lit un fichier texte en entier et renvoie son contenu dans une chaîne allouée avec `malloc` (terminée par `'\0'`). Renvoie `NULL` si le fichier ne s'ouvre pas ou si l'allocation échoue. **L'appelant doit libérer la chaîne avec `free`.**

- **`int texte_vers_motif(const char *contenu, int x[48], int *label)`**
  Transforme le texte d'un motif en 48 pixels : `.` → 0, `*` → 1. Ce qui suit les 8 lignes est lu comme le label (0 à 9). Renvoie 0 si le motif est complet, -1 sinon.
  Le perceptron ne lit pas du texte, seulement des nombres : cette fonction fait le pont entre les deux.

### Perceptron (`perceptron.c`)

Le neurone est la structure `Perceptron` : 48 poids `w[48]` (un par pixel) et un biais `b`. C'est cette structure qui change pendant l'apprentissage.

- **`double score(const Perceptron *p, const int x[N_IN])`**
  Calcule `a = b + Σ wⱼ·xⱼ` sur les 48 pixels.

- **`int predict(double a)`**
  Fonction seuil : renvoie 1 si `a ≥ 0`, sinon 0.

- **`int update(Perceptron *p, const int x[N_IN], int d, double eta)`**
  Règle du perceptron. Calcule la réponse `y`, l'erreur `e = d − y` (−1, 0 ou 1), puis :
  `wⱼ ← wⱼ + η·e·xⱼ` et `b ← b + η·e`.
  Si le neurone a bien répondu (`e = 0`), rien ne change. Renvoie `e`.

## Tests

`tests/test_step1.c` vérifie :

1. la lecture de `patterns/zero.txt` (label 0, 16 pixels noirs) ;
2. le calcul fait à la main : `x = [1, 0]`, `w = [0.2, −0.1]`, `b = −0.5`, `d = 1`, `η = 0.01`
   → `a = −0.30`, `y = 0`, `e = 1`, puis `w₀ = 0.21` et `b = −0.49`.

Compilation et exécution avec le `Makefile` (depuis la racine du dépôt, car le test ouvre `patterns/zero.txt`) :

```sh
make run     # compile puis lance ./test_step1
make clean   # supprime l'exécutable
```

## Pourquoi le C plutôt que Python ?

Le cours porte sur l'**IA embarquée** : le modèle doit finir sur un Raspberry Pi, puis sur des cibles encore plus petites (microcontrôleurs). Ce choix vient de là.

- **C'est le langage de la cible.** Sur un microcontrôleur, il n'y a ni interpréteur Python, ni NumPy, souvent même pas de système d'exploitation. Le C se compile pour presque n'importe quel processeur. Écrire le modèle en C dès le début évite de devoir tout réécrire au moment du déploiement.

- **Des performances prévisibles.** Le code C est compilé en instructions machine : pas d'interpréteur, pas de ramasse-miettes qui se déclenche au mauvais moment. Les temps d'inférence sont rapides et surtout reproductibles, ce qui compte pour les mesures de temps et d'énergie demandées sur le vrai Pi.

- **Le format des poids s'y prête.** Les poids de secours sont des `float32` little-endian bruts : en C, on les lit avec un simple `fread` dans un tableau de `float`.




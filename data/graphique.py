import csv
import matplotlib.pyplot as plt

# Lecture des erreurs pendant l'apprentissage
epoques = []
erreurs = []

with open("erreurs_epoques.csv", newline="") as fichier:
    for ligne in csv.DictReader(fichier):
        epoques.append(int(ligne["epoque"]))
        erreurs.append(int(ligne["erreurs"]))

# Lecture des résultats selon le bruit
donnees = {}

with open("evaluation_bruit.csv", newline="") as fichier:
    for ligne in csv.DictReader(fichier):
        chiffre = int(ligne["chiffre"])
        donnees.setdefault(chiffre, []).append(ligne)

# Création d'une figure avec deux panneaux
fig, axes = plt.subplots(1, 2, figsize=(13, 5))

# Panneau 1 : erreurs selon l'époque
axes[0].plot(epoques, erreurs, marker="o")
axes[0].set_title("Erreurs pendant l'apprentissage")
axes[0].set_xlabel("Époque")
axes[0].set_ylabel("Nombre d'erreurs sur les deux motifs")
axes[0].set_ylim(-0.1, 2.1)
axes[0].grid(True)

# Panneau 2 : taux d'erreur selon le bruit
for chiffre in sorted(donnees):
    lignes = sorted(
        donnees[chiffre],
        key=lambda ligne: int(ligne["pourcentage"])
    )

    bruit = [int(ligne["pourcentage"]) for ligne in lignes]
    taux = [float(ligne["taux_erreur"]) for ligne in lignes]

    axes[1].plot(
        bruit,
        taux,
        marker="o",
        label=f"Chiffre {chiffre}"
    )

axes[1].set_title("Taux d'erreur selon le bruit")
axes[1].set_xlabel("Bruit (%)")
axes[1].set_ylabel("Taux d'erreur (%)")
axes[1].set_ylim(0, 100)
axes[1].legend()
axes[1].grid(True)

plt.tight_layout()

# Enregistrement de la figure
plt.savefig("resultats_perceptron.png", dpi=300)

print("Graphique enregistré dans resultats_perceptron.png")

plt.show()
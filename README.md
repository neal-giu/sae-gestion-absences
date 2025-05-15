# SAE S1.01 – Gestion des absences (BUT Informatique)

Ce projet a été réalisé dans le cadre de la SAE S1.01 du BUT Informatique (1ère année).  
L’objectif est de développer un **interpréteur de commandes** permettant de gérer les absences d'étudiants dans une formation universitaire.

---

## 🧠 Objectifs du projet

- Gérer l'inscription des étudiants.
- Enregistrer des absences par demi-journée.
- Permettre le dépôt, la validation ou le refus de justificatifs.
- Afficher des bilans d’absences et détecter les étudiants défaillants.

Le programme fonctionne **en ligne de commande**, en lisant des instructions saisies par l’utilisateur.

---

## ⚙️ Fonctionnalités implémentées

| Commande                | Fonction                                        |
|-------------------------|--------------------------------------------------|
| `exit`                  | Quitte le programme                              |
| `inscription`           | Inscrit un étudiant (`nom groupe`)              |
| `absence`              | Enregistre une absence (`id jour am|pm`)        |
| `etudiants`             | Affiche la liste des étudiants (`jour`)         |
| `justificatif`          | Dépose un justificatif (`id_abs jour motif`)    |
| `validations`           | Affiche les absences en attente de validation   |
| `validation`            | Valide ou invalide un justificatif (`id ok|ko`) |
| `etudiant`              | Affiche les absences d’un étudiant (`id jour`)  |
| `defaillants`           | Affiche les étudiants défaillants (`jour`)      |

---

## 🏗️ Organisation du projet

- `main.c` : fichier source principal
- `README.md` : ce fichier
- `.gitignore` : ignore les fichiers binaires et objets
- `LICENSE` : licence MIT

---

## 🛠️ Compilation et exécution

### 📦 Compilation
```bash
gcc main.c -o absences
```

### ▶️ Exécution
```bash
./absences
```

Lancez ensuite vos commandes directement dans le terminal (exemples dans le sujet de la SAE).

---

## 🧪 Exemple

```
inscription Lea 101
Inscription enregistree (1)

absence 1 3 am
Absence enregistree [1]

justificatif 1 5 j'avais mal au ventre
Justificatif enregistre

validations
[1] (1) Lea 101 3/am (j'avais mal au ventre)

validation 1 ok
Validation enregistree

etudiant 1 10
(1) Lea 101 1
- Justifiees
  [1] 3/am (j'avais mal au ventre)
```

---

## 📄 Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](./LICENSE) pour plus d’informations.

---

## ✍️ Auteur

Réalisé par **Neal Giustignano**, étudiant en BUT Informatique.

---

## 🔗 Liens utiles

- Sujet de la SAE : [voir le PDF du sujet](./sujet-s1.pdf)
- Ressources C standard : [cppreference](https://en.cppreference.com/w/c)

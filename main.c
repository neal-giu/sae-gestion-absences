#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =======================================================================
// SAE S1.01 : Gestion des absences BUT Informatique (Session 2024-2025)
// Réalisation : <Neal Giustignano>
// Fichier     : main.c
// -----------------------------------------------------------------------
// Application de gestion des absences étudiants : interpréteur de commandes
// Commandes supportées : voir le cahier des charges (9 commandes)
// =======================================================================

#define LONG_NOM_MAX 30
#define NB_ETU_MAX 100
#define NB_ABS_MAX 8000
#define LONG_MOTIF_MAX 50

// =========================== Structures ================================

/**
 * Structure Etudiant
 * - identifiant : identifiant unique (à partir de 1)
 * - groupe      : numéro de groupe
 * - nom         : nom/prénom étudiant (sans espace, max 30 caractères)
 * - nb_absences : nombre d'absences (incrémenté lors des absences)
 */
typedef struct {
    int identifiant;
    int groupe;
    char nom[LONG_NOM_MAX];
    int nb_absences;
} Etudiant;

/**
 * Structure Absence
 * - id_etudiant : identifiant de l'étudiant concerné
 * - jour        : numéro de jour (1 à 40)
 * - demi_jour   : "am" ou "pm"
 * - motif       : texte du justificatif (max 50 caractères)
 * - etat_motif  : 0=non-justifiée; 1=justif. en attente validation;
 *                 2=justifiée; 3=justif. refusée ou hors délai
 */
typedef struct {
    int id_etudiant;
    int jour;
    char demi_jour[3];
    char motif[LONG_MOTIF_MAX];
    int etat_motif;
} Absence;

// ====================== Fonctions utilitaires ==========================

/**
 * Lecture d'une ligne avec suppression du saut de ligne.
 * @param ligne   buffer d'écriture
 * @param max     taille max du buffer
 */
void saisieLigne(char* ligne, int max) {
    int c = getc(stdin);
    while (c == ' ' || c == '\t')
        c = getc(stdin);
    ungetc(c, stdin);
    fgets(ligne, max, stdin);
    ligne[strcspn(ligne, "\n")] = '\0'; // supprime '\n'
}

/**
 * Comparaison pour tri des étudiants (par groupe, puis nom)
 * @param etu1, etu2 : pointeurs vers Etudiant
 * @return <0 si etu1<etu2, >0 si etu1>etu2, 0 sinon
 */
int cmp_etu(const void* etu1, const void* etu2) {
    const Etudiant* e1 = (const Etudiant*)etu1;
    const Etudiant* e2 = (const Etudiant*)etu2;
    if (e1->groupe != e2->groupe)
        return e1->groupe - e2->groupe;
    return strcmp(e1->nom, e2->nom);
}

// ==================== Fonctions Commandes ==============================

/**
 * Commande C0 : exit
 * Sortie immédiate du programme.
 */
void commande_exit(const char* requete) {
    if (strcmp(requete, "exit") == 0)
        exit(0);
}

/**
 * Commande C1 : inscription <nom> <groupe>
 * Ajoute un nouvel étudiant si le nom n'est pas déjà pris dans le groupe.
 * Affiche le message d'enregistrement ou d'erreur.
 * @return 1 si inscription réussie, 0 sinon
 */
int commande_inscription(Etudiant tab[], int nb_etu) {
    char nom[LONG_NOM_MAX];
    int groupe;
    scanf("%s", nom);
    scanf("%d", &groupe);

    if (strlen(nom) >= LONG_NOM_MAX) {
        printf("Nom trop long\n");
        return 0;
    }

    // Vérification unicité nom dans le groupe
    for (int i = 0; i < nb_etu; ++i) {
        if (tab[i].groupe == groupe && strcmp(tab[i].nom, nom) == 0) {
            printf("Nom incorrect\n");
            return 0;
        }
    }

    // Ajout étudiant
    tab[nb_etu].identifiant = nb_etu + 1;
    tab[nb_etu].groupe = groupe;
    strcpy(tab[nb_etu].nom, nom);
    tab[nb_etu].nb_absences = 0;
    printf("Inscription enregistree (%d)\n", tab[nb_etu].identifiant);
    return 1;
}

/**
 * Commande C2 : absence <id> <jour> <am|pm>
 * Ajoute une absence à un étudiant donné, si non déjà déclarée pour ce jour.
 */
void commande_absence(Etudiant tab[], Absence tab2[], int* nb_abs, int nb_etu) {
    int id, jour;
    char demi_jour[3];
    scanf("%d", &id);

    if (id < 1 || id > nb_etu) {
        printf("Identifiant incorrect\n");
        return;
    }
    scanf("%d", &jour);
    if (jour < 1 || jour > 40) {
        printf("Date incorrecte\n");
        return;
    }
    scanf("%s", demi_jour);
    if (strcmp(demi_jour, "am") != 0 && strcmp(demi_jour, "pm") != 0) {
        printf("Demi-journee incorrecte\n");
        return;
    }
    // Vérifie si absence déjà déclarée
    for (int i = 0; i < *nb_abs; ++i) {
        if (tab2[i].id_etudiant == id && tab2[i].jour == jour &&
            strcmp(tab2[i].demi_jour, demi_jour) == 0) {
            printf("Absence deja connue\n");
            return;
        }
    }
    // Ajout
    tab2[*nb_abs].id_etudiant = id;
    tab2[*nb_abs].jour = jour;
    strcpy(tab2[*nb_abs].demi_jour, demi_jour);
    tab2[*nb_abs].motif[0] = '\0';
    tab2[*nb_abs].etat_motif = 0;
    tab[id - 1].nb_absences++;
    (*nb_abs)++;
    printf("Absence enregistree [%d]\n", *nb_abs);
}

/**
 * Commande C3 : etudiants <jour>
 * Affiche la liste des étudiants inscrits, triés par groupe puis nom.
 * Affiche aussi le nombre d'absences de chacun jusqu'au jour donné.
 */
void commande_etudiants(const Etudiant etu[], int nb_etu, const Absence abs[], int nb_abs) {
    int jour;
    scanf("%d", &jour);
    if (nb_etu == 0) {
        printf("Aucun inscrit\n");
        return;
    }
    if (jour < 1) {
        printf("Date incorrecte\n");
        return;
    }
    // Copie pour tri
    Etudiant etu_copy[NB_ETU_MAX];
    memcpy(etu_copy, etu, nb_etu * sizeof(Etudiant));
    qsort(etu_copy, nb_etu, sizeof(Etudiant), cmp_etu);

    for (int i = 0; i < nb_etu; ++i) {
        int abs_count = 0;
        for (int j = 0; j < nb_abs; ++j)
            if (abs[j].id_etudiant == etu_copy[i].identifiant && abs[j].jour <= jour)
                abs_count++;
        printf("(%d) %s %d %d\n", etu_copy[i].identifiant, etu_copy[i].nom, etu_copy[i].groupe, abs_count);
    }
}

/**
 * Commande C4 : justificatif <id_abs> <jour> <motif>
 * Enregistre un justificatif pour une absence (si délai OK, pas déjà justifiée, etc).
 */
void commande_justificatif(Absence abs[], int nb_abs) {
    int id, jour;
    scanf("%d", &id);
    if (id < 1 || id > nb_abs) {
        printf("Identifiant incorrect\n");
        // Consomme la suite
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    scanf("%d", &jour);
    if (jour < abs[id - 1].jour) {
        printf("Date incorrecte\n");
        // Consomme la suite
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    if (abs[id - 1].etat_motif == 1 || abs[id - 1].etat_motif == 2 || abs[id - 1].etat_motif == 3) {
        printf("Justificatif deja connu\n");
        // Consomme la suite
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    // Lecture motif
    char motif[LONG_MOTIF_MAX];
    saisieLigne(motif, LONG_MOTIF_MAX);

    // Justificatif dans le délai de 3 jours ?
    if (abs[id - 1].jour + 3 >= jour) {
        abs[id - 1].etat_motif = 1; // en attente validation
    } else {
        abs[id - 1].etat_motif = 3; // hors délai, non valable
    }
    strncpy(abs[id - 1].motif, motif, LONG_MOTIF_MAX - 1);
    abs[id - 1].motif[LONG_MOTIF_MAX - 1] = '\0';

    printf("Justificatif enregistre\n");
}

/**
 * Commande C5 : validations
 * Affiche la liste des absences avec justificatif en attente de validation.
 */
void commande_validations(const Absence abs[], int nb_abs, const Etudiant etu[], int nb_etu) {
    int found = 0;
    for (int i = 0; i < nb_abs; ++i) {
        if (abs[i].etat_motif == 1) {
            printf("[%d] (%d) %s %d %d/%s (%s)\n",
                   i + 1,
                   abs[i].id_etudiant,
                   etu[abs[i].id_etudiant - 1].nom,
                   etu[abs[i].id_etudiant - 1].groupe,
                   abs[i].jour,
                   abs[i].demi_jour,
                   abs[i].motif);
            found = 1;
        }
    }
    if (!found)
        printf("Aucune validation en attente\n");
}

/**
 * Commande C6 : validation <id_abs> <ok|ko>
 * Valide ou invalide un justificatif.
 */
void commande_validation(Absence abs[], int nb_abs) {
    int id;
    char code[4];
    scanf("%d", &id);
    if (id < 1 || id > nb_abs || abs[id - 1].etat_motif != 1) {
        printf("Identifiant incorrect\n");
        // Consomme la suite
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    scanf("%s", code);
    if (abs[id - 1].etat_motif == 2 || abs[id - 1].etat_motif == 3) {
        printf("Validation deja connue\n");
        return;
    }
    if (strcmp(code, "ok") == 0) {
        abs[id - 1].etat_motif = 2;
        printf("Validation enregistree\n");
    } else if (strcmp(code, "ko") == 0) {
        abs[id - 1].etat_motif = 3;
        printf("Validation enregistree\n");
    } else {
        printf("Code incorrect\n");
    }
}

/**
 * Commande C7 : etudiant <id> <jour>
 * Affiche la situation détaillée d’un étudiant.
 */
void commande_etudiant(const Etudiant etu[], int nb_etu, const Absence abs[], int nb_abs) {
    int id, jour;
    scanf("%d %d", &id, &jour);

    if (id < 1 || id > nb_etu) {
        printf("Identifiant incorrect\n");
        return;
    }
    if (jour < 1) {
        printf("Date incorrecte\n");
        return;
    }

    // Affichage entête étudiant
    printf("(%d) %s %d", etu[id - 1].identifiant, etu[id - 1].nom, etu[id - 1].groupe);

    // Compte le nombre d'absences à ce jour
    int total_abs = 0;
    for (int j = 0; j < nb_abs; ++j) {
        if (abs[j].id_etudiant == id && abs[j].jour <= jour)
            total_abs++;
    }
    printf(" %d\n", total_abs);

    // Préparation des listes d’absences
    int idx[NB_ABS_MAX], na1 = 0, na2 = 0, na3 = 0, na4 = 0;
    int a1[NB_ABS_MAX], a2[NB_ABS_MAX], a3[NB_ABS_MAX], a4[NB_ABS_MAX];

    for (int j = 0; j < nb_abs; ++j) {
        if (abs[j].id_etudiant == id && abs[j].jour <= jour) {
            if (abs[j].etat_motif == 0 && abs[j].jour + 3 >= jour) { // délai encore possible
                a1[na1++] = j;
            } else if (abs[j].etat_motif == 1) {
                a2[na2++] = j;
            } else if (abs[j].etat_motif == 2) {
                a3[na3++] = j;
            } else if ((abs[j].etat_motif == 0 && abs[j].jour + 3 < jour) || abs[j].etat_motif == 3) {
                a4[na4++] = j;
            }
        }
    }
    // Affichage par catégorie
    if (na1 > 0) {
        printf("- En attente justificatif\n");
        for (int i = 0; i < na1; ++i) {
            printf("  [%d] %d/%s\n", a1[i] + 1, abs[a1[i]].jour, abs[a1[i]].demi_jour);
        }
    }
    if (na2 > 0) {
        printf("- En attente validation\n");
        for (int i = 0; i < na2; ++i) {
            printf("  [%d] %d/%s (%s)\n", a2[i] + 1, abs[a2[i]].jour, abs[a2[i]].demi_jour, abs[a2[i]].motif);
        }
    }
    if (na3 > 0) {
        printf("- Justifiees\n");
        for (int i = 0; i < na3; ++i) {
            printf("  [%d] %d/%s (%s)\n", a3[i] + 1, abs[a3[i]].jour, abs[a3[i]].demi_jour, abs[a3[i]].motif);
        }
    }
    if (na4 > 0) {
        printf("- Non-justifiees\n");
        for (int i = 0; i < na4; ++i) {
            if (abs[a4[i]].motif[0])
                printf("  [%d] %d/%s (%s)\n", a4[i] + 1, abs[a4[i]].jour, abs[a4[i]].demi_jour, abs[a4[i]].motif);
            else
                printf("  [%d] %d/%s\n", a4[i] + 1, abs[a4[i]].jour, abs[a4[i]].demi_jour);
        }
    }
}

/**
 * Commande C8 : defaillants <jour>
 * Affiche les étudiants avec 5 absences non-justifiées ou plus à la date donnée.
 */
void commande_defaillants(const Etudiant etu[], int nb_etu, const Absence abs[], int nb_abs) {
    int jour;
    scanf("%d", &jour);
    if (jour < 1) {
        printf("Date incorrecte\n");
        return;
    }
    int found = 0;
    for (int i = 0; i < nb_etu; ++i) {
        int nonjust = 0, total = 0;
        for (int j = 0; j < nb_abs; ++j) {
            if (abs[j].id_etudiant == etu[i].identifiant && abs[j].jour <= jour) {
                total++;
                if ((abs[j].etat_motif == 0 && abs[j].jour + 3 < jour) || abs[j].etat_motif == 3)
                    nonjust++;
            }
        }
        if (nonjust >= 5) {
            printf("(%d) %s %d %d\n", etu[i].identifiant, etu[i].nom, etu[i].groupe, total);
            found = 1;
        }
    }
    if (!found)
        printf("Aucun defaillant\n");
}

// ============================ MAIN =====================================

/**
 * Programme principal : boucle sur l'interprétation des commandes.
 */
int main() {
    char requete[64];
    Etudiant tab[NB_ETU_MAX];
    Absence tab2[NB_ABS_MAX];
    int nb_etu = 0, nb_abs = 0;

    while (1) {
        scanf("%s", requete);
        commande_exit(requete);

        if (strcmp(requete, "inscription") == 0) {
            if (nb_etu >= NB_ETU_MAX) {
                printf("Limite d'inscription atteinte\n");
            } else {
                if (commande_inscription(tab, nb_etu)) nb_etu++;
            }
        }
        else if (strcmp(requete, "absence") == 0) {
            commande_absence(tab, tab2, &nb_abs, nb_etu);
        }
        else if (strcmp(requete, "etudiants") == 0) {
            commande_etudiants(tab, nb_etu, tab2, nb_abs);
        }
        else if (strcmp(requete, "justificatif") == 0) {
            commande_justificatif(tab2, nb_abs);
        }
        else if (strcmp(requete, "validations") == 0) {
            commande_validations(tab2, nb_abs, tab, nb_etu);
        }
        else if (strcmp(requete, "validation") == 0) {
            commande_validation(tab2, nb_abs);
        }
        else if (strcmp(requete, "etudiant") == 0) {
            commande_etudiant(tab, nb_etu, tab2, nb_abs);
        }
        else if (strcmp(requete, "defaillants") == 0) {
            commande_defaillants(tab, nb_etu, tab2, nb_abs);
        }
    }
    return 0;
}

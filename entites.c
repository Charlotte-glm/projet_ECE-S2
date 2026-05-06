/* ============================================================
   entites.c
   Implementation du module Entites.
   Gestion dynamique des listes chainees de bulles, projectiles,
   eclairs et bonus avec malloc/free.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entites.h"

/* ---- Calcul du rayon en fonction de la taille ---- */
int rayon_pour_taille(int taille) {
    switch (taille) {
        case 4: return 35;
        case 3: return 25;
        case 2: return 17;
        case 1: return 10;
        default: return 10;
    }
}

/* ---- Initialisation du joueur ---- */
void init_joueur(Joueur *j, const char *pseudo) {
    j->x = LARGEUR_FENETRE / 2.0f;
    j->y = ZONE_JEU_BOTTOM - 30;
    j->largeur = 30;
    j->hauteur = 40;
    j->vitesse = 4.0f;
    j->vivant = 1;
    j->arme = ARME_BASE;
    j->duree_arme = 0;
    j->timer_tir = 0;
    j->score = 0;
    j->bouclier = 0;
    j->duree_bouclier = 0;
    j->ralenti = 0;
    j->duree_ralenti = 0;
    if (pseudo != NULL) {
        strncpy(j->pseudo, pseudo, TAILLE_PSEUDO - 1);
        j->pseudo[TAILLE_PSEUDO - 1] = '\0';
    } else {
        strcpy(j->pseudo, "JOUEUR");
    }
}

/* ---- Initialisation d'une bulle ---- */
void init_bulle(Bulle *b, float x, float y, int taille, float vx, float vy) {
    b->x = x;
    b->y = y;
    b->vx = vx;
    b->vy = vy;
    b->taille = taille;
    b->rayon = rayon_pour_taille(taille);
    b->vivante = 1;
    b->peut_eclair = 0;
    b->timer_eclair = 0;
    b->contient_bonus = BONUS_AUCUN;
    b->suivante = NULL;
}

/* ---- Initialisation d'un projectile ---- */
void init_projectile(Projectile *p, float x, float y, int type) {
    p->x = x;
    p->y = y;
    p->vitesse = 8.0f;
    p->actif = 1;
    p->type = type;
    p->suivant = NULL;
}

/* ---- Ajout d'une bulle en tete de liste ---- */
void ajt_bulle(ListeBulles *lb, Bulle b) {
    Bulle *nouvelle = (Bulle *)malloc(sizeof(Bulle));
    if (nouvelle == NULL) return;
    *nouvelle = b;
    nouvelle->suivante = lb->tete;
    lb->tete = nouvelle;
    lb->nb++;
}

/* ---- Suppression d'une bulle precise ---- */
void suppr_bulle(ListeBulles *lb, Bulle *b) {
    Bulle *prec = NULL;
    Bulle *cour = lb->tete;
    while (cour != NULL) {
        if (cour == b) {
            if (prec == NULL) lb->tete = cour->suivante;
            else prec->suivante = cour->suivante;
            free(cour);
            lb->nb--;
            return;
        }
        prec = cour;
        cour = cour->suivante;
    }
}

void vider_liste_bulles(ListeBulles *lb) {
    Bulle *cour = lb->tete;
    Bulle *suiv;
    while (cour != NULL) {
        suiv = cour->suivante;
        free(cour);
        cour = suiv;
    }
    lb->tete = NULL;
    lb->nb = 0;
}

/* ---- Liste projectiles ---- */
void ajt_projectile(ListeProjectiles *lp, Projectile p) {
    Projectile *nouveau = (Projectile *)malloc(sizeof(Projectile));
    if (nouveau == NULL) return;
    *nouveau = p;
    nouveau->suivant = lp->tete;
    lp->tete = nouveau;
    lp->nb++;
}

void suppr_projectile(ListeProjectiles *lp, Projectile *p) {
    Projectile *prec = NULL;
    Projectile *cour = lp->tete;
    while (cour != NULL) {
        if (cour == p) {
            if (prec == NULL) lp->tete = cour->suivant;
            else prec->suivant = cour->suivant;
            free(cour);
            lp->nb--;
            return;
        }
        prec = cour;
        cour = cour->suivant;
    }
}

void vider_liste_projectiles(ListeProjectiles *lp) {
    Projectile *cour = lp->tete;
    Projectile *suiv;
    while (cour != NULL) {
        suiv = cour->suivant;
        free(cour);
        cour = suiv;
    }
    lp->tete = NULL;
    lp->nb = 0;
}

/* ---- Liste eclairs ---- */
void ajt_eclair(ListeEclairs *le, float x, float y) {
    Eclair *nouveau = (Eclair *)malloc(sizeof(Eclair));
    if (nouveau == NULL) return;
    nouveau->x = x;
    nouveau->y = y;
    nouveau->vitesse = 5.0f;
    nouveau->actif = 1;
    nouveau->suivant = le->tete;
    le->tete = nouveau;
    le->nb++;
}

void vider_liste_eclairs(ListeEclairs *le) {
    Eclair *cour = le->tete;
    Eclair *suiv;
    while (cour != NULL) {
        suiv = cour->suivant;
        free(cour);
        cour = suiv;
    }
    le->tete = NULL;
    le->nb = 0;
}

/* ---- Liste bonus ---- */
void ajt_bonus(ListeBonus *lbon, float x, float y, int type) {
    BonusObjet *nouveau = (BonusObjet *)malloc(sizeof(BonusObjet));
    if (nouveau == NULL) return;
    nouveau->x = x;
    nouveau->y = y;
    nouveau->vy = 0.0f;
    nouveau->type = type;
    nouveau->actif = 1;
    nouveau->au_sol = 0;
    nouveau->suivant = lbon->tete;
    lbon->tete = nouveau;
    lbon->nb++;
}

void vider_liste_bonus(ListeBonus *lbon) {
    BonusObjet *cour = lbon->tete;
    BonusObjet *suiv;
    while (cour != NULL) {
        suiv = cour->suivant;
        free(cour);
        cour = suiv;
    }
    lbon->tete = NULL;
    lbon->nb = 0;
}

/* ---- Initialisation des entites d'un niveau ---- */
void init_niveau_entites(Niveau *n, int numero) {
    Bulle b;
    int i, nb_bulles;
    float bx;
    int taille;

    /* Vider toutes les listes au cas ou */
    vider_liste_bulles(&n->liste_bulles);
    vider_liste_projectiles(&n->liste_projectiles);
    vider_liste_eclairs(&n->liste_eclairs);
    vider_liste_bonus(&n->liste_bonus);

    /* Determiner le contenu du niveau */
    switch (numero) {
        case 1:
            nb_bulles = 2;
            taille = 4;
            n->temps_limite = 60;
            n->eclairs_actifs = 0;
            n->has_boss = 0;
            break;
        case 2:
            nb_bulles = 3;
            taille = 4;
            n->temps_limite = 70;
            n->eclairs_actifs = 0;
            n->has_boss = 0;
            break;
        case 3:
            nb_bulles = 4;
            taille = 3;
            n->temps_limite = 75;
            n->eclairs_actifs = 1;
            n->has_boss = 0;
            break;
        case 4:
            nb_bulles = 0;
            taille = 0;
            n->temps_limite = 90;
            n->eclairs_actifs = 1;
            n->has_boss = 1;
            break;
        default:
            nb_bulles = 2;
            taille = 4;
            n->temps_limite = 60;
            n->eclairs_actifs = 0;
            n->has_boss = 0;
            break;
    }

    n->numero = numero;
    n->temps_restant = n->temps_limite * 60; /* en frames si 60 fps */

    /* Creation des bulles initiales reparties horizontalement */
    for (i = 0; i < nb_bulles; i++) {
        bx = 100 + i * (LARGEUR_FENETRE - 200) / (nb_bulles > 1 ? nb_bulles - 1 : 1);
        init_bulle(&b, bx, ZONE_JEU_TOP + 80, taille,
                   (i % 2 == 0) ? 2.0f : -2.0f, 0.0f);
        if (numero >= 3 && (i % 2 == 0)) b.peut_eclair = 1;
        if (numero >= 2 && (i == 0)) {
            /* la premiere bulle contient un bonus */
            b.contient_bonus = (numero == 2) ? BONUS_ARME_RAPIDE : BONUS_BOUCLIER;
        }
        ajt_bulle(&n->liste_bulles, b);
    }

    /* Initialisation du boss si present */
    if (n->has_boss) {
        n->boss.x = LARGEUR_FENETRE / 2.0f;
        n->boss.y = ZONE_JEU_TOP + 80;
        n->boss.largeur = 80;
        n->boss.hauteur = 60;
        n->boss.vx = 2.0f;
        n->boss.vy = 0.0f;
        n->boss.pv = 8;
        n->boss.pv_max = 8;
        n->boss.vivant = 1;
        n->boss.timer_tir = 120;
    }
}

/* ---- Reinitialisation totale d'un niveau ---- */
void reinit_NivEntites(Niveau *n) {
    init_niveau_entites(n, n->numero);
}

/* ============================================================
   collisions.c
   Implementation du module Collisions.
   Detection par boites englobantes (hitboxes rectangulaires)
   et traitement des consequences (division, mort, ramassage).
   ============================================================ */

#include <stdlib.h>
#include "collisions.h"
#include "entites.h"

/* ---- Helper : test de chevauchement de deux rectangles ---- */
static int rect_overlap(float ax, float ay, float aw, float ah,
                        float bx, float by, float bw, float bh) {
    return !(ax + aw < bx || bx + bw < ax ||
             ay + ah < by || by + bh < ay);
}

/* ---- Bulle / mur (rebond gere dans deplacements) ----
   Retourne 1 si la bulle touche un bord, 0 sinon. */
int collis_bullemur(Bulle *b) {
    if (b->x - b->rayon <= 0)            return 1;
    if (b->x + b->rayon >= LARGEUR_FENETRE)     return 1;
    if (b->y - b->rayon <= ZONE_JEU_TOP) return 1;
    if (b->y + b->rayon >= ZONE_JEU_BOTTOM) return 1;
    return 0;
}

/* ---- Projectile / bulle ---- */
int collis_projecbulle(Projectile *p, Bulle *b) {
    /* hitbox du projectile : 4x10, hitbox de la bulle : carre */
    return rect_overlap(p->x - 2, p->y - 5, 4, 10,
                        b->x - b->rayon, b->y - b->rayon,
                        2 * b->rayon, 2 * b->rayon);
}

/* ---- Bulle / joueur ---- */
int collis_bullejoueur(Bulle *b, Joueur *j) {
    return rect_overlap(b->x - b->rayon, b->y - b->rayon,
                        2 * b->rayon, 2 * b->rayon,
                        j->x - j->largeur / 2, j->y - j->hauteur / 2,
                        j->largeur, j->hauteur);
}

/* ---- Eclair / joueur ---- */
int collis_eclairjoueur(Eclair *e, Joueur *j) {
    return rect_overlap(e->x - 3, e->y, 6, 20,
                        j->x - j->largeur / 2, j->y - j->hauteur / 2,
                        j->largeur, j->hauteur);
}

/* ---- Bonus / joueur ---- */
int collis_bonusjoueur(BonusObjet *bo, Joueur *j) {
    return rect_overlap(bo->x - 12, bo->y - 12, 24, 24,
                        j->x - j->largeur / 2, j->y - j->hauteur / 2,
                        j->largeur, j->hauteur);
}

/* ---- Projectile / boss ---- */
int collis_projecboss(Projectile *p, Boss *boss) {
    if (!boss->vivant) return 0;
    return rect_overlap(p->x - 2, p->y - 5, 4, 10,
                        boss->x - boss->largeur / 2,
                        boss->y - boss->hauteur / 2,
                        boss->largeur, boss->hauteur);
}

/* ---- Boss / joueur ---- */
int collis_bossjoueur(Boss *boss, Joueur *j) {
    if (!boss->vivant) return 0;
    return rect_overlap(boss->x - boss->largeur / 2,
                        boss->y - boss->hauteur / 2,
                        boss->largeur, boss->hauteur,
                        j->x - j->largeur / 2, j->y - j->hauteur / 2,
                        j->largeur, j->hauteur);
}

/* ---- Traitement projectile / bulle :
   division ou destruction de la bulle, ajout de score, drop bonus ---- */
void traitercollis_projbulle(Niveau *n, Joueur *j, Projectile *p, Bulle *b) {
    Bulle nouvelle;
    int taille_inf = b->taille - 1;
    int rayon = rayon_pour_taille(taille_inf);

    p->actif = 0; /* le projectile disparait */
    j->score += 50 * b->taille;

    /* Si la bulle contient un bonus, on le libere au sol */
    if (b->contient_bonus != BONUS_AUCUN) {
        ajt_bonus(&n->liste_bonus, b->x, b->y, b->contient_bonus);
    }

    if (b->taille <= 1) {
        /* Bulle minuscule : detruite definitivement */
        b->vivante = 0;
        return;
    }

    /* Sinon division en 2 bulles plus petites avec trajectoires divergentes */
    init_bulle(&nouvelle, b->x, b->y, taille_inf, -2.5f, -5.0f);
    if (n->numero >= 3 && (rand() % 3 == 0)) nouvelle.peut_eclair = 1;
    ajt_bulle(&n->liste_bulles, nouvelle);

    init_bulle(&nouvelle, b->x, b->y, taille_inf, 2.5f, -5.0f);
    if (n->numero >= 3 && (rand() % 3 == 0)) nouvelle.peut_eclair = 1;
    ajt_bulle(&n->liste_bulles, nouvelle);

    b->vivante = 0;
    (void)rayon;
}

/* ---- Traitement bulle / joueur ---- */
void traitercollis_bullejoueur(Joueur *j) {
    if (j->bouclier) {
        /* Le bouclier absorbe l'impact */
        j->bouclier = 0;
        j->duree_bouclier = 0;
    } else {
        j->vivant = 0;
    }
}

/* ---- Traitement bonus / joueur ---- */
void traitercollis_bonusjoueur(BonusObjet *bo, Joueur *j) {
    switch (bo->type) {
        case BONUS_ARME_RAPIDE:
            j->arme = ARME_RAPIDE;
            j->duree_arme = 600; /* 10 secondes a 60 fps */
            break;
        case BONUS_ARME_DOUBLE:
            j->arme = ARME_DOUBLE;
            j->duree_arme = 600;
            break;
        case BONUS_BOUCLIER:
            j->bouclier = 1;
            j->duree_bouclier = 600;
            break;
        case MALUS_LENT:
            j->ralenti = 1;
            j->duree_ralenti = 300;
            break;
        default:
            break;
    }
    bo->actif = 0;
}

/* ---- Gestion globale des collisions du frame ---- */
void gerer_collisions(Jeu *jeu) {
    Bulle *b, *b_next;
    Projectile *p, *p_next;
    Eclair *e;
    BonusObjet *bo;
    Niveau *n = jeu->niveau;
    Joueur *j = jeu->joueur;

    /* 1. Projectiles vs bulles */
    p = n->liste_projectiles.tete;
    while (p != NULL) {
        p_next = p->suivant;
        if (p->actif) {
            b = n->liste_bulles.tete;
            while (b != NULL) {
                b_next = b->suivante;
                if (b->vivante && collis_projecbulle(p, b)) {
                    traitercollis_projbulle(n, j, p, b);
                    break; /* un projectile ne touche qu'une bulle */
                }
                b = b_next;
            }
        }
        p = p_next;
    }

    /* 2. Projectiles vs boss */
    if (n->has_boss && n->boss.vivant) {
        p = n->liste_projectiles.tete;
        while (p != NULL) {
            if (p->actif && collis_projecboss(p, &n->boss)) {
                p->actif = 0;
                n->boss.pv--;
                j->score += 100;
                if (n->boss.pv <= 0) {
                    n->boss.vivant = 0;
                    j->score += 1000;
                }
            }
            p = p->suivant;
        }
    }

    /* 3. Bulles vs joueur */
    b = n->liste_bulles.tete;
    while (b != NULL && j->vivant) {
        if (b->vivante && collis_bullejoueur(b, j)) {
            traitercollis_bullejoueur(j);
        }
        b = b->suivante;
    }

    /* 4. Eclairs vs joueur */
    e = n->liste_eclairs.tete;
    while (e != NULL && j->vivant) {
        if (e->actif && collis_eclairjoueur(e, j)) {
            if (j->bouclier) {
                j->bouclier = 0;
                j->duree_bouclier = 0;
                e->actif = 0;
            } else {
                j->vivant = 0;
            }
        }
        e = e->suivant;
    }

    /* 5. Boss vs joueur */
    if (n->has_boss && n->boss.vivant && j->vivant) {
        if (collis_bossjoueur(&n->boss, j)) {
            if (j->bouclier) {
                j->bouclier = 0;
                j->duree_bouclier = 0;
            } else {
                j->vivant = 0;
            }
        }
    }

    /* 6. Bonus vs joueur */
    bo = n->liste_bonus.tete;
    while (bo != NULL) {
        if (bo->actif && collis_bonusjoueur(bo, j)) {
            traitercollis_bonusjoueur(bo, j);
        }
        bo = bo->suivant;
    }

    /* 7. Nettoyage des entites mortes */
    /* Bulles */
    b = n->liste_bulles.tete;
    while (b != NULL) {
        b_next = b->suivante;
        if (!b->vivante) suppr_bulle(&n->liste_bulles, b);
        b = b_next;
    }
    /* Projectiles */
    p = n->liste_projectiles.tete;
    while (p != NULL) {
        p_next = p->suivant;
        if (!p->actif) suppr_projectile(&n->liste_projectiles, p);
        p = p_next;
    }
}

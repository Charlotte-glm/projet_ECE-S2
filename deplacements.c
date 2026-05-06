/* ============================================================
   deplacements.c
   Implementation du module Deplacements.
   Mise a jour des positions et physique des entites.
   ============================================================ */

#include <stdlib.h>
#include "deplacements.h"
#include "entites.h"

#define GRAVITE         0.18f
#define VITESSE_BULLE_X 2.0f

/* ---- Deplacement du joueur ---- */
void joueur_mvt(Joueur *j, Commandes cmd) {
    float v = j->vitesse;
    if (j->ralenti) v *= 0.5f;

    if (cmd.gauche) j->x -= v;
    if (cmd.droite) j->x += v;

    /* Blocage sur les bords de la zone de jeu */
    if (j->x - j->largeur / 2 < 0)
        j->x = j->largeur / 2;
    if (j->x + j->largeur / 2 > LARGEUR_FENETRE)
        j->x = LARGEUR_FENETRE - j->largeur / 2;

    /* Mise a jour des timers d'armes/bonus */
    if (j->duree_arme > 0) {
        j->duree_arme--;
        if (j->duree_arme == 0) j->arme = ARME_BASE;
    }
    if (j->duree_bouclier > 0) {
        j->duree_bouclier--;
        if (j->duree_bouclier == 0) j->bouclier = 0;
    }
    if (j->duree_ralenti > 0) {
        j->duree_ralenti--;
        if (j->duree_ralenti == 0) j->ralenti = 0;
    }
    if (j->timer_tir > 0) j->timer_tir--;
}

/* ---- Rebond d'une bulle sur les bords ---- */
void rebond_bulle(Bulle *b) {
    /* Murs lateraux */
    if (b->x - b->rayon < 0) {
        b->x = b->rayon;
        b->vx = -b->vx;
    }
    if (b->x + b->rayon > LARGEUR_FENETRE) {
        b->x = LARGEUR_FENETRE - b->rayon;
        b->vx = -b->vx;
    }
    /* Plafond */
    if (b->y - b->rayon < ZONE_JEU_TOP) {
        b->y = ZONE_JEU_TOP + b->rayon;
        b->vy = -b->vy;
    }
    /* Sol : rebond avec impulsion verticale, simulant un effet de bulle */
    if (b->y + b->rayon > ZONE_JEU_BOTTOM) {
        b->y = ZONE_JEU_BOTTOM - b->rayon;
        /* Hauteur de rebond proportionnelle a la taille (plus grande = plus haut) */
        b->vy = -7.0f - b->taille * 0.6f;
    }
}

/* ---- Deplacement de toutes les bulles ---- */
void bulles_mvt(ListeBulles *lb, int ralenti) {
    Bulle *b;
    float facteur = ralenti ? 0.4f : 1.0f;

    b = lb->tete;
    while (b != NULL) {
        if (b->vivante) {
            /* Gravite sur la vitesse verticale */
            b->vy += GRAVITE * facteur;
            /* Mise a jour de la position */
            b->x += b->vx * facteur;
            b->y += b->vy * facteur;
            /* Gestion des rebonds */
            rebond_bulle(b);
            /* Timer d'eclairs */
            if (b->peut_eclair) {
                if (b->timer_eclair > 0) b->timer_eclair--;
            }
        }
        b = b->suivante;
    }
}

/* ---- Deplacement des projectiles ---- */
void deplacer_projectiles(ListeProjectiles *lp) {
    Projectile *p = lp->tete;
    while (p != NULL) {
        if (p->actif) {
            p->y -= p->vitesse;
            if (p->y < ZONE_JEU_TOP) p->actif = 0;
        }
        p = p->suivant;
    }
}

/* ---- Deplacement des eclairs ---- */
void deplacer_eclairs(ListeEclairs *le) {
    Eclair *e = le->tete;
    while (e != NULL) {
        if (e->actif) {
            e->y += e->vitesse;
            if (e->y > ZONE_JEU_BOTTOM) e->actif = 0;
        }
        e = e->suivant;
    }
}

/* ---- Deplacement des bonus ---- */
void deplacer_bonus(ListeBonus *lbon) {
    BonusObjet *bo = lbon->tete;
    while (bo != NULL) {
        if (bo->actif && !bo->au_sol) {
            bo->vy += GRAVITE;
            bo->y += bo->vy;
            if (bo->y >= ZONE_JEU_BOTTOM - 10) {
                bo->y = ZONE_JEU_BOTTOM - 10;
                bo->au_sol = 1;
                bo->vy = 0;
            }
        }
        bo = bo->suivant;
    }
}

/* ---- Deplacement du boss ---- */
void boss_mvt(Boss *boss) {
    if (!boss->vivant) return;

    /* Vitesse augmente quand pv diminue */
    float facteur_vitesse = 1.0f + (float)(boss->pv_max - boss->pv) * 0.15f;
    boss->x += boss->vx * facteur_vitesse;

    /* Rebond sur les bords */
    if (boss->x - boss->largeur / 2 < 0) {
        boss->x = boss->largeur / 2;
        boss->vx = -boss->vx;
    }
    if (boss->x + boss->largeur / 2 > LARGEUR_FENETRE) {
        boss->x = LARGEUR_FENETRE - boss->largeur / 2;
        boss->vx = -boss->vx;
    }

    /* Mouvement vertical leger en sinusoide */
    boss->y += boss->vy;
    if (boss->y < ZONE_JEU_TOP + 50) {
        boss->y = ZONE_JEU_TOP + 50;
        boss->vy = 0.5f;
    }
    if (boss->y > ZONE_JEU_TOP + 150) {
        boss->y = ZONE_JEU_TOP + 150;
        boss->vy = -0.5f;
    }

    if (boss->timer_tir > 0) boss->timer_tir--;
}

/* ---- Mise a jour globale des positions des entites
   (hors joueur, dont le deplacement depend des commandes
   passees depuis la boucle de jeu pour respecter le decouplage) ---- */
void maj_positions(Jeu *jeu) {
    bulles_mvt(&jeu->niveau->liste_bulles, jeu->joueur->ralenti);
    deplacer_projectiles(&jeu->niveau->liste_projectiles);
    deplacer_eclairs(&jeu->niveau->liste_eclairs);
    deplacer_bonus(&jeu->niveau->liste_bonus);
    if (jeu->niveau->has_boss) boss_mvt(&jeu->niveau->boss);
}

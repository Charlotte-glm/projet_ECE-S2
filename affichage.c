#include <stdio.h>
#include <allegro.h>
#include "affichage.h"

BITMAP *buffer = NULL;

/* ---- Couleurs (calculees apres le set_gfx_mode) ---- */
static int COL_FOND_HUD;
static int COL_FOND_JEU;
static int COL_SOL;
static int COL_JOUEUR;
static int COL_BOUCLIER;
static int COL_PROJECTILE;
static int COL_BULLE;
static int COL_BULLE_ECLAIR;
static int COL_ECLAIR;
static int COL_BOSS;
static int COL_BONUS_RAPIDE;
static int COL_BONUS_DOUBLE;
static int COL_BONUS_BOUCLIER;
static int COL_BONUS_LENT;
static int COL_HUD_TEXTE;
static int COL_BLANC;
static int COL_NOIR;
static int COL_ROUGE;

/* ---- Init du module affichage ---- */
int init_affichage(void) {
    buffer = create_bitmap(LARGEUR_FENETRE, HAUTEUR_FENETRE);
    if (buffer == NULL) return 0;

    COL_FOND_HUD       = makecol(20, 25, 40);
    COL_FOND_JEU       = makecol(80, 130, 200);
    COL_SOL            = makecol(60, 35, 20);
    COL_JOUEUR         = makecol(255, 200, 60);
    COL_BOUCLIER       = makecol(0, 200, 255);
    COL_PROJECTILE     = makecol(255, 255, 100);
    COL_BULLE          = makecol(220, 60, 60);
    COL_BULLE_ECLAIR   = makecol(180, 60, 220);
    COL_ECLAIR         = makecol(255, 240, 80);
    COL_BOSS           = makecol(150, 30, 30);
    COL_BONUS_RAPIDE   = makecol(255, 150, 0);
    COL_BONUS_DOUBLE   = makecol(0, 255, 100);
    COL_BONUS_BOUCLIER = makecol(0, 200, 255);
    COL_BONUS_LENT     = makecol(150, 80, 220);
    COL_HUD_TEXTE      = makecol(255, 255, 255);
    COL_BLANC          = makecol(255, 255, 255);
    COL_NOIR           = makecol(0, 0, 0);
    COL_ROUGE          = makecol(255, 50, 50);

    return 1;
}

void liberer_affichage(void) {
    if (buffer != NULL) {
        destroy_bitmap(buffer);
        buffer = NULL;
    }
}

/* ---- Affichage du fond du niveau ---- */
void affich_fond(Niveau *n) {
    int teinte;
    /* Couleur de fond legerement variable selon le numero de niveau */
    switch (n->numero) {
        case 1: teinte = makecol(80, 130, 200); break;
        case 2: teinte = makecol(200, 130, 80); break;
        case 3: teinte = makecol(70, 90, 130); break;
        case 4: teinte = makecol(130, 50, 80); break;
        default: teinte = COL_FOND_JEU; break;
    }
    rectfill(buffer, 0, ZONE_JEU_TOP, LARGEUR_FENETRE, ZONE_JEU_BOTTOM, teinte);
    /* Sol */
    rectfill(buffer, 0, ZONE_JEU_BOTTOM, LARGEUR_FENETRE, HAUTEUR_FENETRE, COL_SOL);
}

/* ---- Affichage du joueur ---- */
void affich_joueur(Joueur *j) {
    if (!j->vivant) return;
    int x1 = (int)(j->x - j->largeur / 2);
    int y1 = (int)(j->y - j->hauteur / 2);
    int x2 = (int)(j->x + j->largeur / 2);
    int y2 = (int)(j->y + j->hauteur / 2);

    rectfill(buffer, x1, y1, x2, y2, COL_JOUEUR);
    rect(buffer, x1, y1, x2, y2, COL_NOIR);

    /* Bouclier : halo bleu */
    if (j->bouclier) {
        circle(buffer, (int)j->x, (int)j->y, j->largeur, COL_BOUCLIER);
        circle(buffer, (int)j->x, (int)j->y, j->largeur + 2, COL_BOUCLIER);
    }
}

/* ---- Affichage des bulles ---- */
void affich_bulles(ListeBulles *lb) {
    Bulle *b = lb->tete;
    int couleur;
    while (b != NULL) {
        if (b->vivante) {
            couleur = b->peut_eclair ? COL_BULLE_ECLAIR : COL_BULLE;
            circlefill(buffer, (int)b->x, (int)b->y, b->rayon, couleur);
            circle(buffer, (int)b->x, (int)b->y, b->rayon, COL_NOIR);
            /* Reflet pour effet de bulle */
            circlefill(buffer,
                       (int)(b->x - b->rayon * 0.4),
                       (int)(b->y - b->rayon * 0.4),
                       b->rayon / 5, COL_BLANC);
        }
        b = b->suivante;
    }
}

/* ---- Affichage des projectiles ---- */
void affich_projectiles(ListeProjectiles *lp) {
    Projectile *p = lp->tete;
    while (p != NULL) {
        if (p->actif) {
            rectfill(buffer, (int)p->x - 2, (int)p->y - 8,
                     (int)p->x + 2, (int)p->y + 4, COL_PROJECTILE);
        }
        p = p->suivant;
    }
}

/* ---- Affichage des eclairs ---- */
void affich_eclairs(ListeEclairs *le) {
    Eclair *e = le->tete;
    while (e != NULL) {
        if (e->actif) {
            line(buffer, (int)e->x, (int)e->y, (int)e->x, (int)e->y + 16, COL_ECLAIR);
            line(buffer, (int)e->x - 1, (int)e->y, (int)e->x - 1, (int)e->y + 16, COL_ECLAIR);
            line(buffer, (int)e->x + 1, (int)e->y, (int)e->x + 1, (int)e->y + 16, COL_ECLAIR);
        }
        e = e->suivant;
    }
}

/* ---- Affichage des bonus / malus au sol ---- */
void affich_bonus_objets(ListeBonus *lbon) {
    BonusObjet *bo = lbon->tete;
    int couleur;
    char *lettre;
    while (bo != NULL) {
        if (bo->actif) {
            switch (bo->type) {
                case BONUS_ARME_RAPIDE:   couleur = COL_BONUS_RAPIDE;   lettre = "R"; break;
                case BONUS_ARME_DOUBLE:   couleur = COL_BONUS_DOUBLE;   lettre = "D"; break;
                case BONUS_BOUCLIER:      couleur = COL_BONUS_BOUCLIER; lettre = "B"; break;
                case MALUS_LENT:          couleur = COL_BONUS_LENT;     lettre = "L"; break;
                default:                  couleur = COL_BLANC;          lettre = "?"; break;
            }
            rectfill(buffer, (int)bo->x - 12, (int)bo->y - 12,
                     (int)bo->x + 12, (int)bo->y + 12, couleur);
            rect(buffer, (int)bo->x - 12, (int)bo->y - 12,
                 (int)bo->x + 12, (int)bo->y + 12, COL_NOIR);
            textout_centre_ex(buffer, font, lettre, (int)bo->x, (int)bo->y - 4,
                              COL_NOIR, -1);
        }
        bo = bo->suivant;
    }
}

/* ---- Affichage du boss ---- */
void affich_boss(Boss *boss) {
    if (!boss->vivant) return;
    int x1 = (int)(boss->x - boss->largeur / 2);
    int y1 = (int)(boss->y - boss->hauteur / 2);
    int x2 = (int)(boss->x + boss->largeur / 2);
    int y2 = (int)(boss->y + boss->hauteur / 2);

    rectfill(buffer, x1, y1, x2, y2, COL_BOSS);
    rect(buffer, x1, y1, x2, y2, COL_NOIR);
    /* Yeux */
    circlefill(buffer, x1 + 18, y1 + 18, 5, COL_BLANC);
    circlefill(buffer, x2 - 18, y1 + 18, 5, COL_BLANC);
    circlefill(buffer, x1 + 18, y1 + 18, 2, COL_NOIR);
    circlefill(buffer, x2 - 18, y1 + 18, 2, COL_NOIR);

    /* Barre de vie au-dessus */
    int largeur_barre = 100;
    int x_barre = (int)(boss->x - largeur_barre / 2);
    int y_barre = y1 - 12;
    rectfill(buffer, x_barre, y_barre, x_barre + largeur_barre, y_barre + 6, COL_NOIR);
    int largeur_pv = (largeur_barre * boss->pv) / boss->pv_max;
    if (largeur_pv > 0) {
        rectfill(buffer, x_barre, y_barre,
                 x_barre + largeur_pv, y_barre + 6, COL_ROUGE);
    }
}

/* ---- Affichage du HUD ---- */
void affich_hud(Joueur *j, Niveau *n) {
    char buf[64];
    rectfill(buffer, 0, 0, LARGEUR_FENETRE, HUD_HEIGHT, COL_FOND_HUD);
    line(buffer, 0, HUD_HEIGHT, LARGEUR_FENETRE, HUD_HEIGHT, COL_BLANC);

    sprintf(buf, "Pseudo: %s", j->pseudo);
    textout_ex(buffer, font, buf, 15, 10, COL_HUD_TEXTE, -1);

    sprintf(buf, "Niveau %d", n->numero);
    textout_ex(buffer, font, buf, 15, 35, COL_HUD_TEXTE, -1);

    sprintf(buf, "Score: %d", j->score);
    textout_ex(buffer, font, buf, 220, 22, COL_HUD_TEXTE, -1);

    int sec = n->temps_restant / 60;
    sprintf(buf, "Temps: %d", sec);
    textout_ex(buffer, font, buf, 380, 22, COL_HUD_TEXTE, -1);

    char *arme_str;
    switch (j->arme) {
        case ARME_RAPIDE: arme_str = "Arme: RAPIDE"; break;
        case ARME_DOUBLE: arme_str = "Arme: DOUBLE"; break;
        default:          arme_str = "Arme: BASE";   break;
    }
    textout_ex(buffer, font, arme_str, 520, 10, COL_HUD_TEXTE, -1);

    if (j->bouclier) {
        textout_ex(buffer, font, "BOUCLIER ACTIF", 520, 35, COL_BOUCLIER, -1);
    } else if (j->ralenti) {
        textout_ex(buffer, font, "RALENTI", 520, 35, COL_BONUS_LENT, -1);
    }
}

/* ---- Affichage complet d'un frame ---- */
void affich_EcranJeu(Jeu *jeu) {
    affich_fond(jeu->niveau);
    affich_bonus_objets(&jeu->niveau->liste_bonus);
    affich_bulles(&jeu->niveau->liste_bulles);
    affich_projectiles(&jeu->niveau->liste_projectiles);
    affich_eclairs(&jeu->niveau->liste_eclairs);
    if (jeu->niveau->has_boss) affich_boss(&jeu->niveau->boss);
    affich_joueur(jeu->joueur);
    affich_hud(jeu->joueur, jeu->niveau);
}

/* ---- Mise a jour de l'ecran (blit du buffer) ---- */
void Maj_Ecran(void) {
    blit(buffer, screen, 0, 0, 0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE);
}


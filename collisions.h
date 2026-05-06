/* ============================================================
   collisions.h
   Module Collisions : detection et traitement des collisions
   entre les differentes entites du jeu.
   ============================================================ */

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "types.h"

/* Detection */
int collis_bullemur(Bulle *b);
int collis_projecbulle(Projectile *p, Bulle *b);
int collis_bullejoueur(Bulle *b, Joueur *j);
int collis_eclairjoueur(Eclair *e, Joueur *j);
int collis_bonusjoueur(BonusObjet *bo, Joueur *j);
int collis_projecboss(Projectile *p, Boss *boss);
int collis_bossjoueur(Boss *boss, Joueur *j);

/* Traitement */
void traitercollis_projbulle(Niveau *n, Joueur *j, Projectile *p, Bulle *b);
void traitercollis_bullejoueur(Joueur *j);
void traitercollis_bonusjoueur(BonusObjet *bo, Joueur *j);

/* Gestion globale */
void gerer_collisions(Jeu *jeu);

#endif /* COLLISIONS_H */

/* ============================================================
   entites.h
   Module Entites : creation, ajout et suppression du joueur,
   des bulles, des projectiles, eclairs et bonus.
   ============================================================ */

#ifndef ENTITES_H
#define ENTITES_H

#include "types.h"

/* Initialisation des entites */
void init_joueur(Joueur *j, const char *pseudo);
void init_bulle(Bulle *b, float x, float y, int taille, float vx, float vy);
void init_projectile(Projectile *p, float x, float y, int type);
void init_niveau_entites(Niveau *n, int numero);

/* Gestion liste bulles */
void ajt_bulle(ListeBulles *lb, Bulle b);
void suppr_bulle(ListeBulles *lb, Bulle *b);
void vider_liste_bulles(ListeBulles *lb);

/* Gestion liste projectiles */
void ajt_projectile(ListeProjectiles *lp, Projectile p);
void suppr_projectile(ListeProjectiles *lp, Projectile *p);
void vider_liste_projectiles(ListeProjectiles *lp);

/* Gestion liste eclairs */
void ajt_eclair(ListeEclairs *le, float x, float y);
void vider_liste_eclairs(ListeEclairs *le);

/* Gestion liste bonus */
void ajt_bonus(ListeBonus *lbon, float x, float y, int type);
void vider_liste_bonus(ListeBonus *lbon);

/* Reinitialisation totale d'un niveau */
void reinit_NivEntites(Niveau *n);

/* Calcul du rayon en fonction de la taille */
int rayon_pour_taille(int taille);

#endif /* ENTITES_H */

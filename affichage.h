#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <allegro.h>
#include "types.h"

/* Buffer global pour le double-buffering */
extern BITMAP *buffer;

/* Initialisation et liberation des ressources d'affichage */
int  init_affichage(void);
void liberer_affichage(void);

/* Rendu */
void affich_fond(Niveau *n);
void affich_joueur(Joueur *j);
void affich_bulles(ListeBulles *lb);
void affich_projectiles(ListeProjectiles *lp);
void affich_eclairs(ListeEclairs *le);
void affich_bonus_objets(ListeBonus *lbon);
void affich_boss(Boss *boss);
void affich_hud(Joueur *j, Niveau *n);
void affich_EcranJeu(Jeu *jeu);
void Maj_Ecran(void);

#endif /* AFFICHAGE_H */

/* ============================================================
   deplacements.h
   Module Deplacements : mise a jour des positions du joueur,
   des bulles, projectiles, eclairs et bonus.
   ============================================================ */

#ifndef DEPLACEMENTS_H
#define DEPLACEMENTS_H

#include "types.h"

void joueur_mvt(Joueur *j, Commandes cmd);
void bulles_mvt(ListeBulles *lb, int ralenti);
void deplacer_projectiles(ListeProjectiles *lp);
void deplacer_eclairs(ListeEclairs *le);
void deplacer_bonus(ListeBonus *lbon);
void rebond_bulle(Bulle *b);
void boss_mvt(Boss *boss);

/* Mise a jour globale de toutes les positions */
void maj_positions(Jeu *jeu);

#endif /* DEPLACEMENTS_H */


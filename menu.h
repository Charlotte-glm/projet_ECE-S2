/* ============================================================
   menu.h
   Module Menu : affichage et navigation du menu principal,
   regles, et menus de fin de niveau.
   ============================================================ */

#ifndef MENU_H
#define MENU_H

#include "types.h"

#define MENU_REGLES        0
#define MENU_NOUVELLE      1
#define MENU_REPRENDRE     2
#define MENU_QUITTER       3

#define FIN_REJOUER        0
#define FIN_SUIVANT        1
#define FIN_SAUVEGARDER    2
#define FIN_MENU           3

void affich_MenuPrinc(int choix_actif);
void affich_regles(void);
int  GererMenu(void);
int  affich_FinNiveau(int victoire, int score, int temps_restant);
void affich_VictoireFinale(Joueur *j);
void affich_GameOver(void);
void affich_MeilleursScores(void);
void affich_Decompte(void);

#endif /* MENU_H */

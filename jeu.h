/* ============================================================
   jeu.h
   Module Jeu : pilote la boucle de jeu, l'enchainement des
   niveaux et la gestion d'une partie.
   ============================================================ */

#ifndef JEU_H
#define JEU_H

#include "types.h"

/* Initialisation */
void init_partie(Partie *p, Joueur *j, int niveau_depart);
void init_niveau(Niveau *n, int numero);

/* Lancement */
void LancerPartie(Partie *p);
int  boucle_jeu(Jeu *jeu);            /* 1 = victoire, 0 = defaite */
int  boucle_jeu_boss(Jeu *jeu);

/* Etat */
int  verif_FinNiveau(Niveau *n, Joueur *j);  /* 1 victoire, -1 defaite, 0 en cours */
void NivSuivant(Partie *p);
void FinPartie(Partie *p);

/* Sauvegarde / chargement */
int  sauver_partie(Partie *p);
int  charger_partie(const char *pseudo, int *niveau_charge, int *score_charge);

/* Meilleurs scores */
void enregistrer_score(const char *pseudo, int score, int niveau);
int  lire_meilleurs_scores(ScoreEntry *scores, int max);

#endif /* JEU_H */


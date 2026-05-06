/* ============================================================
   jeu.c
   Implementation du module Jeu.
   Pilote la boucle de jeu, l'enchainement des niveaux,
   la sauvegarde et le tableau des meilleurs scores.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "jeu.h"
#include "entites.h"
#include "deplacements.h"
#include "collisions.h"
#include "ihm.h"
#include "menu.h"
#include "affichage.h"

#define FICHIER_SAUVEGARDE  "sauvegardes.txt"
#define FICHIER_SCORES      "scores.txt"

/* ---- Compteur de frames pour le timer Allegro ---- */
static volatile int compteur_frames = 0;
static void incrementer_compteur(void) {
    compteur_frames++;
}
END_OF_FUNCTION(incrementer_compteur)

/* ---- Initialisation du timer (60 fps) ---- */
static int timer_installe = 0;
static void installer_timer(void) {
    if (!timer_installe) {
        LOCK_VARIABLE(compteur_frames);
        LOCK_FUNCTION(incrementer_compteur);
        install_int_ex(incrementer_compteur, BPS_TO_TIMER(60));
        timer_installe = 1;
    }
}

/* ---- Initialisation d'une partie ---- */
void init_partie(Partie *p, Joueur *j, int niveau_depart) {
    p->joueur = j;
    p->niveau_courant = niveau_depart;
    p->nb_niveaux = NB_NIVEAUX;
    p->en_cours = 1;
}

/* ---- Initialisation d'un niveau ---- */
void init_niveau(Niveau *n, int numero) {
    n->liste_bulles.tete = NULL;
    n->liste_bulles.nb = 0;
    n->liste_projectiles.tete = NULL;
    n->liste_projectiles.nb = 0;
    n->liste_eclairs.tete = NULL;
    n->liste_eclairs.nb = 0;
    n->liste_bonus.tete = NULL;
    n->liste_bonus.nb = 0;
    init_niveau_entites(n, numero);
}

/* ---- Verification de la fin de niveau ---- */
int verif_FinNiveau(Niveau *n, Joueur *j) {
    if (!j->vivant)             return -1; /* defaite */
    if (n->temps_restant <= 0)  return -1;
    if (n->has_boss) {
        if (!n->boss.vivant)    return  1;
    } else {
        if (n->liste_bulles.nb == 0) return 1;
    }
    return 0;
}

/* ---- Helper : tirer un projectile ---- */
static void tirer(Joueur *j, ListeProjectiles *lp) {
    Projectile p;
    int delai_min;

    /* Cadence selon l'arme */
    switch (j->arme) {
        case ARME_RAPIDE: delai_min = 8;  break;
        case ARME_DOUBLE: delai_min = 14; break;
        default:          delai_min = 20; break;
    }
    if (j->timer_tir > 0) return;

    /* Limitation 1 tir a la fois pour l'arme de base */
    if (j->arme == ARME_BASE && lp->nb >= 1) return;

    if (j->arme == ARME_DOUBLE) {
        init_projectile(&p, j->x - 10, j->y - j->hauteur / 2, ARME_DOUBLE);
        ajt_projectile(lp, p);
        init_projectile(&p, j->x + 10, j->y - j->hauteur / 2, ARME_DOUBLE);
        ajt_projectile(lp, p);
    } else {
        init_projectile(&p, j->x, j->y - j->hauteur / 2, j->arme);
        ajt_projectile(lp, p);
    }
    j->timer_tir = delai_min;
}

/* ---- Boucle de jeu : 1 = victoire, 0 = defaite ---- */
int boucle_jeu(Jeu *jeu) {
    Commandes cmd;
    int etat = 0;
    int dernier_compteur = compteur_frames;
    int frames_a_traiter;
    Bulle *b;

    installer_timer();
    compteur_frames = 0;
    dernier_compteur = 0;

    /* Decompte 3-2-1 avant de demarrer */
    affich_Decompte();
    compteur_frames = 0;
    dernier_compteur = 0;

    while (etat == 0) {
        /* Synchronisation a 60 fps */
        frames_a_traiter = compteur_frames - dernier_compteur;
        if (frames_a_traiter <= 0) {
            rest(1);
            continue;
        }
        dernier_compteur = compteur_frames;

        if (frames_a_traiter > 5) frames_a_traiter = 5; /* anti-decrochage */

        while (frames_a_traiter--) {
            LireClavier(&cmd);
            if (cmd.quitter) return 0;

            /* Logique metier : deplacements puis collisions */
            joueur_mvt(jeu->joueur, cmd);
            if (cmd.tir) tirer(jeu->joueur, &jeu->niveau->liste_projectiles);

            maj_positions(jeu);

            /* Eclairs : certaines bulles tirent quand leur timer arrive a 0 */
            if (jeu->niveau->eclairs_actifs) {
                b = jeu->niveau->liste_bulles.tete;
                while (b != NULL) {
                    if (b->vivante && b->peut_eclair) {
                        if (b->timer_eclair <= 0) {
                            ajt_eclair(&jeu->niveau->liste_eclairs, b->x, b->y);
                            b->timer_eclair = 180 + (rand() % 120);
                        }
                    }
                    b = b->suivante;
                }
            }

            /* Boss : emet des bulles regulierement */
            if (jeu->niveau->has_boss && jeu->niveau->boss.vivant
                && jeu->niveau->boss.timer_tir <= 0) {
                Bulle nouvelle;
                init_bulle(&nouvelle, jeu->niveau->boss.x,
                           jeu->niveau->boss.y + jeu->niveau->boss.hauteur / 2,
                           2, (rand() % 2 == 0) ? -3.0f : 3.0f, 1.0f);
                ajt_bulle(&jeu->niveau->liste_bulles, nouvelle);
                jeu->niveau->boss.timer_tir = 180;
            }

            gerer_collisions(jeu);

            /* Decremente le temps */
            if (jeu->niveau->temps_restant > 0) jeu->niveau->temps_restant--;

            etat = verif_FinNiveau(jeu->niveau, jeu->joueur);
            if (etat != 0) break;
        }

        /* Rendu */
        affich_EcranJeu(jeu);
        Maj_Ecran();
    }

    /* Bonus de score sur le temps restant */
    if (etat == 1) {
        jeu->joueur->score += (jeu->niveau->temps_restant / 60) * 10;
    }

    return (etat == 1) ? 1 : 0;
}

/* ---- Lancement de la partie ---- */
void LancerPartie(Partie *p) {
    Niveau niveau;
    Jeu jeu;
    int resultat;
    int choix;
    int continuer = 1;

    init_niveau(&niveau, p->niveau_courant);
    jeu.partie = p;
    jeu.niveau = &niveau;
    jeu.joueur = p->joueur;

    while (continuer && p->en_cours) {
        /* Reinitialise le joueur a chaque niveau (position / vie) */
        p->joueur->x = LARGEUR_FENETRE / 2.0f;
        p->joueur->y = ZONE_JEU_BOTTOM - 30;
        p->joueur->vivant = 1;
        p->joueur->timer_tir = 0;

        init_niveau(&niveau, p->niveau_courant);

        resultat = boucle_jeu(&jeu);

        if (resultat == 1) {
            /* Victoire de niveau */
            int score = p->joueur->score;
            int temps = niveau.temps_restant / 60;

            /* Si c'est le dernier niveau, victoire finale */
            if (p->niveau_courant >= p->nb_niveaux) {
                affich_VictoireFinale(p->joueur);
                enregistrer_score(p->joueur->pseudo, p->joueur->score,
                                  p->niveau_courant);
                p->en_cours = 0;
                continuer = 0;
                break;
            }

            choix = affich_FinNiveau(1, score, temps);
            if (choix == FIN_SUIVANT) {
                NivSuivant(p);
            } else if (choix == FIN_SAUVEGARDER) {
                sauver_partie(p);
                NivSuivant(p);
            } else {
                p->en_cours = 0;
                continuer = 0;
            }
        } else {
            /* Defaite */
            choix = affich_FinNiveau(0, p->joueur->score, 0);
            if (choix == FIN_REJOUER) {
                /* on rejoue le meme niveau */
            } else if (choix == FIN_SAUVEGARDER) {
                sauver_partie(p);
                p->en_cours = 0;
                continuer = 0;
            } else {
                p->en_cours = 0;
                continuer = 0;
            }
        }
    }

    /* Liberation memoire */
    vider_liste_bulles(&niveau.liste_bulles);
    vider_liste_projectiles(&niveau.liste_projectiles);
    vider_liste_eclairs(&niveau.liste_eclairs);
    vider_liste_bonus(&niveau.liste_bonus);
}

/* ---- Passage au niveau suivant ---- */
void NivSuivant(Partie *p) {
    if (p->niveau_courant < p->nb_niveaux) p->niveau_courant++;
}

/* ---- Fin de partie ---- */
void FinPartie(Partie *p) {
    p->en_cours = 0;
}

/* ============================================================
   Sauvegarde et chargement
   Format des fichiers : texte simple, une entree par ligne :
       pseudo niveau score
   ============================================================ */

int sauver_partie(Partie *p) {
    FILE *f_in;
    FILE *f_out;
    char ligne[256];
    char pseudo[TAILLE_PSEUDO];
    int niveau, score;
    int trouve = 0;

    /* On lit toutes les sauvegardes existantes en sautant celle du joueur */
    f_in = fopen(FICHIER_SAUVEGARDE, "r");
    f_out = fopen("sauvegardes.tmp", "w");
    if (f_out == NULL) {
        if (f_in) fclose(f_in);
        return 0;
    }

    if (f_in != NULL) {
        while (fgets(ligne, sizeof(ligne), f_in)) {
            if (sscanf(ligne, "%31s %d %d", pseudo, &niveau, &score) == 3) {
                if (strcmp(pseudo, p->joueur->pseudo) == 0) {
                    /* On remplace par les nouvelles donnees */
                    fprintf(f_out, "%s %d %d\n", p->joueur->pseudo,
                            p->niveau_courant, p->joueur->score);
                    trouve = 1;
                } else {
                    fputs(ligne, f_out);
                }
            }
        }
        fclose(f_in);
    }
    if (!trouve) {
        fprintf(f_out, "%s %d %d\n", p->joueur->pseudo,
                p->niveau_courant, p->joueur->score);
    }
    fclose(f_out);

    remove(FICHIER_SAUVEGARDE);
    rename("sauvegardes.tmp", FICHIER_SAUVEGARDE);
    return 1;
}

int charger_partie(const char *pseudo, int *niveau_charge, int *score_charge) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    char ligne[256];
    char p[TAILLE_PSEUDO];
    int niv, sc;

    if (f == NULL) return 0;
    while (fgets(ligne, sizeof(ligne), f)) {
        if (sscanf(ligne, "%31s %d %d", p, &niv, &sc) == 3) {
            if (strcmp(p, pseudo) == 0) {
                *niveau_charge = niv;
                *score_charge = sc;
                fclose(f);
                return 1;
            }
        }
    }
    fclose(f);
    return 0;
}

/* ============================================================
   Tableau des meilleurs scores
   ============================================================ */

void enregistrer_score(const char *pseudo, int score, int niveau) {
    FILE *f;
    ScoreEntry scores[20];
    int n = 0;
    int i, j;
    char ligne[256];
    char p[TAILLE_PSEUDO];
    int sc, niv;

    f = fopen(FICHIER_SCORES, "r");
    if (f != NULL) {
        while (n < 20 && fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne, "%31s %d %d", p, &sc, &niv) == 3) {
                strcpy(scores[n].pseudo, p);
                scores[n].score = sc;
                scores[n].niveau_atteint = niv;
                n++;
            }
        }
        fclose(f);
    }

    /* Ajout de la nouvelle entree */
    if (n < 20) {
        strcpy(scores[n].pseudo, pseudo);
        scores[n].score = score;
        scores[n].niveau_atteint = niveau;
        n++;
    }

    /* Tri decroissant par score (tri a bulles, plus simple) */
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                ScoreEntry tmp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tmp;
            }
        }
    }

    /* Garder les 10 meilleurs */
    if (n > 10) n = 10;

    /* Reecriture du fichier */
    f = fopen(FICHIER_SCORES, "w");
    if (f != NULL) {
        for (i = 0; i < n; i++) {
            fprintf(f, "%s %d %d\n", scores[i].pseudo,
                    scores[i].score, scores[i].niveau_atteint);
        }
        fclose(f);
    }
}

int lire_meilleurs_scores(ScoreEntry *scores, int max) {
    FILE *f = fopen(FICHIER_SCORES, "r");
    char ligne[256];
    int n = 0;
    char p[TAILLE_PSEUDO];
    int sc, niv;

    if (f == NULL) return 0;
    while (n < max && fgets(ligne, sizeof(ligne), f)) {
        if (sscanf(ligne, "%31s %d %d", p, &sc, &niv) == 3) {
            strcpy(scores[n].pseudo, p);
            scores[n].score = sc;
            scores[n].niveau_atteint = niv;
            n++;
        }
    }
    fclose(f);
    return n;
}


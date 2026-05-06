/* ============================================================
   menu.c
   Implementation du module Menu.
   Affichage et navigation : menu principal, regles, fin de
   niveau, victoire finale, decompte, meilleurs scores.
   ============================================================ */

#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "menu.h"
#include "affichage.h"
#include "jeu.h"

extern BITMAP *buffer;

/* ---- Affichage du menu principal ---- */
void affich_MenuPrinc(int choix_actif) {
    char *libelles[4] = {
        "Regles du jeu",
        "Nouvelle partie",
        "Reprendre une partie",
        "Quitter"
    };
    int i;
    int couleur_titre = makecol(255, 220, 100);
    int couleur_fond  = makecol(20, 30, 60);
    int couleur_btn   = makecol(40, 60, 100);
    int couleur_actif = makecol(220, 100, 50);
    int couleur_blanc = makecol(255, 255, 255);

    clear_to_color(buffer, couleur_fond);

    /* Titre */
    textout_centre_ex(buffer, font, "SUPER BULLES",
                      LARGEUR_FENETRE / 2, 80, couleur_titre, -1);
    textout_centre_ex(buffer, font, "Inspire de Pang",
                      LARGEUR_FENETRE / 2, 110, couleur_blanc, -1);

    /* Boutons */
    for (i = 0; i < 4; i++) {
        int y = 220 + i * 70;
        int couleur = (i == choix_actif) ? couleur_actif : couleur_btn;
        rectfill(buffer, LARGEUR_FENETRE / 2 - 150, y,
                 LARGEUR_FENETRE / 2 + 150, y + 50, couleur);
        rect(buffer, LARGEUR_FENETRE / 2 - 150, y,
             LARGEUR_FENETRE / 2 + 150, y + 50, couleur_blanc);
        textout_centre_ex(buffer, font, libelles[i],
                          LARGEUR_FENETRE / 2, y + 22, couleur_blanc, -1);
    }

    /* Aide */
    textout_centre_ex(buffer, font,
                      "Fleches haut/bas pour naviguer - Entree pour valider",
                      LARGEUR_FENETRE / 2, HAUTEUR_FENETRE - 30, couleur_blanc, -1);

    Maj_Ecran();
}

/* ---- Affichage des regles ---- */
void affich_regles(void) {
    int couleur_fond  = makecol(20, 30, 60);
    int couleur_titre = makecol(255, 220, 100);
    int couleur_blanc = makecol(255, 255, 255);
    int y;
    char *regles[] = {
        "But du jeu : eliminer toutes les bulles de chaque niveau",
        "sans te faire toucher et avant la fin du temps imparti.",
        "",
        "Controles :",
        "  Fleches gauche / droite : se deplacer",
        "  Espace : tirer",
        "  Echap : quitter",
        "",
        "Bonus a ramasser au sol :",
        "  R = arme rapide        D = arme double tir",
        "  B = bouclier           L = malus ralentissement",
        "",
        "Quand une bulle est touchee, elle se divise en deux",
        "bulles plus petites. La plus petite disparait au tir suivant.",
        "",
        "A partir du niveau 3, certaines bulles tirent des eclairs.",
        "Au niveau 4, affronte le BOSS pour gagner la partie !",
        "",
        "Appuie sur Entree pour revenir au menu."
    };
    int n_lignes = sizeof(regles) / sizeof(regles[0]);
    int i;

    clear_keybuf();
    while (1) {
        clear_to_color(buffer, couleur_fond);
        textout_centre_ex(buffer, font, "REGLES DU JEU",
                          LARGEUR_FENETRE / 2, 50, couleur_titre, -1);
        y = 110;
        for (i = 0; i < n_lignes; i++) {
            textout_ex(buffer, font, regles[i], 60, y, couleur_blanc, -1);
            y += 22;
        }
        Maj_Ecran();

        if (key[KEY_ENTER] || key[KEY_ESC]) break;
        rest(20);
    }
    clear_keybuf();
}

/* ---- Affichage de la saisie du pseudo (appele depuis ihm.c) ---- */
void affich_SaisiePseudo(const char *pseudo) {
    char buf[TAILLE_PSEUDO + 4];
    int couleur_fond = makecol(20, 30, 60);
    int couleur_blanc = makecol(255, 255, 255);
    int couleur_jaune = makecol(255, 220, 100);

    clear_to_color(buffer, couleur_fond);
    textout_centre_ex(buffer, font, "ENTRER VOTRE PSEUDO",
                      LARGEUR_FENETRE / 2, 200, couleur_jaune, -1);
    rectfill(buffer, LARGEUR_FENETRE / 2 - 150, 280,
             LARGEUR_FENETRE / 2 + 150, 320, makecol(40, 60, 100));
    rect(buffer, LARGEUR_FENETRE / 2 - 150, 280,
         LARGEUR_FENETRE / 2 + 150, 320, couleur_blanc);

    sprintf(buf, "%s_", pseudo);
    textout_centre_ex(buffer, font, buf,
                      LARGEUR_FENETRE / 2, 295, couleur_blanc, -1);

    textout_centre_ex(buffer, font,
                      "Entree pour valider, Backspace pour effacer",
                      LARGEUR_FENETRE / 2, 380, couleur_blanc, -1);
    Maj_Ecran();
}

/* ---- Gestion de la navigation dans le menu principal ---- */
int GererMenu(void) {
    int choix = MENU_NOUVELLE;
    int valide = 0;
    clear_keybuf();
    while (!valide) {
        affich_MenuPrinc(choix);

        if (keypressed()) {
            int k = readkey() >> 8;
            if (k == KEY_UP)    choix = (choix + 3) % 4;
            else if (k == KEY_DOWN)  choix = (choix + 1) % 4;
            else if (k == KEY_ENTER) valide = 1;
            else if (k == KEY_ESC)   { choix = MENU_QUITTER; valide = 1; }
        }
        rest(20);
    }
    clear_keybuf();
    return choix;
}

/* ---- Affichage du decompte 3-2-1 ---- */
void affich_Decompte(void) {
    int i;
    char buf[8];
    int couleur_blanc = makecol(255, 255, 255);

    for (i = 3; i >= 1; i--) {
        clear_to_color(buffer, makecol(0, 0, 0));
        sprintf(buf, "%d", i);
        textout_centre_ex(buffer, font, buf,
                          LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2, couleur_blanc, -1);
        textout_centre_ex(buffer, font, "PRET ?",
                          LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 + 40, couleur_blanc, -1);
        Maj_Ecran();
        rest(800);
    }
    clear_to_color(buffer, makecol(0, 0, 0));
    textout_centre_ex(buffer, font, "GO !",
                      LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2, makecol(100, 255, 100), -1);
    Maj_Ecran();
    rest(400);
    clear_keybuf();
}

/* ---- Menu de fin de niveau (gagne ou perdu) ---- */
int affich_FinNiveau(int victoire, int score, int temps_restant) {
    int choix;
    int valide = 0;
    int couleur_blanc = makecol(255, 255, 255);
    int couleur_jaune = makecol(255, 220, 100);
    int couleur_rouge = makecol(255, 80, 80);
    int couleur_actif = makecol(220, 100, 50);
    int couleur_btn   = makecol(40, 60, 100);

    char *opts_victoire[3] = {"Continuer", "Sauvegarder", "Retour menu"};
    char *opts_defaite[3]  = {"Recommencer", "Sauvegarder", "Retour menu"};
    char **opts = victoire ? opts_victoire : opts_defaite;
    int n_opts = 3;

    choix = 0;
    clear_keybuf();
    char buf[64];

    while (!valide) {
        rectfill(buffer, LARGEUR_FENETRE / 2 - 220, HAUTEUR_FENETRE / 2 - 150,
                 LARGEUR_FENETRE / 2 + 220, HAUTEUR_FENETRE / 2 + 150,
                 makecol(20, 30, 60));
        rect(buffer, LARGEUR_FENETRE / 2 - 220, HAUTEUR_FENETRE / 2 - 150,
             LARGEUR_FENETRE / 2 + 220, HAUTEUR_FENETRE / 2 + 150, couleur_blanc);

        if (victoire) {
            textout_centre_ex(buffer, font, "NIVEAU GAGNE !",
                              LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 120,
                              couleur_jaune, -1);
            sprintf(buf, "Score: %d", score);
            textout_centre_ex(buffer, font, buf,
                              LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 80,
                              couleur_blanc, -1);
            sprintf(buf, "Temps restant: %d s", temps_restant);
            textout_centre_ex(buffer, font, buf,
                              LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 60,
                              couleur_blanc, -1);
        } else {
            textout_centre_ex(buffer, font, "NIVEAU PERDU",
                              LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 120,
                              couleur_rouge, -1);
            sprintf(buf, "Score: %d", score);
            textout_centre_ex(buffer, font, buf,
                              LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 80,
                              couleur_blanc, -1);
        }

        for (int i = 0; i < n_opts; i++) {
            int y = HAUTEUR_FENETRE / 2 - 20 + i * 40;
            int c = (i == choix) ? couleur_actif : couleur_btn;
            rectfill(buffer, LARGEUR_FENETRE / 2 - 130, y,
                     LARGEUR_FENETRE / 2 + 130, y + 30, c);
            rect(buffer, LARGEUR_FENETRE / 2 - 130, y,
                 LARGEUR_FENETRE / 2 + 130, y + 30, couleur_blanc);
            textout_centre_ex(buffer, font, opts[i],
                              LARGEUR_FENETRE / 2, y + 12, couleur_blanc, -1);
        }
        Maj_Ecran();

        if (keypressed()) {
            int k = readkey() >> 8;
            if (k == KEY_UP)        choix = (choix + n_opts - 1) % n_opts;
            else if (k == KEY_DOWN) choix = (choix + 1) % n_opts;
            else if (k == KEY_ENTER) valide = 1;
            else if (k == KEY_ESC) { choix = 2; valide = 1; }
        }
        rest(20);
    }
    clear_keybuf();

    /* Conversion vers les codes attendus */
    if (victoire) {
        if (choix == 0) return FIN_SUIVANT;
        if (choix == 1) return FIN_SAUVEGARDER;
        return FIN_MENU;
    } else {
        if (choix == 0) return FIN_REJOUER;
        if (choix == 1) return FIN_SAUVEGARDER;
        return FIN_MENU;
    }
}

/* ---- Animation de victoire finale ---- */
void affich_VictoireFinale(Joueur *j) {
    int i;
    char buf[64];
    int couleur_jaune = makecol(255, 220, 100);
    int couleur_blanc = makecol(255, 255, 255);

    for (i = 0; i < 60; i++) {
        clear_to_color(buffer, makecol(20 + (i * 3) % 100, 0, 60));
        textout_centre_ex(buffer, font, "VICTOIRE !",
                          LARGEUR_FENETRE / 2, 150, couleur_jaune, -1);
        textout_centre_ex(buffer, font, "Tu as vaincu le BOSS !",
                          LARGEUR_FENETRE / 2, 200, couleur_blanc, -1);
        sprintf(buf, "Score final : %d", j->score);
        textout_centre_ex(buffer, font, buf,
                          LARGEUR_FENETRE / 2, 270, couleur_blanc, -1);
        sprintf(buf, "Bravo %s !", j->pseudo);
        textout_centre_ex(buffer, font, buf,
                          LARGEUR_FENETRE / 2, 320, couleur_jaune, -1);
        /* etoiles qui clignotent */
        int x;
        for (x = 0; x < 20; x++) {
            int sx = (x * 47 + i * 3) % LARGEUR_FENETRE;
            int sy = 400 + (x * 23) % 100;
            circlefill(buffer, sx, sy, 2, couleur_jaune);
        }
        Maj_Ecran();
        rest(50);
    }
    rest(800);

    /* Affichage des meilleurs scores apres victoire */
    affich_MeilleursScores();
}

/* ---- Game Over ---- */
void affich_GameOver(void) {
    int couleur_rouge = makecol(220, 50, 50);
    int couleur_blanc = makecol(255, 255, 255);
    clear_to_color(buffer, makecol(0, 0, 0));
    textout_centre_ex(buffer, font, "GAME OVER",
                      LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 20,
                      couleur_rouge, -1);
    textout_centre_ex(buffer, font,
                      "Appuie sur Entree pour revenir au menu.",
                      LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 + 30,
                      couleur_blanc, -1);
    Maj_Ecran();
    clear_keybuf();
    while (!key[KEY_ENTER] && !key[KEY_ESC]) rest(20);
    clear_keybuf();
}

/* ---- Affichage des meilleurs scores ---- */
void affich_MeilleursScores(void) {
    ScoreEntry scores[10];
    int n = lire_meilleurs_scores(scores, 10);
    int couleur_fond  = makecol(20, 30, 60);
    int couleur_titre = makecol(255, 220, 100);
    int couleur_blanc = makecol(255, 255, 255);
    char buf[120];
    int i, y;

    clear_keybuf();
    while (1) {
        clear_to_color(buffer, couleur_fond);
        textout_centre_ex(buffer, font, "MEILLEURS SCORES",
                          LARGEUR_FENETRE / 2, 60, couleur_titre, -1);

        if (n == 0) {
            textout_centre_ex(buffer, font, "Aucun score enregistre.",
                              LARGEUR_FENETRE / 2, 200, couleur_blanc, -1);
        } else {
            y = 130;
            for (i = 0; i < n; i++) {
                sprintf(buf, "%2d. %-20s  %6d  (niveau %d)",
                        i + 1, scores[i].pseudo, scores[i].score,
                        scores[i].niveau_atteint);
                textout_ex(buffer, font, buf, 130, y, couleur_blanc, -1);
                y += 25;
            }
        }
        textout_centre_ex(buffer, font,
                          "Entree pour revenir au menu.",
                          LARGEUR_FENETRE / 2, HAUTEUR_FENETRE - 40, couleur_blanc, -1);
        Maj_Ecran();
        if (key[KEY_ENTER] || key[KEY_ESC]) break;
        rest(20);
    }
    clear_keybuf();
}

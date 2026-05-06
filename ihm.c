/* ============================================================
   ihm.c
   Implementation du module IHM.
   Centralise la lecture du clavier, decouple de la logique metier.
   ============================================================ */

#include <allegro.h>
#include <string.h>
#include "ihm.h"

/* ---- Lecture du clavier et remplissage des commandes ---- */
void LireClavier(Commandes *cmd) {
    cmd->gauche  = key[KEY_LEFT]  ? 1 : 0;
    cmd->droite  = key[KEY_RIGHT] ? 1 : 0;
    cmd->tir     = key[KEY_SPACE] ? 1 : 0;
    cmd->entree  = key[KEY_ENTER] ? 1 : 0;
    cmd->quitter = key[KEY_ESC]   ? 1 : 0;
}

/* ---- Recuperation des commandes du joueur ---- */
Commandes recupCommd_joueur(void) {
    Commandes cmd;
    LireClavier(&cmd);
    return cmd;
}

/* ---- Saisie du pseudo caractere par caractere ----
   Bloque jusqu'a ce que le joueur appuie sur Entree.
   Le rendu de la fenetre de saisie est gere dans menu.c. */
void SaisirPseudo(char *pseudo, int taille_max) {
    int len = 0;
    int k;
    extern void affich_SaisiePseudo(const char *pseudo); /* defini dans menu.c */

    pseudo[0] = '\0';
    clear_keybuf();

    while (1) {
        affich_SaisiePseudo(pseudo);

        if (keypressed()) {
            k = readkey();
            int scancode = k >> 8;
            int ascii = k & 0xff;

            if (scancode == KEY_ENTER) {
                if (len > 0) break;
            } else if (scancode == KEY_BACKSPACE) {
                if (len > 0) {
                    len--;
                    pseudo[len] = '\0';
                }
            } else if (ascii >= 32 && ascii <= 126 && len < taille_max - 1) {
                pseudo[len] = (char)ascii;
                len++;
                pseudo[len] = '\0';
            } else if (scancode == KEY_ESC) {
                if (len == 0) {
                    strcpy(pseudo, "JOUEUR");
                    return;
                }
            }
        }
        rest(20);
    }
    if (len == 0) strcpy(pseudo, "JOUEUR");
}

/* ---- Validation d'un choix de menu ---- */
int ValiderChoix(int choix) {
    if (choix < 0) return 0;
    if (choix > 3) return 0;
    return 1;
}

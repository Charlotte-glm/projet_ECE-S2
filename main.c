/* ============================================================
   main.c
   Point d'entree du jeu Super Bulles.
   Initialise Allegro 4 puis lance la boucle des menus.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <allegro.h>

#include "types.h"
#include "entites.h"
#include "jeu.h"
#include "menu.h"
#include "ihm.h"
#include "affichage.h"

int main(void) {
    Joueur joueur;
    Partie partie;
    int choix;
    int continuer = 1;
    int niveau_charge, score_charge;
    char pseudo[TAILLE_PSEUDO];

    /* ---- Initialisation Allegro ---- */
    if (allegro_init() != 0) {
        fprintf(stderr, "Erreur init Allegro\n");
        return 1;
    }
    install_keyboard();
    install_timer();
    install_mouse();
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, LARGEUR_FENETRE, HAUTEUR_FENETRE, 0, 0) != 0) {
        if (set_gfx_mode(GFX_SAFE, LARGEUR_FENETRE, HAUTEUR_FENETRE, 0, 0) != 0) {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Erreur init mode graphique :\n%s\n",
                            allegro_error);
            return 1;
        }
    }
    set_window_title("Super Bulles");
    srand((unsigned)time(NULL));

    if (!init_affichage()) {
        allegro_message("Erreur creation buffer\n");
        return 1;
    }

    /* ---- Boucle principale des menus ---- */
    while (continuer) {
        choix = GererMenu();
        switch (choix) {
            case MENU_REGLES:
                affich_regles();
                break;

            case MENU_NOUVELLE:
                SaisirPseudo(pseudo, TAILLE_PSEUDO);
                init_joueur(&joueur, pseudo);
                init_partie(&partie, &joueur, 1);
                LancerPartie(&partie);
                break;

            case MENU_REPRENDRE:
                SaisirPseudo(pseudo, TAILLE_PSEUDO);
                if (charger_partie(pseudo, &niveau_charge, &score_charge)) {
                    init_joueur(&joueur, pseudo);
                    joueur.score = score_charge;
                    init_partie(&partie, &joueur, niveau_charge);
                    LancerPartie(&partie);
                } else {
                    extern BITMAP *buffer;
                    int couleur_blanc = makecol(255, 255, 255);
                    int couleur_rouge = makecol(255, 80, 80);
                    clear_to_color(buffer, makecol(20, 30, 60));
                    textout_centre_ex(buffer, font, "PSEUDO INCONNU",
                                      LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 - 20,
                                      couleur_rouge, -1);
                    textout_centre_ex(buffer, font,
                        "Aucune sauvegarde trouvee. Entree pour revenir.",
                        LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2 + 20,
                        couleur_blanc, -1);
                    Maj_Ecran();
                    clear_keybuf();
                    while (!key[KEY_ENTER] && !key[KEY_ESC]) rest(20);
                    clear_keybuf();
                }
                break;

            case MENU_QUITTER:
            default:
                continuer = 0;
                break;
        }
    }

    liberer_affichage();
    allegro_exit();
    return 0;
}
END_OF_MAIN()

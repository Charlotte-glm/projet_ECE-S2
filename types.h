/* ============================================================
   types.h
   Definition des structures de donnees du jeu Super Bulles.
   Centralise tous les types pour les autres modules.
   ============================================================ */

#ifndef TYPES_H
#define TYPES_H

/* ---- Dimensions de la fenetre (passees a set_gfx_mode) ----
   Dans tout le reste du code on utilise les macros LARGEUR_FENETRE et
   HAUTEUR_FENETRE d'Allegro pour calculer dynamiquement les positions
   et garantir l'adaptabilite de l'affichage (responsive). */
#define LARGEUR_FENETRE    800
#define HAUTEUR_FENETRE    600
#define HUD_HEIGHT         60
#define ZONE_JEU_TOP       HUD_HEIGHT
#define ZONE_JEU_BOTTOM    (HAUTEUR_FENETRE - 20)

#define NB_NIVEAUX         4
#define TAILLE_PSEUDO      32

/* Types d'armes */
#define ARME_BASE          0
#define ARME_RAPIDE        1
#define ARME_DOUBLE        2

/* Types de bonus / malus */
#define BONUS_AUCUN        0
#define BONUS_ARME_RAPIDE  1
#define BONUS_ARME_DOUBLE  2
#define BONUS_BOUCLIER     3
#define MALUS_LENT         4

/* ---- Joueur ---- */
typedef struct {
    float x, y;             /* position dans la zone de jeu */
    int largeur, hauteur;   /* dimensions pour la hitbox */
    float vitesse;          /* vitesse de deplacement */
    int vivant;             /* 1 = en vie, 0 = mort */
    int arme;               /* arme actuellement equipee */
    int duree_arme;         /* temps restant avec l'arme bonus */
    int timer_tir;          /* delai entre deux tirs */
    int score;              /* score du joueur */
    int bouclier;           /* 1 = bouclier actif */
    int duree_bouclier;     /* temps restant du bouclier */
    int ralenti;            /* 1 = malus ralentissement actif */
    int duree_ralenti;
    char pseudo[TAILLE_PSEUDO];
} Joueur;

/* ---- Bulle (liste chainee) ---- */
typedef struct Bulle {
    float x, y;             /* position du centre */
    float vx, vy;           /* vitesse horizontale et verticale */
    int taille;             /* niveau de taille de 1 a 4 */
    int rayon;              /* rayon en pixels */
    int vivante;            /* 0 = a supprimer */
    int peut_eclair;        /* 1 = lance des eclairs (niveau 3+) */
    int timer_eclair;       /* delai avant le prochain eclair */
    int contient_bonus;     /* type de bonus libere a la destruction */
    struct Bulle *suivante;
} Bulle;

typedef struct {
    Bulle *tete;
    int nb;
} ListeBulles;

/* ---- Projectile (liste chainee) ---- */
typedef struct Projectile {
    float x, y;
    float vitesse;          /* vitesse de deplacement vers le haut */
    int actif;
    int type;
    struct Projectile *suivant;
} Projectile;

typedef struct {
    Projectile *tete;
    int nb;
} ListeProjectiles;

/* ---- Eclair (lance par certaines bulles a partir du niveau 3) ---- */
typedef struct Eclair {
    float x, y;
    float vitesse;
    int actif;
    struct Eclair *suivant;
} Eclair;

typedef struct {
    Eclair *tete;
    int nb;
} ListeEclairs;

/* ---- Bonus a ramasser au sol ---- */
typedef struct BonusObjet {
    float x, y;
    float vy;               /* tombe avec gravite */
    int type;               /* type de bonus */
    int actif;
    int au_sol;
    struct BonusObjet *suivant;
} BonusObjet;

typedef struct {
    BonusObjet *tete;
    int nb;
} ListeBonus;

/* ---- Boss ---- */
typedef struct {
    float x, y;
    int largeur, hauteur;
    float vx, vy;
    int pv;                 /* points de vie restants */
    int pv_max;
    int vivant;
    int timer_tir;          /* delai avant prochaine emission de bulle */
} Boss;

/* ---- Niveau ---- */
typedef struct {
    int numero;
    int temps_limite;
    int temps_restant;      /* en frames */
    ListeBulles liste_bulles;
    ListeProjectiles liste_projectiles;
    ListeEclairs liste_eclairs;
    ListeBonus liste_bonus;
    int eclairs_actifs;
    int has_boss;
    Boss boss;
} Niveau;

/* ---- Partie ---- */
typedef struct {
    Joueur *joueur;
    int niveau_courant;
    int nb_niveaux;
    int en_cours;
} Partie;

/* ---- Jeu ---- */
typedef struct {
    Partie *partie;
    Niveau *niveau;
    Joueur *joueur;
} Jeu;

/* ---- Commandes utilisateur ---- */
typedef struct {
    int gauche;
    int droite;
    int tir;
    int entree;
    int quitter;
} Commandes;

/* ---- Score (pour le tableau des meilleurs scores) ---- */
typedef struct {
    char pseudo[TAILLE_PSEUDO];
    int score;
    int niveau_atteint;
} ScoreEntry;

#endif /* TYPES_H */


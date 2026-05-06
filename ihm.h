#ifndef IHM_H
#define IHM_H

#include "types.h"

void LireClavier(Commandes *cmd);
Commandes recupCommd_joueur(void);
void SaisirPseudo(char *pseudo, int taille_max);
int ValiderChoix(int choix);

#endif /* IHM_H */

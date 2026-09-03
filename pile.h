
#ifndef PILE_H
#define PILE_H
#include "Vols.h"
#define TAILLE_MAX_HISTORIQUE 10
typedef enum { AJOUT_VOL, SUPPRESSION_VOL, DECOLLAGE } TypeAction;
typedef struct action {
    TypeAction type;
    Vol volConcerne;
    struct action *suiv;
} Action;
typedef struct {
    Action *sommet;
    int taille;
} PileHistorique;
void initPile(PileHistorique *p);
int estVidePile(PileHistorique p);
void empilerAction(PileHistorique *p, Action a);
Action depilerAction(PileHistorique *p);
void annulerDerniereAction(PileHistorique *p, Vol **registre);
void afficherHistorique(PileHistorique p);
#endif

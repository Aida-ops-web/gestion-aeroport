#include <stdio.h>
#include <stdlib.h>
#include "pile.h"


/* Initialise la pile d'historique */
void initPile(PileHistorique *p)
{
    /* Le sommet de la pile est vide */
    p->sommet = NULL;

    /* La pile contient zéro élément */
    p->taille = 0;
}


/* Vérifie si la pile est vide */
int estVidePile(PileHistorique p)
{
    /* Si le sommet est NULL, la pile est vide */
    if (p.sommet == NULL)
    {
        /* Retourne 1 : la pile est vide */
        return 1;
    }

    /* Retourne 0 : la pile n'est pas vide */
    return 0;
}


/* Ajoute une action au sommet de la pile */
void empilerAction(PileHistorique *p, Action a)
{
    /* Déclare un nouveau maillon */
    Action *nouvelleAction;

    /* Déclare un pointeur pour parcourir la pile */
    Action *courant;

    /* Réserve de la mémoire pour le nouveau maillon */
    nouvelleAction = (Action *)malloc(sizeof(Action));

    /* Vérifie si la mémoire a été réservée */
    if (nouvelleAction == NULL)
    {
        /* Affiche un message d'erreur */
        printf("Erreur : impossible d'allouer la memoire.\n");

        /* Arrête la fonction */
        return;
    }

    /* Copie l'action dans le nouveau maillon */
    nouvelleAction->type = a.type;

    /* Copie le vol concerné */
    nouvelleAction->volConcerne = a.volConcerne;

    /* Le nouveau maillon pointe vers l'ancien sommet */
    nouvelleAction->suiv = p->sommet;

    /* Le nouveau maillon devient le sommet */
    p->sommet = nouvelleAction;

    /* Augmente la taille de la pile */
    p->taille++;


    /* Vérifie si la pile dépasse la taille maximale */
    if (p->taille > TAILLE_MAX_HISTORIQUE)
    {
        /* Commence au sommet de la pile */
        courant = p->sommet;

        /* Cherche l'avant-dernier maillon */
        while (courant->suiv != NULL &&
               courant->suiv->suiv != NULL)
        {
            /* Avance vers le maillon suivant */
            courant = courant->suiv;
        }

        /* Vérifie qu'il existe bien un ancien maillon */
        if (courant->suiv != NULL)
        {
            /* Déclare un pointeur vers le dernier maillon */
            Action *ancien;

            /* Sauvegarde le dernier maillon */
            ancien = courant->suiv;

            /* L'avant-dernier devient le dernier */
            courant->suiv = NULL;

            /* Libère la mémoire de l'ancienne action */
            free(ancien);

            /* La taille revient à 10 */
            p->taille--;
        }
    }
}


/* Retire et retourne l'action au sommet */
Action depilerAction(PileHistorique *p)
{
    /* Déclare un pointeur vers le sommet */
    Action *ancien;

    /* Déclare l'action à retourner */
    Action a;


    /* Vérifie si la pile est vide */
    if (estVidePile(*p))
    {
        /* Affiche un message */
        printf("La pile d'historique est vide.\n");

        /* Initialise le type par défaut */
        a.type = AJOUT_VOL;

        /* Initialise le numéro du vol */
        a.volConcerne.numVol[0] = '\0';

        /* Initialise la destination */
        a.volConcerne.destination[0] = '\0';

        /* Initialise l'heure */
        a.volConcerne.heureDepart = 0;

        /* Initialise le nombre de places */
        a.volConcerne.nbPlaces = 0;

        /* Initialise les places occupées */
        a.volConcerne.placesOccupees = 0;

        /* Initialise le pointeur suivant */
        a.volConcerne.suiv = NULL;

        /* Initialise le pointeur précédent */
        a.volConcerne.pred = NULL;

        /* Initialise le pointeur de l'action */
        a.suiv = NULL;

        /* Retourne l'action vide */
        return a;
    }


    /* Sauvegarde le sommet actuel */
    ancien = p->sommet;

    /* Copie l'action qui se trouve au sommet */
    a.type = ancien->type;

    /* Copie le vol concerné */
    a.volConcerne = ancien->volConcerne;

    /* Le sommet devient l'action suivante */
    p->sommet = ancien->suiv;

    /* Diminue la taille de la pile */
    p->taille--;

    /* Libère la mémoire de l'ancien sommet */
    free(ancien);

    /* Retourne l'action retirée */
    return a;
}


/* Annule la dernière action */
void annulerDerniereAction(PileHistorique *p, Vol **registre)
{
    /* Déclare une variable pour recevoir l'action */
    Action a;


    /* Vérifie si la pile est vide */
    if (estVidePile(*p))
    {
        /* Affiche un message */
        printf("Aucune action a annuler.\n");

        /* Arrête la fonction */
        return;
    }


    /* Retire la dernière action de la pile */
    a = depilerAction(p);


    /* Vérifie si l'action était un ajout */
    if (a.type == AJOUT_VOL)
    {
        /* Supprime le vol qui avait été ajouté */
        supprimerVol(registre, a.volConcerne.numVol);

        /* Affiche un message */
        printf("Ajout du vol %s annule.\n",
               a.volConcerne.numVol);
    }


    /* Vérifie si l'action était une suppression */
    else if (a.type == SUPPRESSION_VOL)
    {
        /* Réinsère le vol supprimé */
        insererVolTrie(registre, a.volConcerne);

        /* Affiche un message */
        printf("Suppression du vol %s annulee.\n",
               a.volConcerne.numVol);
    }


    /* Vérifie si l'action était un décollage */
    else if (a.type == DECOLLAGE)
    {
        /* Réinsère le vol dans le registre */
        insererVolTrie(registre, a.volConcerne);

        /* Affiche un message */
        printf("Decollage du vol %s annule.\n",
               a.volConcerne.numVol);
    }
}


/* Affiche l'historique sans modifier la pile */
void afficherHistorique(PileHistorique p)
{
    /* Déclare un pointeur pour parcourir la pile */
    Action *courant;

    /* Déclare un compteur */
    int position = 1;


    /* Vérifie si la pile est vide */
    if (estVidePile(p))
    {
        /* Affiche un message */
        printf("\nL'historique est vide.\n");

        /* Arrête la fonction */
        return;
    }


    /* Affiche le titre */
    printf("\n===== HISTORIQUE =====\n");


    /* Commence au sommet de la pile */
    courant = p.sommet;


    /* Parcourt toutes les actions */
    while (courant != NULL)
    {
        /* Affiche la position */
        printf("\nAction %d : ", position);


        /* Vérifie le type de l'action */
        if (courant->type == AJOUT_VOL)
        {
            /* Affiche ajout */
            printf("AJOUT");
        }

        /* Vérifie si c'est une suppression */
        else if (courant->type == SUPPRESSION_VOL)
        {
            /* Affiche suppression */
            printf("SUPPRESSION");
        }

        /* Vérifie si c'est un décollage */
        else if (courant->type == DECOLLAGE)
        {
            /* Affiche décollage */
            printf("DECOLLAGE");
        }


        /* Affiche le numéro du vol concerné */
        printf(" - Vol : %s\n",
               courant->volConcerne.numVol);


        /* Passe à l'action suivante */
        courant = courant->suiv;


        /* Passe à la position suivante */
        position++;
    }


    /* Affiche la taille actuelle */
    printf("\nTaille de l'historique : %d\n",
           p.taille);

    /* Affiche la fin */
    printf("======================\n");
}

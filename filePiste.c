#include <stdio.h>
#include <stdlib.h>
#include "filePiste.h"

/* Initialise la file */
void initFile(FilePiste *f)
{
    /* La tête est vide */
    f->tete = NULL;

    /* La queue est vide */
    f->queue = NULL;

    /* La taille est zéro */
    f->taille = 0;
}


/* Vérifie si la file est vide */
int estVideFile(FilePiste f)
{
    /* Si la tête est NULL, la file est vide */
    if (f.tete == NULL)
    {
        /* Retourne 1 pour dire vrai */
        return 1;
    }

    /* Sinon la file n'est pas vide */
    return 0;
}


/* Ajoute un vol dans la file */
void enfiler(FilePiste *f, Vol v, int urgence)
{
    /* Déclare un nouveau maillon */
    MaillonFile *nouveau;

    /* Déclare un pointeur pour parcourir la file */
    MaillonFile *p;

    /* Réserve de la mémoire pour le nouveau maillon */
    nouveau = (MaillonFile *)malloc(sizeof(MaillonFile));

    /* Vérifie si la mémoire a bien été réservée */
    if (nouveau == NULL)
    {
        /* Affiche un message d'erreur */
        printf("Erreur : impossible d'allouer la memoire.\n");

        /* Arrête la fonction */
        return;
    }

    /* Copie le vol dans le nouveau maillon */
    nouveau->vol = v;

    /* Enregistre la priorité du vol */
    nouveau->urgence = urgence;

    /* Le nouveau maillon ne pointe encore vers personne */
    nouveau->suiv = NULL;

    /* Vérifie si la file est vide */
    if (estVideFile(*f))
    {
        /* Le nouveau maillon devient la tête */
        f->tete = nouveau;

        /* Le nouveau maillon devient aussi la queue */
        f->queue = nouveau;

        /* Augmente la taille de la file */
        f->taille++;

        /* Termine la fonction */
        return;
    }

    /* Vérifie si le vol est urgent */
    if (urgence == 1)
    {
        /* Vérifie si le premier vol est normal */
        if (f->tete->urgence == 0)
        {
            /* Le nouveau vol pointe vers l'ancienne tête */
            nouveau->suiv = f->tete;

            /* Le nouveau vol devient la nouvelle tête */
            f->tete = nouveau;

            /* Augmente la taille */
            f->taille++;

            /* Termine la fonction */
            return;
        }

        /* Commence le parcours à partir de la tête */
        p = f->tete;

        /* Cherche la dernière urgence */
        while (p->suiv != NULL && p->suiv->urgence == 1)
        {
            /* Avance vers le maillon suivant */
            p = p->suiv;
        }

        /* Le nouveau vol pointe vers le maillon suivant */
        nouveau->suiv = p->suiv;

        /* Le maillon précédent pointe vers le nouveau */
        p->suiv = nouveau;

        /* Vérifie si le nouveau vol est à la fin */
        if (nouveau->suiv == NULL)
        {
            /* Le nouveau vol devient la queue */
            f->queue = nouveau;
        }

        /* Augmente la taille */
        f->taille++;

        /* Termine la fonction */
        return;
    }

    /* Le vol est normal : il va à la fin */

    /* La queue actuelle pointe vers le nouveau vol */
    f->queue->suiv = nouveau;

    /* Le nouveau vol devient la queue */
    f->queue = nouveau;

    /* Augmente la taille */
    f->taille++;
}


/* Retire le premier vol de la file */
Vol defiler(FilePiste *f)
{
    /* Déclare un pointeur vers l'ancien premier maillon */
    MaillonFile *ancien;

    /* Déclare la variable qui contiendra le vol */
    Vol v;

    /* Vérifie si la file est vide */
    if (estVideFile(*f))
    {
        /* Affiche un message */
        printf("La file est vide.\n");

        /* Crée un vol vide */
        v.numVol[0] = '\0';

        /* Initialise la destination */
        v.destination[0] = '\0';

        /* Initialise l'heure */
        v.heureDepart = 0;

        /* Initialise le nombre de places */
        v.nbPlaces = 0;

        /* Initialise les places occupées */
        v.placesOccupees = 0;

        /* Initialise le pointeur suivant */
        v.suiv = NULL;

        /* Initialise le pointeur précédent */
        v.pred = NULL;

        /* Retourne le vol vide */
        return v;
    }

    /* Sauvegarde le premier maillon */
    ancien = f->tete;

    /* Sauvegarde le vol du premier maillon */
    v = ancien->vol;

    /* La deuxième position devient la nouvelle tête */
    f->tete = ancien->suiv;

    /* Vérifie si la file est maintenant vide */
    if (f->tete == NULL)
    {
        /* La queue devient aussi NULL */
        f->queue = NULL;
    }

    /* Diminue la taille de la file */
    f->taille--;

    /* Libère la mémoire de l'ancien maillon */
    free(ancien);

    /* Retourne le vol retiré */
    return v;
}


/* Consulte le premier vol sans le supprimer */
Vol consulterTeteFile(FilePiste f)
{
    /* Déclare un vol */
    Vol v;

    /* Vérifie si la file est vide */
    if (estVideFile(f))
    {
        /* Affiche un message */
        printf("La file est vide.\n");

        /* Crée un vol vide */
        v.numVol[0] = '\0';

        /* Initialise la destination */
        v.destination[0] = '\0';

        /* Initialise l'heure */
        v.heureDepart = 0;

        /* Initialise le nombre de places */
        v.nbPlaces = 0;

        /* Initialise les places */
        v.placesOccupees = 0;

        /* Initialise les pointeurs */
        v.suiv = NULL;
        v.pred = NULL;

        /* Retourne le vol vide */
        return v;
    }

    /* Retourne le premier vol sans le supprimer */
    return f.tete->vol;
}


/* Fait décoller le premier vol */
void decollerProchainVol(FilePiste *f)
{
    /* Déclare une variable pour recevoir le vol */
    Vol v;

    /* Vérifie si la file est vide */
    if (estVideFile(*f))
    {
        /* Affiche un message */
        printf("Aucun vol en attente de decollage.\n");

        /* Arrête la fonction */
        return;
    }

    /* Retire le premier vol */
    v = defiler(f);

    /* Affiche le titre */
    printf("\n===== DECOLLAGE =====\n");

    /* Affiche le numéro du vol */
    printf("Numero du vol : %s\n", v.numVol);

    /* Affiche la destination */
    printf("Destination   : %s\n", v.destination);

    /* Affiche l'heure de départ */
    printf("Heure depart  : %04d\n", v.heureDepart);

    /* Affiche la fin du message */
    printf("=====================\n");
}


/* Affiche tous les vols de la file */
void afficherFile(FilePiste f)
{
    /* Déclare un pointeur pour parcourir la file */
    MaillonFile *p;

    /* Commence la position à 1 */
    int position = 1;

    /* Vérifie si la file est vide */
    if (estVideFile(f))
    {
        /* Affiche un message */
        printf("\nLa file de piste est vide.\n");

        /* Arrête la fonction */
        return;
    }

    /* Affiche le titre */
    printf("\n===== FILE DE PISTE =====\n");

    /* Commence le parcours à la tête */
    p = f.tete;

    /* Parcourt tous les maillons */
    while (p != NULL)
    {
        /* Affiche la position */
        printf("\nPosition : %d\n", position);

        /* Affiche le numéro du vol */
        printf("Numero du vol : %s\n", p->vol.numVol);

        /* Affiche la destination */
        printf("Destination   : %s\n", p->vol.destination);

        /* Affiche l'heure de départ */
        printf("Heure depart  : %04d\n", p->vol.heureDepart);

        /* Affiche les places occupées et disponibles */
        printf("Places        : %d/%d\n",
               p->vol.placesOccupees,
               p->vol.nbPlaces);

        /* Vérifie si le vol est urgent */
        if (p->urgence == 1)
        {
            /* Affiche urgence */
            printf("Priorite      : URGENCE\n");
        }
        else
        {
            /* Affiche normal */
            printf("Priorite      : NORMAL\n");
        }

        /* Passe au maillon suivant */
        p = p->suiv;

        /* Passe à la position suivante */
        position++;
    }

    /* Affiche la taille totale */
    printf("\nTaille de la file : %d\n", f.taille);

    /* Affiche la fin */
    printf("=========================\n");
}

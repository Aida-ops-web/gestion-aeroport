#ifndef VOLS_H
#define VOLS_H
typedef struct vol {
    char numVol[8];        // ex : "AF1234"
    char destination[30];
    int heureDepart;       // format HHMM, ex : 1430 pour 14h30
    int nbPlaces;
    int placesOccupees;
    struct vol *suiv;
    struct vol *pred;
} Vol;
void insererVolTrie(Vol **tete, Vol v);
Vol *rechercherVol(Vol *tete, char *numVol);
void supprimerVol(Vol **tete, char *numVol);
void afficherChronologique(Vol *tete);
void afficherInverse(Vol *tete);
Vol *fusionnerRegistres(Vol *l1, Vol *l2);
void libererRegistre(Vol **tete);
#endif

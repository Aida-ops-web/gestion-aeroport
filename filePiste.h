
#ifndef FILEPISTE_H
#define FILEPISTE_H
#include "Vols.h"
typedef struct maillonFile {
    Vol vol;
    int urgence;
    struct maillonFile *suiv;
} MaillonFile;
typedef struct {
    MaillonFile *tete;
    MaillonFile *queue;
    int taille;
} FilePiste;
void initFile(FilePiste *f);
int estVideFile(FilePiste f);
void enfiler(FilePiste *f, Vol v, int urgence);
Vol defiler(FilePiste *f);
Vol consulterTeteFile(FilePiste f);
void decollerProchainVol(FilePiste *f);
void afficherFile(FilePiste f);
#endif

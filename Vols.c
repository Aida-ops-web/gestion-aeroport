#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vols.h"

/*
 * Module 1 : Registre des vols
 * Implémentation d'une liste doublement chaînée triée par ordre
 * croissant d'heure de départ (heureDepart).
 */

// Fonction interne (non exposée dans vols.h) : alloue dynamiquement
// un nouveau maillon de liste à partir d'une valeur Vol, et initialise
// ses pointeurs suiv/pred à NULL.
//static limite la portee de la fonction a vol.c
static Vol *creerMaillonVol(Vol v) {
    // Allocation dynamique de la taille d'un Vol.
    Vol *maillon = (Vol *) malloc(sizeof(Vol));
    if (maillon == NULL) {
        // Sécurité : arrêt du programme si l'allocation échoue.
        fprintf(stderr, "Erreur : allocation memoire echouee.\n");
        exit(EXIT_FAILURE);
    }
    // Copie du contenu de v (numVol, destination, heureDepart, ...) dans le maillon.
    *maillon = v;
    // Un maillon nouvellement créé n'est encore chaîné à personne.
    maillon->suiv = NULL;
    maillon->pred = NULL;
    return maillon;
}

// insererVolTrie : insère un nouveau vol à sa place dans la liste,
// triée par heure de départ croissante. Le numéro de vol doit être
// unique : si numVol existe déjà, l'insertion est rejetée.
void insererVolTrie(Vol **tete, Vol v) {
    // On refuse tout doublon de numéro de vol.
    if (rechercherVol(*tete, v.numVol) != NULL) {
        printf("Erreur : le vol %s existe deja dans le registre.\n", v.numVol);
        return;
    }

    // Création du nouveau maillon à insérer.
    Vol *maillon = creerMaillonVol(v);

    // Cas 1 : liste vide -> le nouveau maillon devient la tête.
    if (*tete == NULL) {
        *tete = maillon;
        return;
    }

    // Cas 2 : insertion en tête (le vol part avant le premier de la liste).
    if (v.heureDepart < (*tete)->heureDepart) {
        maillon->suiv = *tete;
        (*tete)->pred = maillon;
        *tete = maillon;
        return;
    }

    // Cas 3 : on cherche la position d'insertion en parcourant la liste
    // tant que l'élément courant a une heure de départ <= à la nouvelle.
    Vol *courant = *tete;
    while (courant->suiv != NULL && courant->suiv->heureDepart <= v.heureDepart) {
        courant = courant->suiv;
    }

    // Insertion du maillon juste après "courant".
    maillon->suiv = courant->suiv;
    maillon->pred = courant;
    if (courant->suiv != NULL) {
        // Il y avait un élément après : on relie son pred au nouveau maillon.
        courant->suiv->pred = maillon;
    }
    courant->suiv = maillon;
}

// rechercherVol : parcourt la liste à la recherche du vol dont le
// numéro correspond à numVol. Retourne un pointeur vers ce vol, ou
// NULL si aucun vol ne correspond.
Vol *rechercherVol(Vol *tete, char *numVol) {
    Vol *courant = tete;
    // On avance tant qu'on n'a pas trouvé le numéro recherché.
    while (courant != NULL && strcmp(courant->numVol, numVol) != 0) {
        courant = courant->suiv;
    }
    // Si courant == NULL, le vol n'a pas été trouvé.
    return courant;
}

// supprimerVol : retire de la liste le vol dont le numéro est numVol,
// en recâblant correctement les pointeurs pred/suiv des voisins.
void supprimerVol(Vol **tete, char *numVol) {
    // On localise d'abord le maillon à supprimer.
    Vol *cible = rechercherVol(*tete, numVol);

    if (cible == NULL) {
        printf("Erreur : le vol %s est introuvable.\n", numVol);
        return;
    }

    // Si le maillon a un prédécesseur, on le rebranche vers le successeur.
    if (cible->pred != NULL) {
        cible->pred->suiv = cible->suiv;
    } else {
        // Sinon, cible était la tête : la nouvelle tête devient son successeur.
        *tete = cible->suiv;
    }

    // Si le maillon a un successeur, on le rebranche vers le prédécesseur.
    if (cible->suiv != NULL) {
        cible->suiv->pred = cible->pred;
    }

    // Libération de la mémoire occupée par le maillon supprimé.
    free(cible);
}

// afficherChronologique : affiche les vols du plus tôt au plus tard,
// en parcourant simplement la liste vers l'avant (via suiv).
void afficherChronologique(Vol *tete) {
    Vol *courant = tete;
    printf("--- Registre des vols (chronologique) ---\n");
    while (courant != NULL) {
        printf("%s | %-20s | %04d | %d/%d places\n",
               courant->numVol, courant->destination,
               courant->heureDepart, courant->placesOccupees, courant->nbPlaces);
        courant = courant->suiv;
    }
}

// afficherInverse : affiche les vols du plus tard au plus tôt, en
// exploitant le pointeur pred (pas de recopie ni de tableau auxiliaire).
void afficherInverse(Vol *tete) {
    if (tete == NULL) {
        printf("--- Registre des vols (inverse) ---\n(registre vide)\n");
        return;
    }

    // On se place d'abord sur le dernier maillon de la liste.
    Vol *courant = tete;
    while (courant->suiv != NULL) {
        courant = courant->suiv;
    }

    // Puis on remonte vers la tête grâce au pointeur pred.
    printf("--- Registre des vols (inverse) ---\n");
    while (courant != NULL) {
        printf("%s | %-20s | %04d | %d/%d places\n",
               courant->numVol, courant->destination,
               courant->heureDepart, courant->placesOccupees, courant->nbPlaces);
        courant = courant->pred;
    }
}

// fusionnerRegistres : fusionne deux registres déjà triés (l1 et l2)
// en un seul registre trié, sans dupliquer un numéro de vol présent
// dans les deux. En cas de conflit, le vol de l1 est conservé.
Vol *fusionnerRegistres(Vol *l1, Vol *l2) {
    Vol *resultat = NULL;

    // On réinsère tous les vols de l1 un par un (insertion triée,
    // qui garantit l'ordre et l'unicité par construction).
    Vol *courant = l1;
    while (courant != NULL) {
        insererVolTrie(&resultat, *courant);
        courant = courant->suiv;
    }

    // Puis tous les vols de l2 : en cas de doublon, insererVolTrie
    // refuse déjà l'insertion, donc le vol de l1 est conservé.
    courant = l2;
    while (courant != NULL) {
        if (rechercherVol(resultat, courant->numVol) != NULL) {
            // Le numéro existe déjà (venant de l1) : on signale le doublon.
            printf("Doublon detecte lors de la fusion : vol %s ignore (conserve depuis le premier registre).\n",
                   courant->numVol);
        } else {
            insererVolTrie(&resultat, *courant);
        }
        courant = courant->suiv;
    }

    return resultat;
}

// libererRegistre : parcourt la liste et libère chaque maillon, puis
// remet la tête à NULL pour éviter tout pointeur pendant (dangling).
void libererRegistre(Vol **tete) {
    Vol *courant = *tete;
    while (courant != NULL) {
        // On mémorise le suivant avant de libérer le maillon courant,
        // sinon on perd l'accès au reste de la liste.
        Vol *suivant = courant->suiv;
        free(courant);
        courant = suivant;
    }
    // La liste est vide désormais : la tête ne pointe plus sur rien.
    *tete = NULL;
}

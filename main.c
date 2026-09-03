#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Vols.h"
#include "filePiste.h"
#include "pile.h"


/*
============================================================
                FONCTION lireEntier
============================================================

Fonction utilitaire de saisie robuste.

Elle demande un entier a l'utilisateur, et recommence
tant que la saisie n'est pas un nombre valide compris
entre min et max.

C'est cette fonction qui garantit qu'aucune saisie
invalide ne peut faire planter ou bloquer le programme.
*/
int lireEntier(char *message, int min, int max)
{
    /* Variable qui recevra la valeur saisie */
    int valeur;

    /* Variable qui indique si scanf a reussi */
    int resultat;


    /* Boucle jusqu'a obtenir une saisie valide */
    while (1)
    {
        /* Affiche le message demande */
        printf("%s", message);

        /* Tente de lire un entier */
        resultat = scanf("%d", &valeur);


        /*
        Si scanf n'a pas lu un entier (l'utilisateur a
        tape une lettre par exemple), le flux d'entree
        contient encore le caractere fautif : il faut
        le vider, sinon on boucle indefiniment sur le
        meme caractere.
        */
        if (resultat != 1)
        {
            /* Vide le buffer jusqu'a la fin de la ligne */
            while (getchar() != '\n')
                ;

            printf("Erreur : veuillez saisir un nombre entier.\n");

            /* Redemande la saisie */
            continue;
        }


        /* Verifie que la valeur est dans l'intervalle autorise */
        if (valeur < min || valeur > max)
        {
            printf("Erreur : la valeur doit etre comprise entre %d et %d.\n",
                   min, max);

            /* Redemande la saisie */
            continue;
        }


        /* La saisie est valide : on sort de la boucle */
        break;
    }

    return valeur;
}


/*
============================================================
                    FONCTION afficherMenu
============================================================
*/
void afficherMenu()
{
    printf("\n");
    printf("==================================================\n");
    printf("              GESTION AEROPORT\n");
    printf("==================================================\n");

    printf("1.  Ajouter un vol\n");
    printf("2.  Rechercher un vol\n");
    printf("3.  Supprimer un vol\n");
    printf("4.  Afficher les vols chronologiquement\n");
    printf("5.  Afficher les vols dans l'ordre inverse\n");

    printf("------------------------------------------\n");

    printf("6.  Ajouter un vol dans la file de piste\n");
    printf("7.  Consulter le prochain vol\n");
    printf("8.  Afficher la file de piste\n");
    printf("9.  Faire decoller le prochain vol\n");

    printf("------------------------------------------\n");

    printf("10. Afficher l'historique\n");
    printf("11. Annuler la derniere action\n");

    printf("------------------------------------------\n");

    printf("12. Fusionner deux registres\n");

    printf("------------------------------------------\n");

    printf("13. Charger un jeu de donnees de demonstration\n");

    printf("------------------------------------------\n");

    printf("0.  Quitter\n");

    printf("==================================================\n");
}


/*
============================================================
                    FONCTION saisirVol
============================================================

Demande les informations d'un vol a l'utilisateur, avec
des restrictions de saisie sur chaque champ :

- numVol       : ne doit pas etre vide
- destination  : ne doit pas etre vide
- heureDepart  : doit etre un entier entre 0000 et 2359
- nbPlaces     : doit etre strictement positif
- placesOccupees : doit etre compris entre 0 et nbPlaces
                    (on ne peut pas avoir plus de passagers
                    que de places disponibles)
*/
Vol saisirVol()
{
    Vol v;


    /* --- Saisie du numero de vol (non vide) --- */
    do
    {
        printf("\nNumero du vol : ");
        scanf("%7s", v.numVol);

        if (strlen(v.numVol) == 0)
        {
            printf("Erreur : le numero de vol ne peut pas etre vide.\n");
        }
    }
    while (strlen(v.numVol) == 0);


    /* --- Saisie de la destination (non vide) --- */
    do
    {
        printf("Destination : ");

        /* Vide le caractere '\n' laisse par le scanf precedent */
        getchar();

        fgets(v.destination, sizeof(v.destination), stdin);

        /* Supprime le '\n' ajoute par fgets */
        v.destination[strcspn(v.destination, "\n")] = '\0';

        if (strlen(v.destination) == 0)
        {
            printf("Erreur : la destination ne peut pas etre vide.\n");
        }
    }
    while (strlen(v.destination) == 0);


    /*
    --- Saisie de l'heure de depart ---
    Format HHMM attendu, donc une valeur entiere entre 0000 et 2359.
    Remarque : un format HHMM correct a en plus des minutes < 60,
    mais l'intervalle 0-2359 filtre deja la grande majorite
    des erreurs de saisie (heure > 23, valeur negative, etc.).
    */
    v.heureDepart = lireEntier("Heure de depart (HHMM, entre 0000 et 2359) : ", 0, 2359);


    /* --- Saisie du nombre total de places (strictement positif) --- */
    v.nbPlaces = lireEntier("Nombre total de places (min 1) : ", 1, 10000);


    /*
    --- Saisie des places occupees ---
    Ne doit pas depasser le nombre total de places : on utilise
    directement nbPlaces comme borne maximale de lireEntier.
    */
    v.placesOccupees = lireEntier("Nombre de places occupees : ", 0, v.nbPlaces);


    /*
    Lorsqu'on cree un nouveau vol, ses pointeurs doivent etre
    initialises a NULL. Ils seront ensuite geres par la liste
    doublement chainee.
    */
    v.suiv = NULL;
    v.pred = NULL;

    return v;
}


/*
============================================================
                FONCTION ajouterVol
============================================================
*/
void ajouterVol(Vol **registre, PileHistorique *historique)
{
    Vol v;
    Action action;

    v = saisirVol();

    if (rechercherVol(*registre, v.numVol) != NULL)
    {
        printf("\nErreur : ce vol existe deja.\n");
        return;
    }

    insererVolTrie(registre, v);

    action.type = AJOUT_VOL;
    action.volConcerne = v;
    action.suiv = NULL;

    empilerAction(historique, action);

    printf("\nVol %s ajoute avec succes.\n", v.numVol);
}


/*
============================================================
                FONCTION rechercherUnVol
============================================================
*/
void rechercherUnVol(Vol *registre)
{
    char numVol[8];
    Vol *v;

    printf("\nNumero du vol a rechercher : ");
    scanf("%7s", numVol);

    v = rechercherVol(registre, numVol);

    if (v == NULL)
    {
        printf("\nVol %s introuvable.\n", numVol);
        return;
    }

    printf("\n===== VOL TROUVE =====\n");
    printf("Numero       : %s\n", v->numVol);
    printf("Destination  : %s\n", v->destination);
    printf("Heure depart : %04d\n", v->heureDepart);
    printf("Places       : %d/%d\n", v->placesOccupees, v->nbPlaces);
    printf("======================\n");
}


/*
============================================================
                FONCTION supprimerUnVol
============================================================
*/
void supprimerUnVol(Vol **registre, PileHistorique *historique)
{
    char numVol[8];
    Vol *v;
    Vol sauvegarde;
    Action action;

    printf("\nNumero du vol a supprimer : ");
    scanf("%7s", numVol);

    v = rechercherVol(*registre, numVol);

    if (v == NULL)
    {
        printf("\nVol introuvable.\n");
        return;
    }

    sauvegarde = *v;

    supprimerVol(registre, numVol);

    action.type = SUPPRESSION_VOL;
    action.volConcerne = sauvegarde;
    action.suiv = NULL;

    empilerAction(historique, action);

    printf("\nVol %s supprime avec succes.\n", numVol);
}


/*
============================================================
            FONCTION ajouterVolFile
============================================================
*/
void ajouterVolFile(Vol *registre, FilePiste *file)
{
    char numVol[8];
    Vol *v;
    int urgence;

    printf("\nNumero du vol a placer dans la file : ");
    scanf("%7s", numVol);

    v = rechercherVol(registre, numVol);

    if (v == NULL)
    {
        printf("\nErreur : vol introuvable dans le registre.\n");
        return;
    }

    printf("\nType de vol :\n");
    printf("0 - Normal\n");
    printf("1 - Urgence\n");

    /*
    lireEntier garantit que urgence vaut forcement 0 ou 1 :
    plus besoin de verification supplementaire apres l'appel.
    */
    urgence = lireEntier("Votre choix : ", 0, 1);

    enfiler(file, *v, urgence);

    printf("\nVol %s ajoute dans la file de piste.\n", v->numVol);
}


/*
============================================================
            FONCTION consulterProchainVol
============================================================
*/
void consulterProchainVol(FilePiste file)
{
    Vol v;

    if (estVideFile(file))
    {
        printf("\nLa file de piste est vide.\n");
        return;
    }

    v = consulterTeteFile(file);

    printf("\n===== PROCHAIN VOL =====\n");
    printf("Numero       : %s\n", v.numVol);
    printf("Destination  : %s\n", v.destination);
    printf("Heure depart : %04d\n", v.heureDepart);
    printf("Places       : %d/%d\n", v.placesOccupees, v.nbPlaces);
    printf("========================\n");
}


/*
============================================================
            FONCTION decollerVol
============================================================
*/
void decollerVol(FilePiste *file, PileHistorique *historique)
{
    Vol v;
    Action action;

    if (estVideFile(*file))
    {
        printf("\nAucun vol dans la file.\n");
        return;
    }

    v = defiler(file);

    action.type = DECOLLAGE;
    action.volConcerne = v;
    action.suiv = NULL;

    empilerAction(historique, action);

    printf("\nLe vol %s vient de decoller.\n", v.numVol);
}


/*
============================================================
                FONCTION fusionner
============================================================
*/
void fusionner(Vol **registre)
{
    Vol *registre2 = NULL;
    Vol *resultat = NULL;
    Vol v1;
    Vol v2;

    strcpy(v1.numVol, "AF12");
    strcpy(v1.destination, "MALI");
    v1.heureDepart = 1800;
    v1.nbPlaces = 200;
    v1.placesOccupees = 100;
    v1.suiv = NULL;
    v1.pred = NULL;

    strcpy(v2.numVol, "AF13");
    strcpy(v2.destination, "Saint-Louis");
    v2.heureDepart = 2000;
    v2.nbPlaces = 150;
    v2.placesOccupees = 80;
    v2.suiv = NULL;
    v2.pred = NULL;

    insererVolTrie(&registre2, v1);
    insererVolTrie(&registre2, v2);

    resultat = fusionnerRegistres(*registre, registre2);

    libererRegistre(registre);

    *registre = resultat;

    libererRegistre(&registre2);

    printf("\n===== REGISTRE APRES FUSION =====\n");
    afficherChronologique(*registre);
}


/*
============================================================
        FONCTION chargerDonneesDemonstration
============================================================

Cette fonction remplit automatiquement le registre, la file
de piste et l'historique avec un jeu de donnees varie, afin
de pouvoir tester rapidement toutes les fonctionnalites du
programme sans tout saisir manuellement.

Le jeu de donnees couvre volontairement plusieurs cas :
- des vols a des heures differentes (pour verifier le tri)
- un vol complet (placesOccupees == nbPlaces)
- une tentative d'ajout d'un numero de vol en double
  (pour verifier le rejet)
- un vol enfile normalement, puis un vol urgent enfile
  APRES lui (pour verifier qu'il passe bien devant)
- un decollage (pour verifier la file et l'historique)
- une serie d'operations supplementaires pour depasser 10
  actions dans l'historique (pour verifier le plafonnement,
  voir plus bas)
*/
void chargerDonneesDemonstration(Vol **registre, FilePiste *file,
                                  PileHistorique *historique)
{
    /* Variables pour les vols de demonstration */
    Vol v1, v2, v3, v4, v5, v6;
    Action action;
    int i;


    /* --- Vol 1 : un vol classique en milieu de journee --- */
    strcpy(v1.numVol, "AF1234");
    strcpy(v1.destination, "Paris");
    v1.heureDepart = 1430;
    v1.nbPlaces = 180;
    v1.placesOccupees = 120;
    v1.suiv = NULL;
    v1.pred = NULL;


    /* --- Vol 2 : un vol tot le matin (doit se retrouver en tete) --- */
    strcpy(v2.numVol, "SN2020");
    strcpy(v2.destination, "Dakar");
    v2.heureDepart = 900;
    v2.nbPlaces = 90;
    v2.placesOccupees = 90;   // vol complet : cas limite
    v2.suiv = NULL;
    v2.pred = NULL;


    /* --- Vol 3 : un vol en fin de journee --- */
    strcpy(v3.numVol, "TK300");
    strcpy(v3.destination, "Istanbul");
    v3.heureDepart = 1815;
    v3.nbPlaces = 200;
    v3.placesOccupees = 199;
    v3.suiv = NULL;
    v3.pred = NULL;


    /* --- Vol 4 : un vol en milieu d'apres-midi --- */
    strcpy(v4.numVol, "RAM10");
    strcpy(v4.destination, "Casablanca");
    v4.heureDepart = 1200;
    v4.nbPlaces = 150;
    v4.placesOccupees = 60;
    v4.suiv = NULL;
    v4.pred = NULL;


    /* --- Vol 5 : servira a demontrer le decollage normal --- */
    strcpy(v5.numVol, "UGB01");
    strcpy(v5.destination, "Saint-Louis");
    v5.heureDepart = 800;
    v5.nbPlaces = 50;
    v5.placesOccupees = 40;
    v5.suiv = NULL;
    v5.pred = NULL;


    /* --- Vol 6 : servira a demontrer l'urgence dans la file --- */
    strcpy(v6.numVol, "UGB02");
    strcpy(v6.destination, "Ziguinchor");
    v6.heureDepart = 1000;
    v6.nbPlaces = 50;
    v6.placesOccupees = 10;
    v6.suiv = NULL;
    v6.pred = NULL;


    printf("\n===== Chargement du jeu de donnees de demonstration =====\n");


    /*
    Insertion des 6 vols dans le registre (ordre volontairement
    melange pour verifier que insererVolTrie trie correctement).
    Chaque insertion reelle est desormais aussi empilee dans
    l'historique en tant qu'action AJOUT_VOL, exactement comme
    le ferait ajouterVol() depuis le menu : cela porte le total
    a 6 actions apres cette etape.
    */
    insererVolTrie(registre, v1);
    action.type = AJOUT_VOL;
    action.volConcerne = v1;
    action.suiv = NULL;
    empilerAction(historique, action);

    insererVolTrie(registre, v2);
    action.type = AJOUT_VOL;
    action.volConcerne = v2;
    action.suiv = NULL;
    empilerAction(historique, action);

    insererVolTrie(registre, v3);
    action.type = AJOUT_VOL;
    action.volConcerne = v3;
    action.suiv = NULL;
    empilerAction(historique, action);

    insererVolTrie(registre, v4);
    action.type = AJOUT_VOL;
    action.volConcerne = v4;
    action.suiv = NULL;
    empilerAction(historique, action);

    insererVolTrie(registre, v5);
    action.type = AJOUT_VOL;
    action.volConcerne = v5;
    action.suiv = NULL;
    empilerAction(historique, action);

    insererVolTrie(registre, v6);
    action.type = AJOUT_VOL;
    action.volConcerne = v6;
    action.suiv = NULL;
    empilerAction(historique, action);


    /* Tentative volontaire d'un doublon : doit etre rejetee par
       insererVolTrie avec un message d'erreur (numero AF1234 deja pris).
       Comme l'insertion est refusee, aucune action n'est empilee ici. */
    printf("\n--- Test du rejet de doublon (attendu : erreur) ---\n");
    insererVolTrie(registre, v1);


    /* On enfile d'abord UGB01 normalement, puis UGB02 en urgence :
       UGB02 doit passer devant UGB01 dans la file, ce qui demontre
       la gestion de la priorite */
    printf("\n--- Constitution de la file de piste (test priorite) ---\n");
    enfiler(file, v5, 0);   // UGB01, normal
    enfiler(file, v6, 1);   // UGB02, urgence -> doit passer devant


    /* On fait decoller le premier vol de la file : doit etre UGB02
       (l'urgence), et l'action est empilee dans l'historique.
       Total apres cette etape : 7 actions. */
    action.type = DECOLLAGE;
    action.volConcerne = defiler(file);
    action.suiv = NULL;
    empilerAction(historique, action);


    /*
    On simule ensuite 3 allers-retours suppression/reinsertion sur
    le vol TK300 (v3), afin de depasser les 10 actions et prouver
    que la pile d'historique plafonne bien a TAILLE_MAX_HISTORIQUE :
    7 actions deja empilees + 3 x (1 suppression + 1 ajout) = 13 actions
    empilees au total, mais empilerAction() ecarte silencieusement
    le fond de la pile des que taille > TAILLE_MAX_HISTORIQUE, donc
    seules les 10 dernieres actions doivent rester visibles.
    */
    printf("\n--- Simulation d'operations supplementaires (test plafond historique) ---\n");
    for (i = 0; i < 3; i++)
    {
        /* Suppression du vol TK300 : on empile une action SUPPRESSION_VOL */
        supprimerVol(registre, v3.numVol);
        action.type = SUPPRESSION_VOL;
        action.volConcerne = v3;
        action.suiv = NULL;
        empilerAction(historique, action);

        /* Reinsertion du meme vol : on empile une action AJOUT_VOL */
        insererVolTrie(registre, v3);
        action.type = AJOUT_VOL;
        action.volConcerne = v3;
        action.suiv = NULL;
        empilerAction(historique, action);
    }

    /* Affiche le resultat du test de plafonnement pour verification
       immediate a l'ecran (sans avoir a passer par le menu). */
    printf("Nombre total d'actions empilees durant la demo : 13\n");
    printf("Taille actuelle de l'historique (doit etre plafonnee a %d) : %d\n",
           TAILLE_MAX_HISTORIQUE, historique->taille);


    printf("\nJeu de donnees charge avec succes.\n");
    printf("Astuce : utilisez les options 4, 8 et 10 du menu pour\n");
    printf("observer le registre, la file de piste et l'historique.\n");
}


/*
============================================================
                        MAIN
============================================================
*/
int main()
{
    Vol *registre = NULL;
    FilePiste file;
    PileHistorique historique;
    int choix;

    initFile(&file);
    initPile(&historique);

    do
    {
        afficherMenu();

        /*
        La saisie du choix passe desormais par lireEntier,
        qui gere elle-meme les caracteres non numeriques et
        vide le buffer en cas d'erreur : c'est ici que la
        robustesse du menu est assuree (aucune boucle infinie
        possible, aucun plantage).
        */
        choix = lireEntier("Votre choix : ", 0, 13);

        switch (choix)
        {
            case 1:
                ajouterVol(&registre, &historique);
                break;

            case 2:
                rechercherUnVol(registre);
                break;

            case 3:
                supprimerUnVol(&registre, &historique);
                break;

            case 4:
                afficherChronologique(registre);
                break;

            case 5:
                afficherInverse(registre);
                break;

            case 6:
                ajouterVolFile(registre, &file);
                break;

            case 7:
                consulterProchainVol(file);
                break;

            case 8:
                afficherFile(file);
                break;

            case 9:
                decollerVol(&file, &historique);
                break;

            case 10:
                afficherHistorique(historique);
                break;

            case 11:
                annulerDerniereAction(&historique, &registre);
                break;

            case 12:
                fusionner(&registre);
                break;

            case 13:
                chargerDonneesDemonstration(&registre, &file, &historique);
                break;

            case 0:
                printf("\nFermeture du programme...\n");
                break;
        }

    }
    while (choix != 0);

    libererRegistre(&registre);

    return 0;
}

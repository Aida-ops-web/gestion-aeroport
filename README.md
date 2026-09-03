# Système de Gestion d'un Aéroport

Projet réalisé dans le cadre du module **Algorithmique et Programmation 4**  
**Université Gaston Berger de Saint-Louis (UGB)** — *UFR des Sciences Appliquées et de Technologie (SAT)* / *Centre de Formation Professionnelle Pluridisciplinaire (CFPP)*  
**Formation :** Licence Professionnelle Génie Informatique (LPGI)  
**Année Académique :** 2025/2026  

---

##  Présentation du Projet

Ce projet met en œuvre, de manière intégrée et modulaire, les trois structures de données linéaires fondamentales étudiées en cours :
1. **Liste doublement chaînée triée** : Gestion du registre des vols (trié par heure de départ croissante, parcours bidirectionnel).
2. **File FIFO avec gestion de priorité** : File d'attente de la piste de décollage (insertion prioritaire des vols urgents).
3. **Pile LIFO bornée (10 actions max)** : Historique des opérations et fonctionnalité d'annulation (`undo`).

L'objectif est d'assurer une encapsulation stricte des Types Abstraits de Données (TAD) et une gestion mémoire rigoureuse sans fuite mémoire.

---

## Structure du Dépôt et Modularité

Le code source est structuré en modules indépendants (`.h` / `.c`) conformément aux exigences du sujet :

```text
.
├── main.c           # Module 4 : Interface utilisateur, menu interactif et orchestration
├── vols.h / vols.c  # Module 1 : Registre des vols (Liste doublement chaînée triée)
├── filePiste.h / .c # Module 2 : File d'attente de la piste (FIFO + Priorité)
├── pile.h / pile.c  # Module 3 : Historique des opérations (Pile LIFO bornée)
├── Makefile         # Script d'automatisation de la compilation
└── README.md        # Fichier de documentation du projet
```

---

##  Prérequis & Environnement

- **Langage :** C (Standard C99 / C11)
- **Compilateur :** `gcc`
- **Outil de build :** `make`
- **Environnement testé :** Linux (Ubuntu/Debian), Windows (MinGW / Code::Blocks / Dev-C++)
- **Analyseur mémoire :** `valgrind` (recommandé sous Linux)

---

## Compilation et Exécution

### 1. Utilisation du `Makefile` (Recommandé)

Pour compiler l'intégralité du projet avec les options de rigueur `-Wall -Wextra` :
```bash
make
```

Pour lancer le programme :
```bash
./aeroport
```

Pour nettoyer les fichiers objets (`.o`) et l'exécutable généré :
```bash
make clean
```

### 2. Compilation manuelle avec `gcc`

Si l'outil `make` n'est pas installé sur votre environnement :
```bash
gcc -Wall -Wextra -std=c99 -o aeroport main.c vols.c filePiste.c pile.c
./aeroport
```

---

## Guide d'Utilisation & Menu Interactif

Au lancement, l'application présente un menu interactif permettant de gérer l'aéroport :

```text
===== Gestion Aéroport =====
1. Ajouter un vol au registre
2. Rechercher un vol
3. Supprimer un vol
4. Afficher le registre (chronologique / inverse)
5. Enfiler un vol prêt au décollage (normal / urgence)
6. Faire décoller le prochain vol
7. Afficher la file d'attente piste
8. Annuler la dernière action (Undo)
9. Afficher l'historique
13. Charger le jeu de données de démonstration
0. Quitter
```

### Démonstration Rapide (Option 13)
L'option **13** charge automatiquement un jeu de tests complet permettant d'évaluer immédiatement :
- Le tri automatique du registre par heure de départ.
- Le passage prioritaire des vols urgents en tête de file de piste.
- Le plafonnement strict de la pile d'historique à 10 actions.
- La restauration de l'état du registre lors des annulations.

---

##  Choix de Conception Technique

### 1. File de Piste à Priorité (Module 2)
Lors de l'enfilage d'un vol urgent (`urgence = 1`), le maillon est inséré immédiatement après le dernier vol urgent déjà présent dans la file (ou en tête de file si aucun vol urgent ne s'y trouve). Cette solution conserve une structure unique `FilePiste`, garantissant une encapsulation parfaite tout en respectant l'ordre FIFO entre vols de même niveau d'urgence.

### 2. Pile d'Historique Bornée à 10 Éléments (Module 3)
Afin de ne pas dépasser 10 actions dans l'historique (`TAILLE_MAX_HISTORIQUE = 10`), la fonction `empilerAction` vérifie la taille après chaque empilage. Si la limite est dépassée, le maillon le plus ancien (au fond de la pile) est détaché et libéré en mémoire. Cette opération est réalisée en interne dans `pile.c` pour ne jamais exposer la structure aux modules extérieurs.

---

##  Vérification de la Gestion Mémoire (Valgrind)

Toute allocation dynamique (`malloc`) est appariée à sa libération (`free`). La mémoire du registre, de la file et de la pile est entièrement libérée à la fermeture du programme (option 0).

Pour vérifier l'absence de fuites mémoire sous Linux :
```bash
valgrind --leak-check=full --show-leak-kinds=all ./aeroport
```

---

## Auteurs & Encadrement

- **Étudiantes :** Aida Diop & Fathiam Gaye  
- **Formation :** Licence Professionnelle Génie Informatique (LPGI)  
- **Encadrant :** Monsieur Dieng  
- **Établissement :** Université Gaston Berger de Saint-Louis (UGB) — CFPP / UFR SAT  

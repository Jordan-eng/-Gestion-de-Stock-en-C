# 📦 Gestion de Stock en C

## 📖 Description

Ce projet est une application écrite en langage C conçue pour gérer un stock de produits organisés en catégories. L'application propose une interface utilisateur textuelle dotée de menus interactifs permettant d'effectuer diverses opérations de gestion sur les catégories et les produits.

## ✨ Fonctionnalités Principales

L'application est divisée en plusieurs modules fonctionnels :

### 🗂️ Gestion des Catégories

**Création :** Saisie et ajout d'une nouvelle catégorie à la liste.
 
**Affichage :** Visualisation de toutes les catégories existantes avec leurs produits associés.
 
**Suppression :** Retrait d'une catégorie et libération de la mémoire associée.
 
**Modification :** Possibilité de renommer une catégorie existante.


### 🛒 Gestion des Produits
 
**Ajout :** Insertion d'un produit dans une catégorie spécifique.
 
**Mise à jour :** Modification du prix et de la quantité d'un produit existant.
 
**Suppression :** Retrait d'un produit d'une catégorie.
 
**Affichage :** Liste des produits pour une catégorie donnée.


* **Tri :**
* Tri des produits d'une catégorie par nom, prix (croissant/décroissant) ou quantité.

* Tri global de tous les produits de toutes les catégories selon un critère défini.


### 📊 Analyse et Statistiques

**Alertes :** Identification et affichage des produits dont le stock est inférieur à un seuil défini.
 
**Recherche :** Localisation d'un produit par son nom à travers toutes les catégories.
 
**Statistiques Globales :** Affichage du nombre total de produits et de la valeur totale du stock.
 
**Calcul de stock :** Calcul du nombre total de produits au sein d'une catégorie.


### 💾 Persistance des Données (Fichiers)
 
**Sauvegarde :** Exportation des catégories et produits vers un fichier CSV.
 
**Chargement :** Importation des données depuis un fichier CSV.
 
**Visualisation :** Affichage du contenu brut du fichier CSV.


## 🏗️ Structures de Données

Le projet utilise des listes chaînées pour gérer dynamiquement les données.
 
**Catégorie (`Categorie`) :** Contient un titre, une liste chaînée de produits et un pointeur vers la catégorie suivante.
 
**Produit (`Produit`) :** Contient un nom, un prix, une quantité et un pointeur vers le produit suivant.


## 🛡️ Contrôles, Robustesse et Interface

### Interface Utilisateur

L'interface utilise un code couleur pour améliorer la lisibilité :

<img width="528" height="808" alt="image" src="https://github.com/user-attachments/assets/469f0343-b405-4ca1-bbbf-a0d55e8d4e91" />


* 🔵 **Bleu :** Titres et bordures.


* 🔴 **Rouge :** Messages d'erreur.


* 🟢 **Vert :** Messages de confirmation.


* 🟡 **Jaune :** Séparateurs et alertes.



### Gestion des Erreurs et Validation

* Validation des entrées numériques (prix et quantités doivent être positifs).


* Vérification des choix de menu pour qu'ils soient dans les plages valides.


* Nettoyage des chaînes de caractères (suppression des retours à la ligne indésirables).


* Gestion des échecs d'allocation mémoire et des fichiers inaccessibles.



### Gestion de la Mémoire

Le programme assure la libération correcte des listes chaînées (catégories et produits) à la fin de l'exécution pour prévenir les fuites de mémoire.


## 🚀 Pistes d'Amélioration

* Ajout de tests unitaires pour valider les fonctions principales.

* Centralisation des messages d'erreur et de confirmation via une fonction dédiée.


* Optimisation de l'utilisation mémoire lors des tris.

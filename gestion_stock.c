#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <conio.h> // Ajout pour utiliser _getch()
#include <ctype.h> // Ajout pour utiliser tolower()

#define MAX_NAME_LENGTH 256
#define MAX_MENU_OPTION 17       // Nombre maximum d'options dans le menu
#define SEUIL_STOCK_FAIBLE 10    // Seuil pour l'alerte de stock faible
#define SEUIL_ALERTE 70           // Seuil global pour les alertes de stock

#define RES         "\033[0m"
#define ROUGE       "\033[31m"
#define VERT        "\033[32m"
#define BLEU        "\033[34m"
#define GRAS        "\033[1m" 
#define inverse     "\033[36m"
#define cyan        "\033[36m"
#define JAUNE       "\033[33m"

#define FILENAME "produits.csv"

// Structure pour representer un produit
typedef struct Produit {
    char name[MAX_NAME_LENGTH];
    float price;
    int quantities;
    struct Produit* suivant; // Pointeur vers le produit suivant
} Produit;

// Structure pour representer une categorie
typedef struct Categorie {
    char title[MAX_NAME_LENGTH];
    Produit* liste_produits; // Liste chainee de produits
    struct Categorie* suivant; // Pointeur vers la categorie suivante
} Categorie;

// Declarations des fonctions
Produit* creer_produit(char* nom, float prix, int quantite_disponible);
void ajouter_produit(Categorie* categorie, Produit* produit);
Categorie* creer_categorie(char* nom);
void sauvegarder_dans_fichier(const char* fichier, Categorie* liste_categories);
Categorie* charger_dans_fichier(const char* fichier);
void afficher_produits(Categorie* categorie);
void afficher_categories(Categorie* liste_categories);
void liberer_produits(Produit* produit);
void liberer_categories(Categorie* categorie);
void retirer_produit(Categorie* categorie, char* nom_produit);
void retirer_categorie(Categorie** liste_categories, char* nom_categorie);
void mise_a_jour_produit(Categorie* categorie, char* nom_produit, float nouveau_prix, int nouvelle_quantite);
int comparer_par_nom(const void* a, const void* b);
int comparer_par_prix_croissant(const void* a, const void* b);
int comparer_par_prix_decroissant(const void* a, const void* b);
int comparer_par_quantite(const void* a, const void* b);
void afficher_produits_tries(Categorie* categorie, int critere);
void afficher_tous_produits_tries(Categorie* liste_categories, int critere);
Produit* saisir_produit();
Categorie* saisir_categorie();
void saisir_categories_et_produits(Categorie** liste_categories);
Categorie* trouver_categorie(Categorie* liste, const char* nom);
Produit* trouver_produit(Produit* liste_produits, const char* nom);
int calculer_stock_total(Categorie* categorie);
void alerte_stock_faible(Categorie* categorie, int seuil);
void afficher_categories_avec_compte(Categorie* liste_categories);
Produit* rechercher_produit_par_nom(Categorie* liste_categories, const char* nom);
void afficher_statistiques(Categorie* liste_categories);
void recenser_alertes(Categorie* liste_categories);
void menu(Categorie** liste_categories);
void nettoyer_chaine(char* chaine);
void convertir_en_minuscule(char* chaine);

int main(int argc, char* argv[]) {
    char* fichier_base = FILENAME; // Fichier par défaut
    // Vérifier si un fichier est passé en argument
    if (argc == 2) {
        fichier_base = argv[1];
    } else if (argc > 2) {
        printf("\t\t\t%sErreur : Syntaxe incorrecte. Utilisation : stock FILE%s\n", ROUGE, RES);
        return 1;
    }

    Categorie* liste_categories = NULL;

    // Charger les catégories et les produits depuis le fichier spécifié
    liste_categories = charger_dans_fichier(fichier_base);

    // Appel de la fonction menu pour permettre à l'utilisateur de choisir une action
    menu(&liste_categories);

    // Sauvegarder les catégories et les produits dans le fichier spécifié
    printf("\t\t\t%sSauvegarde automatique%s\n", VERT, RES);
    sauvegarder_dans_fichier(fichier_base, liste_categories);
    
    // Libération de la mémoire allouée à la fin
    liberer_categories(liste_categories);
    return 0;
}

// Definition des fonctions
Produit* creer_produit(char* nom, float prix, int quantite_disponible) {
    if (nom == NULL || prix <= 0 || quantite_disponible < 0) {
        printf("\t\t\t%sErreur : Parametres invalides pour creer un produit.%s\n", ROUGE, RES);
        return NULL;
    }

    Produit* nouveau_produit = (Produit*)malloc(sizeof(Produit));
    if (nouveau_produit == NULL) {
        printf("\t\t\t%sErreur d'allocation memoire pour le produit.%s\n", ROUGE, RES);
        return NULL;
    }

    strncpy(nouveau_produit->name, nom, MAX_NAME_LENGTH - 1);
    nouveau_produit->name[MAX_NAME_LENGTH - 1] = '\0'; // Assurer la terminaison de la chaine
    nouveau_produit->price = prix;
    nouveau_produit->quantities = quantite_disponible;
    nouveau_produit->suivant = NULL;
    return nouveau_produit;
}

void ajouter_produit(Categorie* categorie, Produit* produit) {
    if (categorie == NULL || produit == NULL) {
        printf("\t\t\t%sErreur : Parametres invalides pour ajouter un produit.%s\n", ROUGE, RES);
        return;
    }

    if (categorie->liste_produits == NULL) {
        categorie->liste_produits = produit;
    }
    else {
        Produit* current = categorie->liste_produits;
        while (current->suivant != NULL) {
            current = current->suivant;
        }
        current->suivant = produit;
    }
}

Categorie* creer_categorie(char* nom) {
    if (nom == NULL) {
        printf("\t\t\t%sErreur : Nom de categorie invalide.%s\n", ROUGE, RES);
        return NULL;
    }

    Categorie* nouvelle_categorie = (Categorie*)malloc(sizeof(Categorie));
    if (nouvelle_categorie == NULL) {
        printf("\t\t\t%sErreur d'allocation memoire pour la categorie.%s\n", ROUGE, RES);
        return NULL;
    }

    strncpy(nouvelle_categorie->title, nom, MAX_NAME_LENGTH - 1);
    nouvelle_categorie->title[MAX_NAME_LENGTH - 1] = '\0'; // Assurer la terminaison de la chaine
    nouvelle_categorie->liste_produits = NULL;
    nouvelle_categorie->suivant = NULL;
    return nouvelle_categorie;
}

void sauvegarder_dans_fichier(const char* fichier, Categorie* liste_categories) {
    if (fichier == NULL || liste_categories == NULL) {
        printf("\t\t\t%sErreur : Parametres invalides pour la sauvegarde.%s\n", ROUGE, RES);
        return;
    }

    FILE* file = fopen(fichier, "w");
    if (file == NULL) {
        printf("\t\t\t%sErreur d'ouverture du fichier %s%s\n", ROUGE, fichier, RES);
        return;
    }

    Categorie* categorie = liste_categories;
    while (categorie != NULL) {
        // Écrire le titre de la catégorie entre crochets
        fprintf(file, "[%s]\n", categorie->title);

        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            // Écrire chaque produit sous la catégorie
            fprintf(file, "%s;%.2f;%d\n", produit->name, produit->price, produit->quantities);
            produit = produit->suivant;
        }

        fprintf(file, "\n"); // Ajouter une ligne vide entre les catégories
        categorie = categorie->suivant;
    }

    fclose(file);
    printf("\t\t\t%sDonnees sauvegardees avec succes dans '%s'.%s\n", VERT, fichier, RES);
}

Categorie* charger_dans_fichier(const char* fichier) {
    if (fichier == NULL) {
        printf("\t\t\t%sErreur : Nom de fichier invalide.%s\n", ROUGE, RES);
        return NULL;
    }

    FILE* file = fopen(fichier, "r");
    if (file == NULL) {
        printf("\t\t\t%sErreur d'ouverture du fichier %s%s\n", ROUGE, fichier, RES);
        return NULL;
    }

    Categorie* liste_categories = NULL;
    Categorie* derniere_categorie = NULL;
    char ligne[256];

    while (fgets(ligne, sizeof(ligne), file)) {
        nettoyer_chaine(ligne); // Supprimer le retour à la ligne

        if (ligne[0] == '[') { // Nouvelle catégorie
            char titre[MAX_NAME_LENGTH];
            strncpy(titre, ligne + 1, strlen(ligne) - 2); // Extraire le titre sans les crochets
            titre[strlen(ligne) - 2] = '\0';

            Categorie* nouvelle_categorie = creer_categorie(titre);
            if (nouvelle_categorie == NULL) {
                printf("\t\t\t%sErreur : Impossible de creer la categorie '%s'.%s\n", ROUGE, titre, RES);
                continue;
            }

            if (liste_categories == NULL) {
                liste_categories = nouvelle_categorie; // Premier élément de la liste
            } else {
                derniere_categorie->suivant = nouvelle_categorie; // Chaîner avec la catégorie précédente
            }
            derniere_categorie = nouvelle_categorie; // Mettre à jour la dernière catégorie
        } else if (strchr(ligne, ';') != NULL) { // Produit
            char nom[MAX_NAME_LENGTH];
            float prix = 0.0;
            int quantite = 0;

            char* token = strtok(ligne, ";");
            if (token != NULL) strncpy(nom, token, MAX_NAME_LENGTH - 1);
            nom[MAX_NAME_LENGTH - 1] = '\0';

            token = strtok(NULL, ";");
            if (token != NULL) prix = atof(token);

            token = strtok(NULL, ";");
            if (token != NULL) quantite = atoi(token);

            if (derniere_categorie != NULL) {
                Produit* produit = creer_produit(nom, prix, quantite);
                if (produit == NULL) {
                    printf("\t\t\t%sErreur : Impossible de creer le produit '%s'.%s\n", ROUGE, nom, RES);
                    continue;
                }
                ajouter_produit(derniere_categorie, produit);
            } else {
                printf("\t\t\t%sErreur : Produit '%s' sans categorie associee.%s\n", ROUGE, nom, RES);
            }
        }
    }

    fclose(file);
    printf("\t\t\t%sCategories et produits charges depuis '%s'.%s\n", VERT, fichier, RES);
    return liste_categories;
}

void afficher_produits(Categorie* categorie) {
    if (categorie == NULL) {
        printf("\t\t\t%sErreur : Categorie invalide.%s\n", ROUGE, RES);
        return;
    }

    printf("\t\t\tCategorie: %s\n", categorie->title);
    Produit* produit = categorie->liste_produits;
    if (produit == NULL) {
        printf("\t\t\t  Aucun produit dans cette categorie.\n");
        return;
    }

    while (produit != NULL) {
        printf("\t\t\t  Produit: %s, Prix: %.2f, Quantite: %d\n", produit->name, produit->price, produit->quantities);
        produit = produit->suivant;
    }
}

void afficher_categories(Categorie* liste_categories) {
    if (liste_categories == NULL) {
        printf("\t\t\t%s===================================================%s\n", BLEU, RES);
        printf("\t\t\t%s||           Aucune categorie a afficher         ||%s\n", BLEU, RES);
        printf("\t\t\t%s===================================================%s\n", BLEU, RES);
        return;
    }

    printf("\t\t\t%s===================================================%s\n", BLEU, RES);
    printf("\t\t\t%s||               Liste des categories            ||%s\n", BLEU, RES);
    printf("\t\t\t%s===================================================%s\n", BLEU, RES);
    printf("\t\t\t%s%s-------------------------------------------------------------------------%s\n", GRAS, BLEU, RES);
    printf("\t\t\t%-20s %s%s|%s %-25s %s%s|%s %-10s %s%s|%s %-10s\n", "Categorie", GRAS, BLEU, RES, "Nom du produit", GRAS, BLEU, RES, "Prix", GRAS, BLEU, RES, "Quantite");
    printf("\t\t\t%s%s-------------------------------------------------------------------------%s\n", GRAS, BLEU, RES);

    Categorie* categorie = liste_categories;
    while (categorie != NULL) {
        Produit* produit = categorie->liste_produits;
        if (produit == NULL) {
            printf("\t\t\t%s%-20s%s %s%s|%s %-25s %s%s|%s %-10s %s%s|%s %-10s\n", cyan, categorie->title, RES, GRAS, BLEU, RES, "Aucun produit", GRAS, BLEU, RES, "-", GRAS, BLEU, RES, "-");
        } else {
            printf("\t\t\t%s%-20s%s %s%s|%s %-25s %s%s|%s %-10.2f %s%s|%s %-10d\n", cyan, categorie->title, RES, GRAS, BLEU, RES, produit->name, GRAS, BLEU, RES, produit->price, GRAS, BLEU, RES, produit->quantities);
            produit = produit->suivant;
            while (produit != NULL) {
                printf("\t\t\t\t\t     %s%s----------------------------------------------------%s\n", GRAS, BLEU, RES);
                printf("\t\t\t%-20s %s%s|%s %-25s %s%s|%s %-10.2f %s%s|%s %-10d\n", "", GRAS, BLEU, RES, produit->name, GRAS, BLEU, RES, produit->price, GRAS, BLEU, RES, produit->quantities);
                produit = produit->suivant;
            }
        }
        printf("\t\t\t%s%s-------------------------------------------------------------------------%s\n", GRAS, JAUNE, RES);
        categorie = categorie->suivant;
    }
}

void liberer_produits(Produit* produit) {
    while (produit != NULL) {
        Produit* temp = produit;
        produit = produit->suivant;
        free(temp); // Liberer chaque produit
    }
}

void liberer_categories(Categorie* categorie) {
    while (categorie != NULL) {
        Categorie* temp = categorie;
        categorie = categorie->suivant;
        liberer_produits(temp->liste_produits); // Liberer les produits avant de liberer la categorie
        free(temp); // Liberer la categorie
    }
}

void retirer_produit(Categorie* categorie, char* nom_produit) {
    if (categorie == NULL) {
        printf("\t\t\t%sCategorie invalide.%s\n", ROUGE, RES);
        return;
    }

    Produit* current = categorie->liste_produits;
    Produit* prev = NULL;

    // Chercher le produit dans la liste chainee
    while (current != NULL && strcmp(current->name, nom_produit) != 0) {
        prev = current;
        current = current->suivant;
    }

    // Si le produit est trouve
    if (current != NULL) {
        if (prev == NULL) {
            // Si le produit est le premier de la liste
            categorie->liste_produits = current->suivant;
        }
        else {
            // Si le produit est au milieu ou a la fin de la liste
            prev->suivant = current->suivant;
        }
        free(current); // Liberer la memoire allouee pour le produit
        printf("\t\t\t%s%sProduit '%s' retire de la categorie '%s'.%s\n", GRAS, VERT, nom_produit, categorie->title, RES);
    }
    else {
        printf("\t\t\t%s%SProduit '%s' non trouve dans la categorie '%s'.%s\n", GRAS, ROUGE, nom_produit, categorie->title, RES);
    }
}

void retirer_categorie(Categorie** liste_categories, char* nom_categorie) {
    if (liste_categories == NULL || *liste_categories == NULL || nom_categorie == NULL) {
        printf("\t\t\t%sErreur : Parametres invalides pour retirer une categorie.%s\n", ROUGE, RES);
        return;
    }

    Categorie* current = *liste_categories;
    Categorie* prev = NULL;

    // Chercher la categorie dans la liste chainee
    while (current != NULL && strcmp(current->title, nom_categorie) != 0) {
        prev = current;
        current = current->suivant;
    }

    // Si la categorie est trouvee
    if (current != NULL) {
        liberer_produits(current->liste_produits); // Liberer tous les produits de la categorie
        if (prev == NULL) {
            *liste_categories = current->suivant;
        }
        else {
            prev->suivant = current->suivant;
        }
        free(current); // Liberer la memoire allouee pour la categorie
        printf("\t\t\t%s%sCategorie '%s' retiree avec succes.%s\n", GRAS, VERT, nom_categorie, RES);
    }
    else {
        printf("\t\t\t%s%sCategorie '%s' non trouvee.%s\n", GRAS, ROUGE,  nom_categorie, RES);
    }
}

void mise_a_jour_produit(Categorie* categorie, char* nom_produit, float nouveau_prix, int nouvelle_quantite) {
    if (categorie == NULL) {
        printf("\t\t\t%sErreur : Categorie invalide.%s\n", ROUGE, RES);
        return;
    }

    Produit* current = categorie->liste_produits;

    while (current != NULL) {
        if (strcmp(current->name, nom_produit) == 0) {
            current->price = nouveau_prix;
            current->quantities = nouvelle_quantite;
            printf("\t\t\t%sProduit '%s' mis a jour avec succes : Nouveau prix = %.2f, Nouvelle quantite = %d.%s\n",
                VERT, nom_produit, nouveau_prix, nouvelle_quantite, RES);
            return;
        }
        current = current->suivant;
    }

    printf("\t\t\t%sErreur : Produit '%s' non trouve dans la categorie '%s'.%s\n", ROUGE, nom_produit, categorie->title, RES);
}

int comparer_par_nom(const void* a, const void* b) {
    Produit* p1 = *(Produit**)a;
    Produit* p2 = *(Produit**)b;
    return strcmp(p1->name, p2->name);
}

int comparer_par_prix_croissant(const void* a, const void* b) {
    Produit* p1 = *(Produit**)a;
    Produit* p2 = *(Produit**)b;
    return (p1->price > p2->price) - (p1->price < p2->price);
}

int comparer_par_prix_decroissant(const void* a, const void* b) {
    Produit* p1 = *(Produit**)a;
    Produit* p2 = *(Produit**)b;
    return (p2->price > p1->price) - (p2->price < p1->price);
}

int comparer_par_quantite(const void* a, const void* b) {
    Produit* p1 = *(Produit**)a;
    Produit* p2 = *(Produit**)b;
    return p1->quantities - p2->quantities;
}

void afficher_produits_tries(Categorie* categorie, int critere) {
    if (categorie == NULL) {
        printf("\t\t\t%sErreur : Categorie invalide.%s\n", ROUGE, RES);
        return;
    }

    // Compter le nombre de produits
    int count = 0;
    Produit* current = categorie->liste_produits;
    while (current != NULL) {
        count++;
        current = current->suivant;
    }

    if (count == 0) {
        printf("\t\t\t%sAucun produit a trier dans la categorie '%s'.%s\n", ROUGE, categorie->title, RES);
        return;
    }

    // Créer un tableau temporaire pour stocker les produits
    Produit** produits_array = (Produit**)malloc(count * sizeof(Produit*));
    if (produits_array == NULL) {
        printf("\t\t\t%sErreur : Allocation memoire echouee pour le tri.%s\n", ROUGE, RES);
        return;
    }

    current = categorie->liste_produits;
    for (int i = 0; i < count; i++) {
        produits_array[i] = current;
        current = current->suivant;
    }

    // Trier le tableau en fonction du critere choisi
    switch (critere) {
    case 1: // Tri par nom (ordre alphabetique)
        qsort(produits_array, count, sizeof(Produit*), comparer_par_nom);
        break;
    case 2: // Tri par prix croissant
        qsort(produits_array, count, sizeof(Produit*), comparer_par_prix_croissant);
        break;
    case 3: // Tri par prix decroissant
        qsort(produits_array, count, sizeof(Produit*), comparer_par_prix_decroissant);
        break;
    case 4: // Tri par quantite disponible
        qsort(produits_array, count, sizeof(Produit*), comparer_par_quantite);
        break;
    default:
        printf("\t\t\t%sCritere de tri invalide.%s\n", ROUGE, RES);
        free(produits_array);
        return;
    }

    // Afficher les produits tries avec un formatage coloré
    printf("\t\t\t%s%sProduits de la categorie '%s' tries :%s\n", GRAS, cyan, categorie->title, RES);
    printf("\t\t\t%s===================================================%s\n", BLEU, RES);
    printf("\t\t\t%s%-20s | %-10s | %-10s%s\n", GRAS, "Nom du produit", "Prix", "Quantite", RES);
    printf("\t\t\t%s---------------------------------------------------%s\n", BLEU, RES);

    for (int i = 0; i < count; i++) {
        printf("\t\t\t%-20s | %s%-10.2f%s | %s%-10d%s\n",
               produits_array[i]->name,
               VERT, produits_array[i]->price, RES,
               JAUNE, produits_array[i]->quantities, RES);
    }

    printf("\t\t\t%s===================================================%s\n", BLEU, RES);

    free(produits_array); // Liberer la memoire allouee pour le tableau temporaire
}

void afficher_tous_produits_tries(Categorie* liste_categories, int critere) {
    if (liste_categories == NULL) {
        printf("\t\t\t%sErreur : Liste des categories vide.%s\n", ROUGE, RES);
        return;
    }

    // Compter le nombre total de produits
    int count = 0;
    Categorie* categorie = liste_categories;
    while (categorie != NULL) {
        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            count++;
            produit = produit->suivant;
        }
        categorie = categorie->suivant;
    }

    if (count == 0) {
        printf("\t\t\t%s%sAucun produit a trier.%s\n", GRAS, ROUGE, RES);
        return;
    }

    // Créer un tableau temporaire pour stocker tous les produits et leurs catégories
    struct {
        Produit* produit;
        char* categorie;
    } *produits_array = malloc(count * sizeof(*produits_array));
    if (produits_array == NULL) {
        printf("\t\t\t%sErreur : Allocation memoire echouee pour le tri.%s\n", ROUGE, RES);
        return;
    }

    // Remplir le tableau avec tous les produits et leurs catégories
    int index = 0;
    categorie = liste_categories;
    while (categorie != NULL) {
        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            produits_array[index].produit = produit;
            produits_array[index].categorie = categorie->title;
            index++;
            produit = produit->suivant;
        }
        categorie = categorie->suivant;
    }

    // Trier le tableau en fonction du critere choisi
    switch (critere) {
    case 1: // Tri par nom (ordre alphabetique)
        qsort(produits_array, count, sizeof(*produits_array), comparer_par_nom);
        break;
    case 2: // Tri par prix croissant
        qsort(produits_array, count, sizeof(*produits_array), comparer_par_prix_croissant);
        break;
    case 3: // Tri par prix decroissant
        qsort(produits_array, count, sizeof(*produits_array), comparer_par_prix_decroissant);
        break;
    case 4: // Tri par quantite disponible
        qsort(produits_array, count, sizeof(*produits_array), comparer_par_quantite);
        break;
    default:
        printf("\t\t\t%sCritere de tri invalide.%s\n", ROUGE, RES);
        free(produits_array);
        return;
    }

    // Afficher les produits tries avec un formatage coloré
    printf("\t\t\t%sTous les produits tries :%s\n", GRAS, RES);
    printf("\t\t\t%s=============================================================%s\n", BLEU, RES);
    printf("\t\t\t%s%-20s | %-10s | %-10s | %-20s%s\n", GRAS, "Nom du produit", "Prix", "Quantite", "Categorie", RES);
    printf("\t\t\t%s-------------------------------------------------------------%s\n", BLEU, RES);

    for (int i = 0; i < count; i++) {
        printf("\t\t\t%-20s | %s%-10.2f%s | %s%-10d%s | %s%-20s%s\n",
               produits_array[i].produit->name,
               VERT, produits_array[i].produit->price, RES,
               JAUNE, produits_array[i].produit->quantities, RES,
               cyan, produits_array[i].categorie, RES);
    }

    printf("\t\t\t%s=============================================================%s\n", BLEU, RES);

    free(produits_array); // Liberer la memoire allouee pour le tableau temporaire
}

Produit* saisir_produit() {
    Produit* nouveau_produit = (Produit*)malloc(sizeof(Produit));
    if (nouveau_produit == NULL) {
        printf("\t\t\t%sErreur d'allocation memoire pour le produit.%s\n", ROUGE, RES);
        return NULL;
    }

    printf("\t\t\tEntrez le nom du produit : ");
    fgets(nouveau_produit->name, MAX_NAME_LENGTH, stdin);
    nouveau_produit->name[strcspn(nouveau_produit->name, "\n")] = '\0';
    nettoyer_chaine(nouveau_produit->name); // Supprimer le '\n'
    convertir_en_minuscule(nouveau_produit->name); // Convertir en minuscule

    do {
        printf("\t\t\tEntrez le prix du produit (positif) : ");
        if (scanf("%f", &nouveau_produit->price) != 1 || nouveau_produit->price <= 0) {
            printf("\t\t\t%sErreur : Le prix doit etre un nombre positif.%s\n", ROUGE, RES);
            while (getchar() != '\n'); // Nettoyage du buffer
        }
        else {
            break;
        }
    } while (1);

    do {
        printf("\t\t\tEntrez la quantite disponible du produit (positive) : ");
        if (scanf("%d", &nouveau_produit->quantities) != 1 || nouveau_produit->quantities < 0) {
            printf("\t\t\t%sErreur : La quantite doit etre un entier positif.%s\n", ROUGE, RES);
            while (getchar() != '\n'); // Nettoyage du buffer
        }
        else {
            break;
        }
    } while (1);

    nouveau_produit->suivant = NULL;
    getchar(); // Consommer le '\n' restant dans le buffer de saisie

    return nouveau_produit;
}

Categorie* saisir_categorie() {
    Categorie* nouvelle_categorie = (Categorie*)malloc(sizeof(Categorie));
    if (nouvelle_categorie == NULL) {
        printf("\t\t\t%sErreur d'allocation memoire pour la categorie.%s\n", ROUGE, RES);
        return NULL;
    }

    printf("\t\t\tEntrez le nom de la categorie : ");
    fgets(nouvelle_categorie->title, MAX_NAME_LENGTH, stdin);
    // Supprimer le '\n' a la fin de la chaine
    nouvelle_categorie->title[strcspn(nouvelle_categorie->title, "\n")] = 0;
    nettoyer_chaine(nouvelle_categorie->title); // Supprimer le '\n'
    convertir_en_minuscule(nouvelle_categorie->title); // Convertir en minuscule

    nouvelle_categorie->liste_produits = NULL;
    nouvelle_categorie->suivant = NULL;

    return nouvelle_categorie;
}

void saisir_categories_et_produits(Categorie** liste_categories) {
    int continuer = 1;

    while (continuer) {
        // Saisie d'une nouvelle categorie
        Categorie* nouvelle_categorie = saisir_categorie();

        // Vérifier si la catégorie existe déjà
        if (trouver_categorie(*liste_categories, nouvelle_categorie->title) != NULL) {
            printf("\t\t\t%sErreur : La categorie '%s' existe deja.%s\n", ROUGE, nouvelle_categorie->title, RES);
            free(nouvelle_categorie);
            continue;
        }

        // Ajouter la categorie à la fin de la liste des categories
        if (*liste_categories == NULL) {
            *liste_categories = nouvelle_categorie; // Si la liste est vide
        } else {
            Categorie* temp = *liste_categories;
            while (temp->suivant != NULL) {
                temp = temp->suivant; // Parcourir jusqu'à la fin
            }
            temp->suivant = nouvelle_categorie; // Ajouter à la fin
        }

        int saisie_produit = 1;
        while (saisie_produit) {
            // Saisie d'un produit
            Produit* nouveau_produit = saisir_produit();

            // Vérifier si le produit existe déjà dans la catégorie
            if (trouver_produit(nouvelle_categorie->liste_produits, nouveau_produit->name) != NULL) {
                printf("\t\t\t%sErreur : Le produit '%s' existe deja dans la categorie '%s'.%s\n", ROUGE, nouveau_produit->name, nouvelle_categorie->title, RES);
                free(nouveau_produit);
                continue;
            }

            ajouter_produit(nouvelle_categorie, nouveau_produit);

            // Demander à l'utilisateur s'il veut saisir un autre produit pour cette catégorie
            printf("\n\t\t\t%s%sVoulez-vous saisir un autre produit pour cette categorie ? (1 = Oui, 0 = Non) : %s", GRAS, cyan, RES);
            scanf("%d", &saisie_produit);
            getchar(); // Consommer le '\n' restant
        }

        // Demander à l'utilisateur s'il veut saisir une autre catégorie
        printf("\n\t\t\t%s%sVoulez-vous saisir une autre categorie ? (1 = Oui, 0 = Non) : %s", GRAS, JAUNE, RES);
        scanf("%d", &continuer);
        getchar(); // Consommer le '\n' restant
    }
}

Categorie* trouver_categorie(Categorie* liste, const char* nom) {
    if (liste == NULL || nom == NULL) {
        printf("\t\t\t%sErreur : Liste ou nom invalide.%s\n", ROUGE, RES);
        return NULL;
    }

    char nom_minuscule[MAX_NAME_LENGTH];
    strncpy(nom_minuscule, nom, MAX_NAME_LENGTH - 1);
    nom_minuscule[MAX_NAME_LENGTH - 1] = '\0';
    convertir_en_minuscule(nom_minuscule); // Convertir en minuscule pour la recherche

    while (liste != NULL) {
        if (strcmp(liste->title, nom_minuscule) == 0) {
            return liste;
        }
        liste = liste->suivant;
    }

    //printf("\t\t\t%sErreur : Categorie '%s' non trouvee.%s\n", ROUGE, nom, RES);
    return NULL;
}

Produit* trouver_produit(Produit* liste_produits, const char* nom) {
    char nom_minuscule[MAX_NAME_LENGTH];
    strncpy(nom_minuscule, nom, MAX_NAME_LENGTH - 1);
    nom_minuscule[MAX_NAME_LENGTH - 1] = '\0';
    convertir_en_minuscule(nom_minuscule); // Convertir en minuscule pour la recherche

    while (liste_produits != NULL) {
        if (strcmp(liste_produits->name, nom_minuscule) == 0) {
            return liste_produits;
        }
        liste_produits = liste_produits->suivant;
    }
    return NULL;
}

int comparer_par_prix_et_quantite(const void* a, const void* b) {
    Produit* p1 = *(Produit**)a;
    Produit* p2 = *(Produit**)b;
    int cmp_price = (p1->price > p2->price) - (p1->price < p2->price);
    if (cmp_price != 0) {
        return cmp_price;
    }
    return p1->quantities - p2->quantities;
}

int calculer_stock_total(Categorie* categorie) {
    int stock_total = 0;
    Produit* produit = categorie->liste_produits;
    while (produit != NULL) {
        stock_total += produit->quantities;
        produit = produit->suivant;
    }
    return stock_total;
}

void alerte_stock_faible(Categorie* categorie, int seuil) {
    Produit* produit = categorie->liste_produits;
    while (produit != NULL) {
        if (produit->quantities < seuil) {
            printf("\t\t\t%s%sAlerte:%s Le produit '%s' a un stock faible (%d unites restantes).\n", GRAS, ROUGE, RES, produit->name, produit->quantities);
        }
        produit = produit->suivant;
    }
}

void afficher_categories_avec_compte(Categorie* liste_categories) {
    Categorie* categorie = liste_categories;
    while (categorie != NULL) {
        int count = 0;
        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            count++;
            produit = produit->suivant;
        }
        printf("\t\t\tCategorie '%s' contient %d produit(s).\n", categorie->title, count);
        categorie = categorie->suivant;
    }
}

Produit* rechercher_produit_par_nom(Categorie* liste_categories, const char* nom) {
    Categorie* categorie = liste_categories;
    while (categorie != NULL) {
        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            if (strcmp(produit->name, nom) == 0) {
                return produit; // Trouve
            }
            produit = produit->suivant;
        }
        categorie = categorie->suivant;
    }
    return NULL; // Non trouve
}

void afficher_statistiques(Categorie* liste_categories) {
    int total_produits = 0;
    float valeur_totale = 0.0;
    Categorie* categorie = liste_categories;

    while (categorie != NULL) {
        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            total_produits++;
            valeur_totale += produit->price * produit->quantities;
            produit = produit->suivant;
        }
        categorie = categorie->suivant;
    }

    printf("\t\t\tNombre total de produits : %d\n", total_produits);
    printf("\t\t\tValeur totale du stock : %.2f\n", valeur_totale);
}

void recenser_alertes(Categorie* liste_categories) {
    if (liste_categories == NULL) {
        printf("\t\t\t%sAucune categorie disponible pour recenser les alertes.%s\n", ROUGE, RES);
        return;
    }

    printf("\t\t\t%s===================================================%s\n", ROUGE, RES);
    printf("\t\t\t%s||               Alertes de stock                ||%s\n", ROUGE, RES);
    printf("\t\t\t%s===================================================%s\n", ROUGE, RES);
    printf("\t\t\t%-18s %s%s|%s %-12s %s%s|%s %-10s\n", "Categorie", GRAS, ROUGE, RES, "Produit", GRAS, ROUGE, RES, "Stock restant");
    printf("\t\t\t%s%s---------------------------------------------------%s\n", GRAS, ROUGE, RES);

    int alertes_trouvees = 0;
    Categorie* categorie = liste_categories;

    while (categorie != NULL) {
        Produit* produit = categorie->liste_produits;
        while (produit != NULL) {
            if (produit->quantities < SEUIL_ALERTE) {
                printf("\t\t\t%-18s %s%s|%s %-12s %s%s|%s %-10d\n", categorie->title, GRAS, ROUGE, RES, produit->name, GRAS, ROUGE, RES, produit->quantities);
                alertes_trouvees = 1;
            }
            produit = produit->suivant;
        }
        categorie = categorie->suivant;
    }

    if (!alertes_trouvees) {
        printf("\t\t\t%sAucune alerte de stock detectee.%s\n", VERT, RES);
    }

    printf("\t\t\t%s%s---------------------------------------------------%s\n", GRAS, ROUGE, RES);
}

void nettoyer_chaine(char* chaine) {
    if (chaine == NULL) return;
    size_t longueur = strlen(chaine);
    if (longueur > 0 && chaine[longueur - 1] == '\n') {
        chaine[longueur - 1] = '\0';
    }
}

void convertir_en_minuscule(char* chaine) {
    if (chaine == NULL) return;
    for (int i = 0; chaine[i]; i++) {
        chaine[i] = tolower((unsigned char)chaine[i]);
    }
}

void menu(Categorie** liste_categories) {
    int choix, sous_choix;
    char nom_categorie[MAX_NAME_LENGTH];
    Categorie* categorie;
    
    do {
        //system("cls"); // Effacer l'écran
        // Affichage du menu principal
        printf("\n%s%s", GRAS, BLEU);
        printf("\t\t\t###################################################\n");
        printf("\t\t\t#                                                 #\n");
        printf("\t\t\t#                GESTION DE STOCK                 #\n");
        printf("\t\t\t#                                                 #\n");
        printf("\t\t\t###################################################\n");
        printf("%s\n", RES);

        // Ajouter une bordure autour du menu avec des bordures bleues
        printf("\t\t\t%s===================================================%s\n", BLEU, RES);
        printf("\t\t\t%s||                 Menu Principal                ||%s\n", BLEU, RES);
        printf("\t\t\t%s===================================================%s\n", BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s||%s %s1. Gestion des categories%s                     %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
        printf("\t\t\t%s||%s    1.1. Creer une categorie                   %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    1.2. Afficher toutes les categories        %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    1.3. Retirer une categorie                 %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    1.4. Modifier une categorie                %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s||%s %s2. Gestion des produits%s                       %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
        printf("\t\t\t%s||%s    2.1. Ajouter un produit                    %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    2.2. Afficher les produits                 %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    2.3. Retirer un produit                    %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    2.4. Mettre a jour un produit              %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    2.5. Trier les produits                    %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    2.7. Trier tous les produits               %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s||%s %s3. Analyse et statistiques%s                    %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
        printf("\t\t\t%s||%s    3.1. Calculer le stock total               %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    3.2. Alerte stock faible categorie         %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    3.3. Alerte stock faible produit           %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    3.4. Rechercher un produit                 %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    3.5. Afficher les statistiques             %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s||%s %s4. Gestion des fichiers%s                       %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
        printf("\t\t\t%s||%s    4.1. Sauvegarder                           %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    4.2. Charger                               %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||%s    4.3. Afficher le contenu                   %s||%s\n", BLEU, RES, BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s||%s %s5. Afficher les alertes de stock%s              %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s||%s %s6. Quitter %s                                   %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
        printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
        printf("\t\t\t%s===================================================%s\n", BLEU, RES);
        printf("\t\t\tChoisissez une option (1-6): ");

        if (scanf("%d", &choix) != 1 || choix < 1 || choix > 6) {
            printf("\t\t\t%sErreur : Entree invalide. Veuillez entrer un nombre entre 1 et 6.%s\n", ROUGE, RES);
            while (getchar() != '\n'); // Nettoyage du buffer
            continue;
        }
        getchar(); // Consommer le retour à la ligne après scanf

        switch (choix) {
        
        case 4: // Fichiers
        system("cls"); // Effacer l'écran
            do {
                
                // Affichage du sous-menu fichiers
                printf("\n%s%s", GRAS, BLEU);
                printf("\t\t\t###################################################\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t#                GESTION DE STOCK                 #\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t###################################################\n");
                printf("%s\n", RES);

                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||             Gestion des fichiers              ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s||%s %s0. Retour%s                                     %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
                printf("\t\t\t%s||%s 1. Sauvegarder dans un fichier                %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 2. Charger depuis un fichier                  %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 3. Afficher le contenu du fichier             %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\tChoisissez une sous-option (0-3): ");
 
                if (scanf("%d", &sous_choix) != 1 || sous_choix < 0 || sous_choix > 3) {
                    printf("\t\t\t%sErreur : Entree invalide. Veuillez entrer un nombre entre 0 et 3.%s\n", ROUGE, RES);
                    while (getchar() != '\n'); // Nettoyage du buffer
                    continue;
                }
                getchar(); // Consommer le retour à la ligne après scanf

                switch (sous_choix) {
                case 0:
                    break;
                case 1: { // Sauvegarder dans un fichier
                    sauvegarder_dans_fichier(FILENAME, *liste_categories);
                    break;
                }
                case 2: { // Charger depuis un fichier
                    *liste_categories = charger_dans_fichier(FILENAME);
                    break;
                }
                case 3: { // Afficher le contenu du fichier
                    FILE* file = fopen(FILENAME, "r");
                    if (file == NULL) {
                        printf("\t\t\t%sErreur : Impossible d'ouvrir le fichier '%s'.%s\n", ROUGE, FILENAME, RES);
                    }
                    else {
                        printf("\t\t\t%sContenu du fichier '%s' :%s\n", BLEU, FILENAME, RES);
                        char ligne[256];
                        while (fgets(ligne, sizeof(ligne), file)) {
                            printf("\t\t\t%s", ligne);
                        }
                        fclose(file);
                    }
                    break;
                }
                default:
                    printf("\t\t\t%sOption invalide.%s\n", ROUGE, RES);
                    break;
                }
            } while (sous_choix != 0);
            break;

        case 1: // Gestion des categories
            system("cls"); // Effacer l'écran
            do {
                
                // Affichage du sous-menu categories
                printf("\n%s%s", GRAS, BLEU);
                printf("\t\t\t###################################################\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t#                GESTION DE STOCK                 #\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t###################################################\n");
                printf("%s\n", RES);

                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||           Gestion des categories              ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s||%s %s0. Retour %s                                    %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
                printf("\t\t\t%s||%s 1. Creer une categorie                        %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 2. Afficher toutes les categories             %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 3. Retirer une categorie                      %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 4. Modifier une categorie                     %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\tChoisissez une sous-option (0-4): ");

                if (scanf("%d", &sous_choix) != 1 || sous_choix < 0 || sous_choix > 4) {
                    printf("\t\t\t%sErreur : Entree invalide. Veuillez entrer un nombre entre 0 et 4.%s\n", ROUGE, RES);
                    while (getchar() != '\n'); // Nettoyage du buffer
                    continue;
                }
                getchar(); // Consommer le retour à la ligne après scanf

                switch (sous_choix) {
                case 0:
                    break;
                case 1:
                    saisir_categories_et_produits(liste_categories);
                    break;
                case 2:
                    afficher_categories(*liste_categories);
                    break;
                case 3:
                    printf("\t\t\tEntrez le nom de la categorie a retirer: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    retirer_categorie(liste_categories, nom_categorie);
                    break;
                case 4:
                    printf("\t\t\tEntrez le nom de la categorie a modifier: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        printf("\t\t\tEntrez le nouveau nom de la categorie: ");
                        fgets(categorie->title, MAX_NAME_LENGTH, stdin);
                        nettoyer_chaine(categorie->title); // Nettoyer la chaîne
                        convertir_en_minuscule(categorie->title); // Convertir en minuscule
                        printf("\t\t\t%s%sCategorie modifiee avec succes.%s\n",GRAS, VERT, RES);
                    }
                    else {
                        printf("\t\t\t%sCategorie non trouvee.%s\n", ROUGE, RES);
                    }
                    break;
                default:
                    printf("\t\t\t%sOption invalide.%s\n", ROUGE, RES);
                    break;
                }
            } while (sous_choix != 0);
            break;

        case 2: // Gestion des produits
            system("cls"); // Effacer l'écran
            do {
                // Affichage du sous-menu produits
                printf("\n%s%s", GRAS, BLEU);
                printf("\t\t\t###################################################\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t#                GESTION DE STOCK                 #\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t###################################################\n");
                printf("%s\n", RES);

                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||            Gestion des produits               ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s||%s %s0. Retour%s                                     %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
                printf("\t\t\t%s||%s 1. Ajouter un produit                         %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 2. Afficher les produits                      %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 3. Retirer un produit                         %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 4. Mettre a jour un produit                   %s||%s\n", BLEU, RES,  BLEU, RES);
                printf("\t\t\t%s||%s 5. Trier les produits                         %s||%s\n", BLEU, RES,  BLEU, RES);
                printf("\t\t\t%s||%s 6. Trier tous les produits                    %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\tChoisissez une sous-option (0-6): ");

                if (scanf("%d", &sous_choix) != 1 || sous_choix < 0 || sous_choix > 6) {
                    printf("\t\t\t%sErreur : Entree invalide. Veuillez entrer un nombre entre 0 et 7.%s\n", ROUGE, RES);
                    while (getchar() != '\n'); // Nettoyage du buffer
                    continue;
                }
                getchar(); // Consommer le retour à la ligne après scanf

                switch (sous_choix) {
                case 0:
                    break;
                case 1: // Ajouter un produit
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        Produit* nouveau_produit = saisir_produit();

                        // Vérifier si le produit existe déjà dans la catégorie
                        if (trouver_produit(categorie->liste_produits, nouveau_produit->name) != NULL) {
                            printf("\t\t\t%sErreur : Le produit '%s' existe deja dans la categorie '%s'.%s\n", ROUGE, nouveau_produit->name, categorie->title, RES);
                            free(nouveau_produit); // Libérer la mémoire allouée pour le produit
                        } else {
                            ajouter_produit(categorie, nouveau_produit);
                            printf("\t\t\t%s%SProduit ajoute a la categorie '%s'.%s\n",GRAS,VERT, categorie->title, RES);
                        }
                    } else {
                        printf("\t\t\t%s%sCategorie non trouvee.%s\n", GRAS, ROUGE, RES);
                    }
                    break;
                case 2: // Afficher les produits
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        Produit* produit = categorie->liste_produits;
                        if (produit == NULL) {
                            printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                            printf("\t\t\t%s||           Aucun produit a afficher           ||%s\n", BLEU, RES);
                            printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                        } else {
                            printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                            printf("\t\t\t%s||           Liste des produits                 ||%s\n", BLEU, RES);
                            printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                            printf("\t\t\t%s%s---------------------------------------------------%s\n", GRAS, BLEU, RES);
                            printf("\t\t\t%-20s %s%s|%s %-10s %s%s|%s %-10s\n", "Nom du produit", GRAS, BLEU, RES, "Prix", GRAS, BLEU, RES, "Quantite");
                            printf("\t\t\t%s%s---------------------------------------------------%s\n", GRAS, BLEU, RES);

                            while (produit != NULL) {
                                printf("\t\t\t%-20s %s%s|%s %-10.2f %s%s|%s %-10d\n", produit->name, GRAS, BLEU, RES, produit->price, GRAS, BLEU, RES, produit->quantities);
                                produit = produit->suivant;
                            }

                            printf("\t\t\t%s%s---------------------------------------------------%s\n", GRAS, JAUNE, RES);
                        }
                    } else {
                        printf("\t\t\t%sCategorie non trouvee.%s\n", ROUGE, RES);
                    }
                    break;
                case 3:
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        char nom_produit[MAX_NAME_LENGTH];
                        printf("\t\t\tEntrez le nom du produit a retirer: ");
                        fgets(nom_produit, MAX_NAME_LENGTH, stdin);
                        nettoyer_chaine(nom_produit); // Nettoyer la chaîne
                        convertir_en_minuscule(nom_produit); // Convertir en minuscule
                        retirer_produit(categorie, nom_produit);
                    }
                    else {
                        printf("\t\t\t%s%sCategorie non trouvee.%s\n", GRAS, ROUGE, RES);
                    }
                    break;
                case 4:
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        char nom_produit[MAX_NAME_LENGTH];
                        float nouveau_prix;
                        int nouvelle_quantite;
                        printf("\t\t\tEntrez le nom du produit a mettre a jour: ");
                        fgets(nom_produit, MAX_NAME_LENGTH, stdin);
                        nettoyer_chaine(nom_produit); // Nettoyer la chaîne
                        convertir_en_minuscule(nom_produit); // Convertir en minuscule
                        printf("\t\t\tEntrez le nouveau prix: ");
                        scanf("%f", &nouveau_prix);
                        printf("\t\t\tEntrez la nouvelle quantite: ");
                        scanf("%d", &nouvelle_quantite);
                        mise_a_jour_produit(categorie, nom_produit, nouveau_prix, nouvelle_quantite);
                    }
                    else {
                        printf("\t\t\t%s%sCategorie non trouvee.%s\n", GRAS, ROUGE, RES);
                    }
                    break;
                case 5: // Afficher les produits tries
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        int critere;
                        printf("\t\t\tChoisissez le critere de tri:\n");
                        printf("\t\t\t\t1. Par nom\n");
                        printf("\t\t\t\t2. Par prix croissant\n");
                        printf("\t\t\t\t3. Par prix decroissant\n");
                        printf("\t\t\t\t4. Par quantite\n");
                        printf("\t\t\tVotre choix (1-4): ");
                        if (scanf("%d", &critere) != 1 || critere < 1 || critere > 4) {
                            printf("\t\t\t%sErreur : Entree invalide pour le critere.%s\n", ROUGE, RES);
                            while (getchar() != '\n'); // Nettoyage du buffer
                            break;
                        }
                        afficher_produits_tries(categorie, critere);
                    }
                    else {
                        printf("\t\t\t%sCategorie non trouvee.%s\n", ROUGE, RES);
                    }
                    break;
                case 6: // Trier tous les produits
                    printf("\t\t\tChoisissez le critere de tri:\n");
                    printf("\t\t\t\t1. Par nom\n");
                    printf("\t\t\t\t2. Par prix croissant\n");
                    printf("\t\t\t\t3. Par prix decroissant\n");
                    printf("\t\t\t\t4. Par quantite\n");
                    printf("\t\t\tVotre choix (1-4): ");
                    int critere;
                    if (scanf("%d", &critere) != 1 || critere < 1 || critere > 4) {
                        printf("\t\t\t%sErreur : Entree invalide pour le critere.%s\n", ROUGE, RES);
                        while (getchar() != '\n'); // Nettoyage du buffer
                        break;
                    }
                    afficher_tous_produits_tries(*liste_categories, critere);
                    break;
                default:
                    printf("\t\t\t%sOption invalide.%s\n", ROUGE, RES);
                    break;
                }
            } while (sous_choix != 0);
            break;

        case 3: // Analyse et statistiques
            system("cls"); // Effacer l'écran
            do {
                // Affichage du sous-menu statistiques
                printf("\n%s%s", GRAS, BLEU);
                printf("\t\t\t###################################################\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t#                GESTION DE STOCK                 #\n");
                printf("\t\t\t#                                                 #\n");
                printf("\t\t\t###################################################\n");
                printf("%s\n", RES);

                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||         Analyse et statistiques               ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s||%s %s0. Retour %s                                    %s||%s\n", BLEU, RES, cyan, RES, BLEU, RES);
                printf("\t\t\t%s||%s 1. Calculer le stock total                    %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 2. Alerte stock faible categorie              %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 3. Alerte stock faible produit                %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 4. Rechercher un produit                      %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||%s 5. Afficher les statistiques                  %s||%s\n", BLEU, RES, BLEU, RES);
                printf("\t\t\t%s||                                               ||%s\n", BLEU, RES);
                printf("\t\t\t%s===================================================%s\n", BLEU, RES);
                printf("\t\t\tChoisissez une sous-option (0-5): ");

                if (scanf("%d", &sous_choix) != 1 || sous_choix < 0 || sous_choix > 5) {
                    printf("\t\t\t%sErreur : Entree invalide. Veuillez entrer un nombre entre 0 et 5.%s\n", ROUGE, RES);
                    while (getchar() != '\n'); // Nettoyage du buffer
                    continue;
                }
                getchar(); // Consommer le retour à la ligne après scanf

                switch (sous_choix) {
                case 0:
                    break;
                case 1:
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        int stock_total = calculer_stock_total(categorie);
                        printf("\t\t\tLe stock total de la categorie '%s' est de %d produits.\n", categorie->title, stock_total);
                    }
                    else {
                        printf("\t\t\t%sCategorie non trouvee.%s\n", ROUGE, RES);
                    }
                    break;
                case 2:
                    printf("\t\t\tEntrez le nom de la categorie: ");
                    fgets(nom_categorie, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_categorie); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_categorie); // Convertir en minuscule
                    categorie = trouver_categorie(*liste_categories, nom_categorie);
                    if (categorie) {
                        int seuil;
                        printf("\t\t\tEntrez le seuil pour le stock faible: ");
                        scanf("%d", &seuil);
                        alerte_stock_faible(categorie, seuil);
                    }
                    else {
                        printf("\t\t\t%sCategorie non trouvee.%s\n", ROUGE, RES);
                    }
                    break;
                case 3: { // Alerte stock faible pour un produit
                    char nom_produit[MAX_NAME_LENGTH];
                    printf("\t\t\tEntrez le nom du produit: ");
                    fgets(nom_produit, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_produit); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_produit); // Convertir en minuscule
                    Produit* produit = rechercher_produit_par_nom(*liste_categories, nom_produit);
                    if (produit) {
                        int seuil;
                        printf("\t\t\tEntrez le seuil pour le stock faible: ");
                        if (scanf("%d", &seuil) != 1) {
                            printf("\t\t\t%sErreur : Entree invalide pour le seuil.%s\n", ROUGE, RES);
                            while (getchar() != '\n'); // Nettoyage du buffer
                            break;
                        }
                        if (produit->quantities < seuil) {
                            printf("\t\t\tAlerte: Le produit '%s' a un stock faible (%d unites restantes).\n", produit->name, produit->quantities);
                        }
                        else {
                            printf("\t\t\tLe produit '%s' a un stock suffisant (%d unites).\n", produit->name, produit->quantities);
                        }
                    }
                    else {
                        printf("\t\t\t%sProduit non trouve.%s\n", ROUGE, RES);
                    }
                    break;
                }
                case 4: { // Rechercher un produit par nom
                    char nom_produit[MAX_NAME_LENGTH];
                    printf("\t\t\tEntrez le nom du produit a rechercher: ");
                    fgets(nom_produit, MAX_NAME_LENGTH, stdin);
                    nettoyer_chaine(nom_produit); // Nettoyer la chaîne
                    convertir_en_minuscule(nom_produit); // Convertir en minuscule
                    Produit* produit = rechercher_produit_par_nom(*liste_categories, nom_produit);
                    if (produit) {
                        printf("\t\t\tProduit trouve : %s, Prix: %.2f, Quantite: %d\n", produit->name, produit->price, produit->quantities);
                    }
                    else {
                        printf("\t\t\t%sProduit non trouve.%s\n", ROUGE, RES);
                    }
                    break;
                }
                case 5:
                    afficher_statistiques(*liste_categories);
                    break;
                default:
                    printf("\t\t\t%sOption invalide.%s\n", ROUGE, RES);
                    break;
                }
            } while (sous_choix != 0);
            break;

        case 5: // Afficher les alertes de stock
            system("cls"); // Effacer l'écran
            recenser_alertes(*liste_categories);
            break;

        case 6: // Quitter
            printf("\t\t\t\t%sAu revoir !%s\n", inverse, RES);
            break;

        default:
            printf("\t\t\t%sOption invalide.%s\n", ROUGE, RES);
            break;
        }
    } while (choix != 6);
}

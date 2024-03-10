
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <time.h>

#define LONGUEUR_MAX_LOGIN 40
#define LONGUEUR_MAX_MDP 40
#define NB_MAX_ETUDIANTS 100

typedef struct {
    int jours;
    int mois;
    int annee;
} DATE;

typedef struct {
    char login[LONGUEUR_MAX_LOGIN];
    char motDePasse[LONGUEUR_MAX_MDP];
} Identifiants;

typedef struct {
    char nom[25];
    char prenom[75];
    char matricule[5];
    DATE d_birth;
    int estPresent;
} ETUDIANT;

int verifierIdentifiants(Identifiants *identifiants, int nombreIdentifiants, char *login, char *motDePasse) {
    for (int i = 0; i < nombreIdentifiants; i++) {
        if (strcmp(identifiants[i].login, login) == 0 && strcmp(identifiants[i].motDePasse, motDePasse) == 0) {
            return 1; // Identifiants valides
        }
    }
    return 0; // Identifiants invalides
}

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void saisirMotDePasse(char *motDePasse, int tailleMax) {
    int i = 0;
    char c;
    printf("Mot de passe : ");
    while (i < tailleMax - 1) {
        c = getch(); // Récupère le caractère sans l'afficher
        if (c == '\r' || c == '\n') { // Si l'utilisateur appuie sur Entrée
            break;
        } else if (c == '\b' && i > 0) { // Si l'utilisateur appuie sur Retour Arrière
            printf("\b \b"); // Efface le caractère précédent de l'affichage
            i--;
        } else if (isprint(c)) { // Si c'est un caractère imprimable
            motDePasse[i++] = c;
            printf("*"); // Affiche un astérisque à la place du caractère saisi
        }
    }
    motDePasse[i] = '\0'; // Termine la chaîne de caractères
}

void enregistrerPresenceEtudiant(char *matricule, char *nom, char *date) {
    FILE *fichier = fopen("Etudiantspresnts.txt", "a");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier des présences étudiantes.\n");
        return;
    }

    fprintf(fichier, "%s\t%s\t%s\n", matricule, nom, date);
    fclose(fichier);
}

void enregistrerPresence(char *matricule) {
    FILE *fichier = fopen("info_marque.txt", "a"); // Utilisation de "a" pour ajouter à la fin du fichier
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de présence.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    char date[20];
    sprintf(date, "%02d/%02d/%04d", localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);
    fprintf(fichier, "%s, %s, %s\n", matricule, "Présent", date);
    fclose(fichier);

    // Enregistrement de la présence dans le fichier des étudiants présents
    enregistrerPresenceEtudiant(matricule, "Nom", date);
}

void afficherToutesPresences() {
    FILE *fichier = fopen("info_marque.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de présence.\n");
        return;
    }

    printf("Liste de toutes les présences :\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Prénom\t\tNom\t\tMatricule\tDate\n");
    printf("--------------------------------------------------------------------------\n");

    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        char matricule[5], presence[10], date[11];
        if (sscanf(ligne, "%4[^,], %9[^,], %10[^\n]", matricule, presence, date) == 3) {
            if (strcmp(matricule, "0000") == 0) {
                printf("Baba\t\tIssa\t\t0000\t%s\n", date);
            } else if (strcmp(matricule, "1111") == 0) {
                printf("Aminata\t\tNdiaye\t\t1111\t%s\n", date);
            } else {
                printf("Matricule inconnu\t%s\n", date);
            }
        } else {
            printf("Erreur de lecture de la ligne : %s\n", ligne);
        }
    }

    fclose(fichier);
}


void afficherPresenceParDate() {
    int jour, mois, annee;
    printf("Entrez une date (JJ/MM/AAAA) : ");
    scanf("%d/%d/%d", &jour, &mois, &annee);

    // Vérification de la validité de la date
    if (jour < 1 || jour > 31 || mois < 1 || mois > 12 || annee < 1900) {
        printf("Date invalide. Veuillez entrer une date valide.\n");
        return;
    }

    FILE *fichier = fopen("info_marque.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de présence.\n");
        return;
    }

    printf("Liste des présences pour la date %02d/%02d/%04d :\n", jour, mois, annee);
    printf("--------------------------------------------------------------------------\n");
    printf("Prénom\t\tNom\t\tMatricule\n");
    printf("--------------------------------------------------------------------------\n");

    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        char matricule[5], presence[10], date[11];
        if (sscanf(ligne, "%4[^,], %9[^,], %10[^\n]", matricule, presence, date) == 3) {
            int j, m, a;
            sscanf(date, "%d/%d/%d", &j, &m, &a);
            if (j == jour && m == mois && a == annee) {
                if (strcmp(matricule, "0000") == 0) {
                    printf("Baba\t\tIssa\t\t0000\n");
                } else if (strcmp(matricule, "1111") == 0) {
                    printf("Aminata\t\tNdiaye\t\t1111\n");
                } else {
                    printf("Matricule inconnu\n");
                }
            }
        } else {
            printf("Erreur de lecture de la ligne : %s\n", ligne);
        }
    }

    fclose(fichier);
}

int menuAdmin() {
    int choix = 0;
    do {
        printf("--------------------------------------------------------------------------\n");
        printf("Menu de l'administrateur :\n");
        printf("--------------------------------------------------------------------------\n");
        printf("1 ---------- Gestion des étudiants\n");
        printf("2 ---------- Génération de fichiers\n");
        printf("3 ---------- Marquer les présences\n");
        printf("4 ---------- Envoyer un message\n");
        printf("5 ---------- Quitter\n");
        printf("\nEntrez votre choix : ");
        scanf("%d", &choix);
        if (choix < 1 || choix > 5) {
            printf("Choix invalide. Veuillez entrer un choix entre 1 et 5.\n");
        }
    } while (choix < 1 || choix > 5);
    return choix;
}

int menuEtudiant() {
    int choix = 0;
    do {
        printf("--------------------------------------------------------------------------\n");
        printf("Menu de l'etudiant :\n");
        printf("--------------------------------------------------------------------------\n");
        printf("1 ---------- MARQUER MA PRÉSENCE\n");
        printf("2 ---------- NOMBRE DE MINUTES D’ABSENCE\n");
        printf("3 ---------- MES MESSAGES (0) \n");
        printf("4 ---------- Quitter\n");
        printf("\nEntrez votre choix : ");
        scanf("%d", &choix);
        if (choix < 1 || choix > 4) {
            printf("Choix invalide. Veuillez entrer un choix entre 1 et 4.\n");
        }
    } while (choix < 1 || choix > 4);
    return choix;
}

void marquerPresence(ETUDIANT *etudiants, int nbEtudiants, int isAdmin) {
    char saisieMatricule[5];
    int marque = 0;
    nbEtudiants = 2;
    char matriculesSaisis[NB_MAX_ETUDIANTS][5];
    int nbMatriculesSaisis = 0;

    if (isAdmin) {
        for (int i = 0; i < nbEtudiants; i++) {
            if (strcmp(etudiants[i].matricule, "0000") == 0 && etudiants[i].estPresent) {
                printf("Erreur : l'admin est déjà présent avec le matricule 0000.\n");
                return;
            }
        }
    }

    do {
        printf("Veuillez saisir le matricule de l'etudiant (ou 'q' pour quitter) : ");
        scanf("%4s", saisieMatricule);

        while (getchar() != '\n');

        if (strcmp(saisieMatricule, "q") == 0) {
            if (!isAdmin) {
                printf("Déconnexion...\n");
                return;
            } else {
                char saisieMotDePasse[LONGUEUR_MAX_MDP];
                printf("Mot de passe : ");
                saisirMotDePasse(saisieMotDePasse, LONGUEUR_MAX_MDP);
                if (strcmp(saisieMotDePasse, "admin") == 0) {
                    printf("Mot de passe correct. Retour au menu administrateur.\n");
                    printf("Déconnexion...\n");
                    return;
                } else {
                    printf("Mot de passe incorrect. Veuillez ressaisir le matricule.\n");
                    continue;
                }
            }
        }

        int matriculeDejaSaisi = 0;
        for (int i = 0; i < nbMatriculesSaisis; i++) {
            if (strcmp(saisieMatricule, matriculesSaisis[i]) == 0) {
                matriculeDejaSaisi = 1;
                break;
            }
        }

        if (matriculeDejaSaisi) {
            printf("Erreur : le matricule a déjà été saisi. Veuillez saisir un matricule différent.\n");
            continue;
        }

        for (int i = 0; i < nbEtudiants; i++) {
            if (strcmp(saisieMatricule, etudiants[i].matricule) == 0) {
                marque = 1;
                enregistrerPresence(saisieMatricule);
                time_t now = time(NULL);
                struct tm *localTime = localtime(&now);
                printf("Présence enregistrée pour l'étudiant de matricule %s à %02d:%02d:%02d.\n", saisieMatricule, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
                strcpy(matriculesSaisis[nbMatriculesSaisis], saisieMatricule);
                nbMatriculesSaisis++;
                printf("Tapez espace pour continuer...\n");
                while (getch() != ' ') {}
                break;
            }
        }

        if (!marque) {
            printf("Erreur : matricule invalide. Veuillez saisir un matricule valide.\n");
        }
    } while (1);
}

void afficherMessages() {
    // Ajoutez votre code pour afficher les messages ici
}

void marquerPresence1(ETUDIANT *etudiants, int nbEtudiants, int isAdmin) {
    char choix;
    time_t now;
    struct tm *localTime;
    static int estPresent = 0;

    if (estPresent) {
        printf("Vous êtes déjà enregistré.\n");
        return;
    }

    printf("Veuillez confirmer votre présence ? (o/n) : ");
    scanf(" %c", &choix);

    if (choix == 'o' || choix == 'O') {
        now = time(NULL);
        localTime = localtime(&now);
        printf("Présence confirmée à %02d:%02d:%02d.\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
        if (!isAdmin) {
            for (int i = 0; i < nbEtudiants; i++) {
                if (strlen(etudiants[i].matricule) == 0) {
                    strcpy(etudiants[i].matricule, "0000");
                    etudiants[i].estPresent = 1;
                    printf("Vous êtes maintenant lié au matricule 0000.\n");
                    break;
                }
            }
        }
        estPresent = 1;
    } else {
        printf("Retour au menu étudiant.\n");
        return;
    }
}

int main() {
    ETUDIANT etudiants[NB_MAX_ETUDIANTS];
    int nbEtudiants = 0;

    strcpy(etudiants[0].matricule, "0000");
    strcpy(etudiants[1].matricule, "1111");

    FILE *fichierAdmin = fopen("admin.txt", "r+");
    FILE *fichierEtudiant = fopen("etudiant.txt", "r+");

    if (fichierAdmin == NULL || fichierEtudiant == NULL) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return 1;
    }

    Identifiants identifiantsAdmin[NB_MAX_ETUDIANTS];
    Identifiants identifiantsEtudiant[NB_MAX_ETUDIANTS];
    int nombreIdentifiantsAdmin = 0;
    int nombreIdentifiantsEtudiant = 0;

    while (fscanf(fichierAdmin, "%s %s", identifiantsAdmin[nombreIdentifiantsAdmin].login, identifiantsAdmin[nombreIdentifiantsAdmin].motDePasse) == 2) {
        nombreIdentifiantsAdmin++;
    }
    fclose(fichierAdmin);

    while (fscanf(fichierEtudiant, "%s %s", identifiantsEtudiant[nombreIdentifiantsEtudiant].login, identifiantsEtudiant[nombreIdentifiantsEtudiant].motDePasse) == 2) {
        nombreIdentifiantsEtudiant++;
    }
    fclose(fichierEtudiant);

    char saisieLogin[LONGUEUR_MAX_LOGIN];
    char saisieMotDePasse[LONGUEUR_MAX_MDP];
    int isAdmin = 0;

    while (1) {
        do {
            printf("---------------- Connexion ----------------\n\n");
            printf("Login : ");

            fgets(saisieLogin, LONGUEUR_MAX_LOGIN, stdin);
            saisieLogin[strcspn(saisieLogin, "\n")] = 0;
            if (strlen(saisieLogin) == 0) {
                printf("\nVous avez laissé le champ vide. Veuillez rentrer votre login.\n");
                continue;
            }

            saisirMotDePasse(saisieMotDePasse, LONGUEUR_MAX_MDP);
            if (strlen(saisieMotDePasse) == 0) {
                printf("\nVous avez laissé le champ vide. Veuillez entrer votre mot de passe.\n");
                continue;
            }

            if (verifierIdentifiants(identifiantsAdmin, nombreIdentifiantsAdmin, saisieLogin, saisieMotDePasse)) {
                isAdmin = 1;
                break;
            } else if (verifierIdentifiants(identifiantsEtudiant, nombreIdentifiantsEtudiant, saisieLogin, saisieMotDePasse)) {
                isAdmin = 0;
                break;
            } else {
                printf("Login ou mot de passe invalides. Veuillez réessayer.\n");
            }

        } while (1);

        if (isAdmin) {
            int choix;
            do {
                choix = menuAdmin();
                switch (choix) {
                    case 1:
                        // Code pour la gestion des étudiants
                        break;
                    case 2:
                        int sousChoix;
                        do {
                            printf("--------------------------------------------------------------------------\n");
                            printf("Sous-menu de génération de fichiers :\n");
                            printf("--------------------------------------------------------------------------\n");
                            printf("1 ---------- Afficher toutes les présences\n");
                            printf("2 ---------- Afficher les présences par date\n");
                            printf("3 ---------- Retour\n");
                            printf("\nEntrez votre choix : ");
                            scanf("%d", &sousChoix);
                            if (sousChoix < 1 || sousChoix > 3) {
                                printf("Choix invalide. Veuillez entrer un choix entre 1 et 3.\n");
                            } else {
                                switch (sousChoix) {
                                    case 1:
                                        afficherToutesPresences();
                                        break;
                                    case 2:
                                        afficherPresenceParDate();
                                        break;
                                    case 3:
                                        printf("Retour au menu principal.\n");
                                        break;
                                }
                            }
                        } while (sousChoix != 3);
                        break;
                    case 3:
                        marquerPresence(etudiants, nbEtudiants, isAdmin);
                        break;
                    case 4:
                        // Code pour envoyer un message
                        break;
                    case 5:
                        printf("Déconnexion...\n");
                        break;
                }
            } while (choix != 5);
        } else {
            int choix;
            do {
                choix = menuEtudiant();
                switch (choix) {
                    case 1:
                        marquerPresence1(etudiants, nbEtudiants, isAdmin);
                        break;
                    case 2:
                        // Code pour afficher le nombre de minutes d'absence
                        break;
                    case 3:
                        afficherMessages();
                        break;
                    case 4:
                        printf("Déconnexion...\n");
                        break;
                }
            } while (choix != 4);
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "api_client.h"
#include "comment_generator.h"
#include "harassment_detector.h"
#include "stats.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_INPUT_LENGTH 1024

static void setup_console(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
}

static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void print_banner(void) {
    printf("\n");
    printf("==================================================================\n");
    printf("         DETECTEUR DE CYBERHARCELEMENT v2.0                       \n");
    printf("       Analyse & Prevention par Intelligence Artificielle         \n");
    printf("==================================================================\n");
    printf("\n");
}

static void print_menu(void) {
    printf("------------------------------------------\n");
    printf("              MENU PRINCIPAL              \n");
    printf("------------------------------------------\n");
    printf("  [1] Generation automatique (IA)         \n");
    printf("  [2] Analyser un commentaire recu        \n");
    printf("  [3] Verifier avant de publier           \n");
    printf("  [4] Mode conversation (multi-analyse)   \n");
    printf("  [5] Ressources d'aide                   \n");
    printf("  [6] Quitter                             \n");
    printf("------------------------------------------\n");
    printf("\nVotre choix : ");
}

static void print_help_resources(void) {
    printf("\n");
    printf("==================================================================\n");
    printf("              RESSOURCES D'AIDE ET DE PREVENTION                  \n");
    printf("==================================================================\n");
    printf("\n");
    printf("Si vous etes VICTIME de cyberharcelement :\n");
    printf("------------------------------------------\n");
    printf("  - 3018 : Numero national contre le cyberharcelement (gratuit)\n");
    printf("  - https://www.netecoute.fr : Ecoute et conseils\n");
    printf("  - https://www.e-enfance.org : Protection des mineurs\n");
    printf("  - https://www.internet-signalement.gouv.fr : Signalement officiel\n");
    printf("\n");
    printf("Si vous avez des PENSEES SOMBRES :\n");
    printf("------------------------------------------\n");
    printf("  - 3114 : Numero national de prevention du suicide (24h/24)\n");
    printf("  - https://www.sos-amitie.com : Ecoute 24h/24\n");
    printf("  - https://www.filsantejeunes.com : Pour les 12-25 ans\n");
    printf("\n");
    printf("Pour SIGNALER un contenu :\n");
    printf("------------------------------------------\n");
    printf("  - Utilisez les boutons de signalement des plateformes\n");
    printf("  - Faites des captures d'ecran comme preuves\n");
    printf("  - Contactez la police si menaces graves\n");
    printf("\n");
    printf("Pour COMPRENDRE et S'AMELIORER :\n");
    printf("------------------------------------------\n");
    printf("  - https://www.nonauharcelement.education.gouv.fr\n");
    printf("  - Reflechissez avant de poster : \"Est-ce que je le dirais en face ?\"\n");
    printf("\n");
}

static void print_victim_resources(int score) {
    if (score >= 60) {
        printf("\n");
        printf("------------------------------------------\n");
        printf("  RESSOURCES SI VOUS ETES CONCERNE(E)     \n");
        printf("------------------------------------------\n");
        
        if (score >= 80) {
            printf("  Ce commentaire est GRAVE.\n");
            printf("  -> Appelez le 3018 (gratuit) si vous etes victime\n");
            printf("  -> Signalez sur internet-signalement.gouv.fr\n");
            printf("  -> Gardez des preuves (captures d'ecran)\n");
        } else {
            printf("  Ce commentaire est blessant.\n");
            printf("  -> N'hesitez pas a en parler : netecoute.fr\n");
            printf("  -> Vous pouvez bloquer/signaler l'auteur\n");
        }
        printf("------------------------------------------\n");
    }
}

static void print_author_warning(int score, const char *categorie) {
    printf("\n");
    printf("##################################################\n");
    printf("#           VERIFICATION AVANT PUBLICATION       #\n");
    printf("##################################################\n");
    printf("\n");
    
    if (score <= 20) {
        printf("[OK] Ce commentaire semble correct.\n");
        printf("     Vous pouvez le publier.\n");
    } else if (score <= 40) {
        printf("[!] Ce commentaire est un peu rude.\n");
        printf("    Conseil : Reformulez pour etre plus constructif.\n");
        printf("    Question : Le diriez-vous en face a cette personne ?\n");
    } else if (score <= 60) {
        printf("[!!] ATTENTION : Ce commentaire contient des elements problematiques.\n");
        printf("     Categorie detectee : %s\n", categorie);
        printf("\n");
        printf("     -> Ce type de message peut BLESSER quelqu'un.\n");
        printf("     -> Il pourrait etre SIGNALE et SUPPRIME.\n");
        printf("     -> Vous pourriez etre SANCTIONNE par la plateforme.\n");
        printf("\n");
        printf("     CONSEIL : Prenez du recul, reformulez calmement.\n");
    } else if (score <= 80) {
        printf("[XXX] STOP ! Ce commentaire est une ATTAQUE.\n");
        printf("      Categorie : %s (score: %d/100)\n", categorie, score);
        printf("\n");
        printf("      -> Ceci peut constituer du HARCELEMENT.\n");
        printf("      -> La victime pourrait PORTER PLAINTE.\n");
        printf("      -> Vous risquez des POURSUITES JUDICIAIRES.\n");
        printf("\n");
        printf("      NE PUBLIEZ PAS CE MESSAGE.\n");
        printf("      Respirez. Fermez l'ecran. Revenez plus tard.\n");
    } else {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("!         DANGER - MESSAGE TRES GRAVE            !\n");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("\n");
        printf("      Categorie : %s (score: %d/100)\n", categorie, score);
        printf("\n");
        printf("      -> Ce message peut detruire quelqu'un.\n");
        printf("      -> Le cyberharcelement a des CONSEQUENCES REELLES.\n");
        printf("      -> Des personnes se sont SUICIDEES a cause de ca.\n");
        printf("\n");
        printf("      CE MESSAGE NE DOIT JAMAIS ETRE ENVOYE.\n");
        printf("\n");
        printf("      Si vous ressentez de la colere ou de la haine,\n");
        printf("      parlez-en : https://www.filsantejeunes.com\n");
    }
    printf("\n");
    printf("##################################################\n");
}

static void print_result_analysis(const AnalysisResult *result) {
    printf("\n--- RESULTAT DE L'ANALYSE ---\n");
    printf("| Score     : %d/100\n", result->score);
    printf("| Categorie : %s\n", result->categorie);
    printf("|-----------------------------\n");
    printf("| Explication :\n");
    printf("| %s\n", result->explication);
    printf("-----------------------------\n");
    
    if (result->score <= 20) {
        printf("[OK] Ce commentaire semble sain.\n");
    } else if (result->score <= 40) {
        printf("[!] Ce commentaire est un peu rude mais acceptable.\n");
    } else if (result->score <= 60) {
        printf("[!!] Ce commentaire contient des insultes.\n");
    } else if (result->score <= 80) {
        printf("[XXX] Ce commentaire est une attaque personnelle.\n");
    } else {
        printf("[DANGER] Ce commentaire est TRES GRAVE.\n");
    }
    
    print_victim_resources(result->score);
}

static void mode_generation_auto(void) {
    char theme[256];
    int count;
    
    printf("\n=== MODE GENERATION AUTOMATIQUE ===\n\n");
    
    printf("Theme des commentaires (ex: jeux video, politique, musique) : ");
    if (fgets(theme, sizeof(theme), stdin) == NULL) return;
    theme[strcspn(theme, "\n")] = 0;
    
    if (strlen(theme) == 0) {
        strcpy(theme, "reseaux sociaux");
    }
    
    printf("Nombre de commentaires a generer (1-20) : ");
    if (scanf("%d", &count) != 1) {
        count = 5;
    }
    clear_input_buffer();
    
    if (count < 1) count = 1;
    if (count > 20) count = 20;
    
    printf("\n[Generation de %d commentaires sur \"%s\"...]\n", count, theme);
    printf("[Cela peut prendre quelques secondes...]\n\n");
    
    CommentList comments;
    if (generate_comments(theme, count, &comments) != 0) {
        printf("Erreur lors de la generation des commentaires.\n");
        printf("Verifiez votre cle API et votre connexion internet.\n");
        return;
    }
    
    if (comments.count == 0) {
        printf("Aucun commentaire genere.\n");
        return;
    }
    
    printf("=== %d COMMENTAIRES GENERES ===\n\n", comments.count);
    
    Statistics stats;
    stats_init(&stats);
    
    for (int i = 0; i < comments.count; i++) {
        printf("========================================\n");
        printf("Commentaire #%d\n", i + 1);
        printf("Texte : \"%s\"\n", comments.comments[i].texte);
        
        AnalysisResult result;
        if (analyze_comment(comments.comments[i].texte, &result) == 0) {
            printf("Score : %d | Categorie : %s\n", result.score, result.categorie);
            printf("-> %s\n", result.explication);
            stats_add_result(&stats, &result, i);
        } else {
            printf("Erreur lors de l'analyse.\n");
        }
        printf("\n");
    }
    
    stats_print(&stats);
}

static void mode_analyse_recu(void) {
    char comment[MAX_INPUT_LENGTH];
    
    printf("\n=== ANALYSER UN COMMENTAIRE RECU ===\n");
    printf("(Pour verifier si un message que vous avez recu est problematique)\n\n");
    printf("Collez le commentaire a analyser :\n> ");
    
    if (fgets(comment, sizeof(comment), stdin) == NULL) return;
    comment[strcspn(comment, "\n")] = 0;
    
    if (strlen(comment) == 0) {
        printf("Commentaire vide, abandon.\n");
        return;
    }
    
    printf("\n[Analyse en cours...]\n");
    
    AnalysisResult result;
    if (analyze_comment(comment, &result) == 0) {
        print_result_analysis(&result);
    } else {
        printf("Erreur lors de l'analyse.\n");
    }
}

static void mode_avant_publication(void) {
    char comment[MAX_INPUT_LENGTH];
    
    printf("\n=== VERIFIER AVANT DE PUBLIER ===\n");
    printf("(Testez votre message AVANT de l'envoyer)\n\n");
    printf("Ecrivez votre commentaire :\n> ");
    
    if (fgets(comment, sizeof(comment), stdin) == NULL) return;
    comment[strcspn(comment, "\n")] = 0;
    
    if (strlen(comment) == 0) {
        printf("Commentaire vide, abandon.\n");
        return;
    }
    
    printf("\n[Verification en cours...]\n");
    
    AnalysisResult result;
    if (analyze_comment(comment, &result) == 0) {
        print_author_warning(result.score, result.categorie);
        
        if (result.score > 40) {
            printf("\nVoulez-vous des suggestions pour reformuler ? (o/n) : ");
            char choice;
            if (scanf(" %c", &choice) == 1 && (choice == 'o' || choice == 'O')) {
                clear_input_buffer();
                printf("\nConseils pour reformuler :\n");
                printf("- Exprimez votre desaccord sans attaquer la personne\n");
                printf("- Utilisez \"je pense que...\" plutot que \"tu es...\"\n");
                printf("- Critiquez les idees, pas les individus\n");
                printf("- Si vous etes en colere, attendez avant de repondre\n");
            } else {
                clear_input_buffer();
            }
        }
    } else {
        printf("Erreur lors de l'analyse.\n");
    }
}

static void mode_conversation(void) {
    char comment[MAX_INPUT_LENGTH];
    Statistics stats;
    stats_init(&stats);
    int comment_num = 0;
    
    printf("\n=== MODE CONVERSATION ===\n");
    printf("Analysez plusieurs commentaires a la suite.\n");
    printf("Tapez 'quit' ou 'q' pour quitter et voir les statistiques.\n\n");
    
    while (1) {
        printf("----------------------------------------\n");
        printf("Commentaire #%d (ou 'q' pour quitter) :\n> ", comment_num + 1);
        
        if (fgets(comment, sizeof(comment), stdin) == NULL) break;
        comment[strcspn(comment, "\n")] = 0;
        
        if (strcmp(comment, "quit") == 0 || strcmp(comment, "q") == 0 || 
            strcmp(comment, "exit") == 0) {
            break;
        }
        
        if (strlen(comment) == 0) {
            printf("Commentaire vide, reessayez.\n");
            continue;
        }
        
        printf("[Analyse...]\n");
        
        AnalysisResult result;
        if (analyze_comment(comment, &result) == 0) {
            comment_num++;
            printf("-> Score: %d | %s\n", result.score, result.categorie);
            printf("   %s\n", result.explication);
            stats_add_result(&stats, &result, comment_num - 1);
            
            if (result.score >= 80) {
                printf("   [!] Commentaire grave - Ressources : 3018 / netecoute.fr\n");
            }
        } else {
            printf("Erreur lors de l'analyse.\n");
        }
        printf("\n");
    }
    
    if (stats.total_comments > 0) {
        printf("\n");
        stats_print(&stats);
    } else {
        printf("\nAucun commentaire analyse.\n");
    }
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    setup_console();
    
    if (api_init() != 0) {
        fprintf(stderr, "Erreur: Impossible d'initialiser le client API.\n");
        fprintf(stderr, "Verifiez que vous avez une connexion internet.\n");
        return 1;
    }
    
    print_banner();
    
    int running = 1;
    while (running) {
        print_menu();
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("\nChoix invalide. Entrez un numero entre 1 et 6.\n\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                mode_generation_auto();
                break;
            case 2:
                mode_analyse_recu();
                break;
            case 3:
                mode_avant_publication();
                break;
            case 4:
                mode_conversation();
                break;
            case 5:
                print_help_resources();
                break;
            case 6:
                running = 0;
                printf("\n------------------------------------------\n");
                printf("Merci d'avoir utilise ce detecteur.\n");
                printf("Ensemble, luttons contre le cyberharcelement.\n");
                printf("------------------------------------------\n\n");
                break;
            default:
                printf("\nChoix invalide. Entrez un numero entre 1 et 6.\n\n");
                break;
        }
        
        if (running && choice >= 1 && choice <= 5) {
            printf("\nAppuyez sur Entree pour continuer...");
            getchar();
            printf("\n");
        }
    }
    
    api_cleanup();
    return 0;
}

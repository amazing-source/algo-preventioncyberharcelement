#include "stats.h"
#include <stdio.h>
#include <string.h>

static const char *CATEGORY_NAMES[NUM_CATEGORIES] = {
    "sain",
    "rude",
    "insulte",
    "humiliation",
    "body-shaming",
    "discrimination",
    "menace",
    "harcelement"
};

void stats_init(Statistics *stats) {
    stats->total_comments = 0;
    stats->average_score = 0.0;
    stats->most_toxic_index = -1;
    stats->most_toxic_score = -1;

    for (int i = 0; i < NUM_CATEGORIES; i++) {
        stats->category_counts[i] = 0;
    }
}

int stats_get_category_index(const char *category) {
    if (!category) return 0;

    // Correspondances exactes d'abord
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        if (strcmp(category, CATEGORY_NAMES[i]) == 0) {
            return i;
        }
    }

    // Correspondances flexibles (l'IA peut varier les termes)
    // Index 0 = sain
    if (strstr(category, "sain") || strstr(category, "amical") ||
        strstr(category, "neutre") || strstr(category, "positif") ||
        strstr(category, "bienveillant")) {
        return 0;
    }

    // Index 1 = rude
    if (strstr(category, "rude") || strstr(category, "desagreable") ||
        strstr(category, "agressif") || strstr(category, "moqueur")) {
        return 1;
    }

    // Index 2 = problematique
    if (strstr(category, "problematique") || strstr(category, "moquerie") ||
        strstr(category, "ironique") || strstr(category, "sarcastique")) {
        return 2;
    }

    // Index 3 = insulte
    if (strstr(category, "insulte") || strstr(category, "insultant") ||
        strstr(category, "offensant") || strstr(category, "vulgaire")) {
        return 3;
    }

    // Index 4 = humiliation
    if (strstr(category, "humiliation") || strstr(category, "humiliant") ||
        strstr(category, "attaque") || strstr(category, "rabaissant") ||
        strstr(category, "body") || strstr(category, "physique")) {
        return 4;
    }

    // Index 5 = discrimination
    if (strstr(category, "discrimination") || strstr(category, "discriminatoire") ||
        strstr(category, "racis") || strstr(category, "sexis") ||
        strstr(category, "homophob") || strstr(category, "haine") ||
        strstr(category, "haineux")) {
        return 5;
    }

    // Index 6 = menace
    if (strstr(category, "menace") || strstr(category, "mena") ||
        strstr(category, "danger") || strstr(category, "violence") ||
        strstr(category, "violent")) {
        return 6;
    }

    // Index 7 = harcelement
    if (strstr(category, "harcelement") || strstr(category, "harc") ||
        strstr(category, "repete") || strstr(category, "acharnement")) {
        return 7;
    }

    // Par défaut, utiliser le score pour deviner
    return 0;
}

const char* stats_get_category_name(int index) {
    if (index >= 0 && index < NUM_CATEGORIES) {
        return CATEGORY_NAMES[index];
    }
    return "inconnu";
}

void stats_add_result(Statistics *stats, const AnalysisResult *result, int index) {
    double old_total = stats->average_score * stats->total_comments;
    stats->total_comments++;
    stats->average_score = (old_total + result->score) / stats->total_comments;

    // D'abord essayer par catégorie
    int cat_index = stats_get_category_index(result->categorie);

    // Si catégorie = sain mais score élevé, corriger avec le score
    if (cat_index == 0 && result->score > 20) {
        if (result->score <= 40) {
            cat_index = 1; // rude
        } else if (result->score <= 60) {
            cat_index = 3; // insulte
        } else if (result->score <= 80) {
            cat_index = 4; // humiliation
        } else {
            cat_index = 6; // menace
        }
    }

    stats->category_counts[cat_index]++;

    if (result->score > stats->most_toxic_score) {
        stats->most_toxic_score = result->score;
        stats->most_toxic_index = index;
    }
}

void stats_print(const Statistics *stats) {
    printf("\n=== STATISTIQUES FINALES ===\n");
    printf("Total commentaires analyses : %d\n", stats->total_comments);
    printf("Score moyen de toxicite : %.1f\n\n", stats->average_score);

    printf("Repartition par categorie :\n");
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        if (stats->category_counts[i] > 0) {
            double pct = (100.0 * stats->category_counts[i]) / stats->total_comments;
            printf("  - %s : %d (%.1f%%)\n",
                   CATEGORY_NAMES[i],
                   stats->category_counts[i],
                   pct);
        }
    }

    if (stats->most_toxic_index >= 0) {
        printf("\nCommentaire le plus toxique : #%d (score: %d)\n",
               stats->most_toxic_index + 1,
               stats->most_toxic_score);
    }
}

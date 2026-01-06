#include "harassment_detector.h"
#include "api_client.h"
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *ANALYZER_PROMPT_TEMPLATE =
    "Tu es un detecteur de cyberharcelement base sur des criteres contextuels "
    "(similaires a TikTok, Reddit et Google Perspective API). "
    "Analyse le commentaire suivant : \"%s\". "
    "NE TE BASE JAMAIS sur les mots isoles. "
    "Le mot \"gros\" par exemple peut etre amical (\"ca va gros ?\") ou insultant "
    "(\"sale gros va manger\"). "
    "Evalue les elements suivants : "
    "- Ton general : amical / neutre / moqueur / agressif / haineux "
    "- Intention : humoristique, affective, critique, insultante, humiliante "
    "- Structure : attaque directe (\"t'es...\"), presence d'imperatifs offensifs "
    "- Cible : une personne specifique ou non "
    "- Intensite : legere, moderee, forte "
    "- Contexte linguistique : modificateurs (\"sale\", \"espece de\") = aggravants "
    "Grille de scoring contextuelle : "
    "0-20 : sain / amical / neutre, "
    "20-40 : rude / desagreable mais non insultant, "
    "40-60 : insulte legere / moquerie isolee, "
    "60-80 : attaque personnelle claire / humiliation, "
    "80-100 : haine, body-shaming agressif, menace, discrimination. "
    "ATTENTION : "
    "Un mot insultant seul => score modere (40-60). "
    "Une combinaison insultante + attaque sur la personne => score eleve (60+). "
    "Le body-shaming agressif => 80+. "
    "Reponds uniquement en JSON : "
    "{\"score\": ..., \"categorie\": \"...\", \"explication\": \"...\"}";

static char* extract_text_from_response(const char *api_response) {
    struct json_object *root = json_tokener_parse(api_response);
    if (!root) return NULL;

    struct json_object *choices_array;
    if (!json_object_object_get_ex(root, "choices", &choices_array)) {
        json_object_put(root);
        return NULL;
    }

    if (json_object_array_length(choices_array) == 0) {
        json_object_put(root);
        return NULL;
    }

    struct json_object *first_choice = json_object_array_get_idx(choices_array, 0);
    struct json_object *message_obj;
    if (!json_object_object_get_ex(first_choice, "message", &message_obj)) {
        json_object_put(root);
        return NULL;
    }

    struct json_object *content_obj;
    if (!json_object_object_get_ex(message_obj, "content", &content_obj)) {
        json_object_put(root);
        return NULL;
    }

    const char *text = json_object_get_string(content_obj);
    char *result = strdup(text);

    json_object_put(root);
    return result;
}

static int parse_analysis_json(const char *json_str, AnalysisResult *result) {
    const char *start = strchr(json_str, '{');
    if (!start) return -1;

    struct json_object *root = json_tokener_parse(start);
    if (!root) return -1;

    struct json_object *score_obj, *cat_obj, *expl_obj;

    if (json_object_object_get_ex(root, "score", &score_obj)) {
        result->score = json_object_get_int(score_obj);
    } else {
        result->score = -1;
    }

    if (json_object_object_get_ex(root, "categorie", &cat_obj)) {
        strncpy(result->categorie, json_object_get_string(cat_obj), 63);
        result->categorie[63] = '\0';
    } else {
        strcpy(result->categorie, "inconnu");
    }

    if (json_object_object_get_ex(root, "explication", &expl_obj)) {
        strncpy(result->explication, json_object_get_string(expl_obj), MAX_EXPLANATION_LENGTH - 1);
        result->explication[MAX_EXPLANATION_LENGTH - 1] = '\0';
    } else {
        strcpy(result->explication, "Aucune explication fournie.");
    }

    json_object_put(root);
    return 0;
}

int analyze_comment(const char *comment, AnalysisResult *result) {
    if (!comment || !result) {
        return -1;
    }

    size_t prompt_size = strlen(ANALYZER_PROMPT_TEMPLATE) + strlen(comment) + 64;
    char *prompt = malloc(prompt_size);
    if (!prompt) return -1;

    snprintf(prompt, prompt_size, ANALYZER_PROMPT_TEMPLATE, comment);

    char *api_response = api_send_request(prompt);
    free(prompt);

    if (!api_response) {
        return -1;
    }

    char *json_content = extract_text_from_response(api_response);
    api_free_response(api_response);

    if (!json_content) {
        return -1;
    }

    int parse_result = parse_analysis_json(json_content, result);
    free(json_content);

    return parse_result;
}

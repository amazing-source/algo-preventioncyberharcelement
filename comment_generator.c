#include "comment_generator.h"
#include "api_client.h"
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *GENERATOR_PROMPT_TEMPLATE =
    "Tu es un generateur de commentaires de reseaux sociaux ULTRA REALISTES. "
    "Genere %d commentaires sur : \"%s\". "
    "REGLES STRICTES pour etre realiste : "
    "- Certains commentaires ont des fautes d'orthographe volontaires (sa au lieu de ca, ses au lieu de c'est, etc) "
    "- Certains n'ont pas de majuscules du tout "
    "- Certains utilisent des abreviations (tkt, mdr, ptdr, jsp, chui, pr, tt, etc) "
    "- Certains sont tres courts (1-5 mots) genre 'nul' ou 'trop bien' "
    "- Certains ont des emojis ou des ... ou des ??? ou !!! "
    "- Melange : positif, neutre, moqueur, agressif, troll, haineux "
    "- Style VRAI Twitter/TikTok/YouTube francais, PAS un style de robot "
    "- Evite les phrases trop bien construites, les vrais gens ecrivent mal "
    "Reponds UNIQUEMENT en JSON : "
    "{\"commentaires\": [{\"id\": 1, \"texte\": \"...\"}, {\"id\": 2, \"texte\": \"...\"}]} "
    "Genere exactement %d commentaires.";

static char* extract_json_from_response(const char *api_response) {
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

static int parse_comments_json(const char *json_str, CommentList *list) {
    const char *start = strchr(json_str, '{');
    if (!start) return -1;

    struct json_object *root = json_tokener_parse(start);
    if (!root) return -1;

    struct json_object *commentaires;
    if (!json_object_object_get_ex(root, "commentaires", &commentaires)) {
        json_object_put(root);
        return -1;
    }

    int len = json_object_array_length(commentaires);
    list->count = 0;

    for (int i = 0; i < len && i < MAX_COMMENTS; i++) {
        struct json_object *item = json_object_array_get_idx(commentaires, i);

        struct json_object *id_obj, *texte_obj;
        if (json_object_object_get_ex(item, "id", &id_obj) &&
            json_object_object_get_ex(item, "texte", &texte_obj)) {

            list->comments[list->count].id = json_object_get_int(id_obj);
            strncpy(list->comments[list->count].texte,
                    json_object_get_string(texte_obj),
                    MAX_COMMENT_LENGTH - 1);
            list->comments[list->count].texte[MAX_COMMENT_LENGTH - 1] = '\0';
            list->count++;
        }
    }

    json_object_put(root);
    return 0;
}

int generate_comments(const char *theme, int count, CommentList *list) {
    if (!theme || !list || count <= 0 || count > MAX_COMMENTS) {
        return -1;
    }

    size_t prompt_size = strlen(GENERATOR_PROMPT_TEMPLATE) + strlen(theme) + 64;
    char *prompt = malloc(prompt_size);
    if (!prompt) return -1;

    snprintf(prompt, prompt_size, GENERATOR_PROMPT_TEMPLATE, count, theme, count);

    char *api_response = api_send_request(prompt);
    free(prompt);

    if (!api_response) {
        return -1;
    }

    char *json_content = extract_json_from_response(api_response);
    api_free_response(api_response);

    if (!json_content) {
        return -1;
    }

    int result = parse_comments_json(json_content, list);
    free(json_content);

    return result;
}

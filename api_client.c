#include "api_client.h"
#include "config.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static CURL *curl = NULL;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    ResponseBuffer *mem = (ResponseBuffer *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

int api_init(void) {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    return (curl != NULL) ? 0 : -1;
}

void api_cleanup(void) {
    if (curl) {
        curl_easy_cleanup(curl);
        curl = NULL;
    }
    curl_global_cleanup();
}

static char* escape_json_string(const char *str) {
    size_t len = strlen(str);
    char *escaped = malloc(len * 2 + 1);
    if (!escaped) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
            case '"':  escaped[j++] = '\\'; escaped[j++] = '"'; break;
            case '\\': escaped[j++] = '\\'; escaped[j++] = '\\'; break;
            case '\n': escaped[j++] = '\\'; escaped[j++] = 'n'; break;
            case '\r': escaped[j++] = '\\'; escaped[j++] = 'r'; break;
            case '\t': escaped[j++] = '\\'; escaped[j++] = 't'; break;
            default:   escaped[j++] = str[i]; break;
        }
    }
    escaped[j] = '\0';
    return escaped;
}

char* api_send_request(const char *prompt) {
    if (!curl) return NULL;

    ResponseBuffer response = {0};
    response.data = malloc(1);
    if (!response.data) return NULL;
    response.data[0] = '\0';

    char *escaped_prompt = escape_json_string(prompt);
    if (!escaped_prompt) {
        free(response.data);
        return NULL;
    }

    size_t json_size = strlen(escaped_prompt) + 512;
    char *json_body = malloc(json_size);
    if (!json_body) {
        free(escaped_prompt);
        free(response.data);
        return NULL;
    }

    snprintf(json_body, json_size,
        "{"
        "\"model\": \"%s\","
        "\"max_tokens\": %d,"
        "\"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]"
        "}",
        MODEL, MAX_TOKENS, escaped_prompt);

    free(escaped_prompt);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", API_KEY);
    headers = curl_slist_append(headers, auth_header);

    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    free(json_body);

    if (res != CURLE_OK) {
        free(response.data);
        return NULL;
    }

    return response.data;
}

void api_free_response(char *response) {
    if (response) {
        free(response);
    }
}

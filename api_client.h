#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t size;
} ResponseBuffer;

int api_init(void);
void api_cleanup(void);
char* api_send_request(const char *prompt);
void api_free_response(char *response);

#endif

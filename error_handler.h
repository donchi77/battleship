#ifndef ERR_HANDLER_H
    #define ERR_HANDLER_H
#define MAX_CHAR 1024

void fatal(char *message) {
    char error_message[MAX_CHAR];

    strcpy(error_message, "[ERROR] ");
    strncat(error_message, message, strlen(message));
    perror(error_message);
    exit(EXIT_FAILURE);
}

void debug(char *message) {
    char debug_message[MAX_CHAR];

    strcpy(debug_message, "[DEBUG] ");
    strncat(debug_message, message, strlen(message));
    printf(debug_message);
}

void *errorchecked_malloc(unsigned int size) {
    void *ptr;
    ptr = malloc(size);

    if (ptr == NULL)
        fatal("Malloc failed.\n");
    
    return ptr;
}

#endif
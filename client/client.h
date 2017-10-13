#pragma once

#include <pthread.h>
#include <openssl/ssl.h>
#include <termios.h>

struct Client{
    int socket;
    char* username;
    pthread_t thread;
    SSL_CTX* ctx;
    SSL* ssl;
    const char* certificate;
    struct termios backup;
    size_t message_len;
    size_t cursor_pos;
    char* message_read;
};

void print_to_terminal(struct Client* client, const char* message, size_t message_len);
int read_from_terminal(struct Client* client, char** message);

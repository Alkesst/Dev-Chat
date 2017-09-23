#pragma once

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <openssl/ssl.h>


struct User{
    char* username;
    size_t length;
    int id;
    int connection;
    SSL* ssl;
};


struct Server{
    struct User* users;
    size_t length;   // Elementos actuales dentro de la lista;
    size_t capacity; // cantidad de elementos que caben en el array
    int server;
    pthread_t thread;
    pthread_mutex_t sync;
    SSL_CTX* ssl_context;
};

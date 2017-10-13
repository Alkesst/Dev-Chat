/*
 * Dev-Chat A chat made by developers, for developers.
 * Copyright (C) 2017  Alejandro Garau Madrigal & Melchor Garau Madrigal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
    const char* private_path;
    const char* public_path;
};

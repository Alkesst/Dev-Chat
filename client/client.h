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

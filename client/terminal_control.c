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
#include "client.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

static void enable_canon(struct Client* client);
static void disable_canon(struct Client* client);
static inline void move_left(int pos_to_move);
static inline void move_right(int pos_to_move);

static void enable_canon(struct Client* client){
    tcsetattr(STDIN_FILENO, TCSANOW, &client->backup);
}

static void disable_canon(struct Client* client){
    tcgetattr(STDIN_FILENO, &client->backup);
    struct termios backup_copy = client->backup;
    backup_copy.c_lflag &= (~ICANON);
    backup_copy.c_lflag &= (~ECHO);
    backup_copy.c_cc[VTIME] = 0;
    backup_copy.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &backup_copy);
}

void print_to_terminal(struct Client* client, const char* message, size_t message_len){
    printf("\r");
    printf("%.*s", (int) message_len - 1, message);
    if(client->username != NULL){
        for(size_t cont = message_len; cont < (5 + strlen(client->username) + client->message_len); cont++){
            putc(' ', stdout);
        }
        fflush(stdout);
    }
    printf("\n");
    if(client->username != NULL){
        printf("(%s): ", client->username);
        fflush(stdout);
        printf("%.*s", (int) client->message_len, client->message_read);
        if(client->cursor_pos < client->message_len) move_left(client->message_len - client->cursor_pos);
        else fflush(stdout);
    }
}


int read_from_terminal(struct Client* client, char** message){
    int process_successful = 0;
    client->message_len = 0;
    client->cursor_pos = 0;
    char current_char;
    client->message_read = *message = malloc(1001);
    disable_canon(client);
    do{
        int a = current_char = getc(stdin);
        if(a == 255 || a == EOF){
            process_successful = -1;
        } else if(current_char == 27){
            current_char = getc(stdin);
            if (current_char == 91) {
                current_char = getc(stdin);
                if(current_char == 'D' && client->cursor_pos > 0){
                    move_left(1);
                    client->cursor_pos--;
                } else if(current_char == 'C' && client->cursor_pos < client->message_len){
                    move_right(1);
                    client->cursor_pos++;
                }
            }
        } else if(current_char == 127){
            if(client->cursor_pos > 0){
                if(client->cursor_pos < client->message_len){
                    memmove(&(*message)[client->cursor_pos - 1], &(*message)[client->cursor_pos], client->message_len - client->cursor_pos);
                }
                client->message_len--;
                client->cursor_pos--;
                move_left(client->cursor_pos + 1);
                printf("%.*s ", (int) client->message_len, *message);
                move_left(client->message_len - client->cursor_pos + 1);
            }
        } else if(current_char == 4 && client->message_len == 0) {
            process_successful = -1;
        } else if (current_char >= 32){
            memmove(&(*message)[client->cursor_pos + 1], &(*message)[client->cursor_pos], client->message_len - client->cursor_pos);
            (*message)[client->cursor_pos] = current_char;
            client->cursor_pos++; client->message_len++;
            if(client->cursor_pos > 1) move_left(client->cursor_pos - 1);
            printf("%.*s", (int) client->message_len, *message);
            if(client->cursor_pos < client->message_len) move_left(client->message_len - client->cursor_pos);

        }
        else if(current_char == '\n') {
            putc('\n', stdout);
            (*message)[client->message_len] = '\n';
            client->message_len++;
        }
    } while(client->message_len < 999 && current_char != '\n' && process_successful == 0);
    if(client->message_len == 999){
        (*message)[client->message_len] = '\n';
        client->message_len++;
    }
    (*message)[client->message_len] = '\0';
    enable_canon(client);
    return process_successful;
}

static inline void move_left(int pos_to_move){
    if(pos_to_move > 0){
        printf("\033[%dD", pos_to_move);
        fflush(stdout);
    }
}

static inline void move_right(int pos_to_move){
    if(pos_to_move > 0){
        printf("\033[%dC", pos_to_move);
        fflush(stdout);
    }
}

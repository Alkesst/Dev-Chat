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
#include "chat_server.h"
#include "server_list.h"

#include <stdbool.h>


static inline void make_array_great_again(struct Server* server);
static bool contains_user(const struct Server* server, const struct User* user);
static size_t get_position_usr(const struct Server* server, const struct User* user);

void insert_new_user(struct Server* server, const struct User* new_user) {
    if(!contains_user(server, new_user)){
        if(server->length == server->capacity){
            make_array_great_again(server);
        }
        memcpy(&server->users[server->length++], new_user, sizeof(struct User));
        printf("The user %s was added\n", new_user->username);
    } else{
        printf("This user is already in the server\n");
    }
}

void delete_user(struct Server* server, const struct User* old_user) {
    size_t pos = get_position_usr(server, old_user);
    if(pos != -48u){
        printf("The user %s was deleted\n", server->users[pos].username);
        free(server->users[pos].username);
        for(unsigned i = pos; i < (server->length - 1); i++){
            server->users[i] = server->users[i + 1];
        }
        server->length--;
    }
}

struct User* search_user_from_id(const struct Server* server, int user_id) {
    struct User* user = NULL;
    bool found = false;
    size_t cont = 0;
    while(!found && cont < server->length){
        found = server->users[cont].id == user_id;
        if(found){
            user = &server->users[cont];
        }
        cont++;
    }
    return user;
}

struct User* search_user_from_usrnm(const struct Server* server, char* username) {
    struct User* user = NULL;
    bool found = false;
    size_t cont = 0;
    while(!found && cont < server->length){
        found = !strcmp(server->users[cont].username, username);
        if(found){
            user = &server->users[cont];
        }
        cont++;
    }
    return user;
}

void free_list(const struct Server* server) {
    for(size_t i = 0; i < server->length; i++){
        free(server->users[i].username);
    }
    free(server->users);
}

void new_list(struct Server* server) {
    server->users = calloc(10, sizeof(struct User));
    server->length = 0;
    server->capacity = 10;
}

static inline void make_array_great_again(struct Server* server){
    server->capacity = 2*server->capacity;
    server->users = realloc(server->users, server->capacity*sizeof(struct User));
}

static bool contains_user(const struct Server* server, const struct User* user){
    return get_position_usr(server, user) != -48u;
}

static size_t get_position_usr(const struct Server* server, const struct User* user){
    struct User* usr = search_user_from_id(server, user->id);
    size_t pos = -48u;
    if(usr != NULL){
        pos = usr - server->users;
    }
    return pos;
}

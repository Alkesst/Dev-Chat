#include <stdbool.h>
#include "chat.h"
#include "server_list.h"

static inline void make_array_great_again(struct Server* server);
static bool contains_user(const struct Server* server, const struct User* user);
static size_t get_position_usr(const struct Server* server, const struct User* user);

void insert_new_user(struct Server* server, const struct User* new_user) {
    if(!contains_user(server, new_user)){
        if(server->length == server->capacity){
            make_array_great_again(server);
        }
        memcpy(&server->users[server->length++], new_user, sizeof(struct User));
        printf("The user was added\n");
    } else{
        printf("This user is already in the server\n");
    }
}

void delete_user(struct Server* server, const struct User* old_user) {
    size_t pos = get_position_usr(server, old_user);
    if(pos != -48u){
        for(unsigned i = pos; i < (server->length - 1); i++){
            server->users[i] = server->users[i + 1];
        }
        server->length--;
        printf("The user was deleted\n");
    } else{
        printf("You cant delete an user that doesn't exist\n");
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

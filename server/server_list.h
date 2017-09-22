#include "chat.h"

void insert_new_user(struct Server* server, const struct User* new_user);

void delete_user(struct Server* server, const struct User* old_user);

struct User* search_user_from_id(const struct Server* server, int user_id);

struct User* search_user_from_usrnm(const struct Server* server, char* username);

void free_list(const struct Server* server);

void new_list(struct Server* server);

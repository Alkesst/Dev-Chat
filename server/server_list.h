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

void insert_new_user(struct Server* server, const struct User* new_user);

void delete_user(struct Server* server, const struct User* old_user);

struct User* search_user_from_id(const struct Server* server, int user_id);

struct User* search_user_from_usrnm(const struct Server* server, char* username);

void free_list(const struct Server* server);

void new_list(struct Server* server);
